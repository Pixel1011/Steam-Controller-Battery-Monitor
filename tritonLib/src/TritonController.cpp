#include "TritonController.h"
#include "Constants.h"
#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <thread>

TritonController::TritonController(hid_device* handle, TritonInterface connection) {
  this->hid_handle = handle;
  this->connectionType = connection;
}

TritonController::~TritonController()
{
  this->stopPoll();
  this->close();
}

void TritonController::close() {
  hid_close(hid_handle);
}

// these 2 are basically the only thing stolen from SteamHapticsSinger
int TritonController::playNote(int channel, int note, int velocity) {
  double frequency = midiFrequency[note];
  return playFrequency(channel, frequency, velocity);
}

int TritonController::playFrequency(int channel, double frequency, int velocity) {
  MsgHapticLfoTone packet{};
  if (frequency == -1) {
    // This prevents the controller from rebooting when using rumble motors and drifting out of tune
    packet.side = channel;
  } else {
    int frequencyValue = static_cast<int>(frequency);
    packet.side = channel;
    packet.gain_db = velocity;
    packet.frequency = frequencyValue;
    packet.duration_ms = 0xff64;
    packet.lfo_freq = 0;
    packet.lfo_depth = 0;
  }

  constexpr size_t size = sizeof(MsgHapticLfoTone);

  unsigned char buff[size + 1] = {0};
  buff[0] = LFO_TONE;
  memcpy(&buff[1], &packet, size);
  return sendRaw(buff, sizeof(buff));
}

int TritonController::sendPCMMode(MsgHapticPCMMode* packet) {
  constexpr size_t size = sizeof(MsgHapticPCMMode);

  unsigned char buff[size + 1];
  buff[0] = PCM_MODE;
  memcpy(&buff[1], packet, size);
  return sendRaw(buff, sizeof(buff));
}

int TritonController::sendPCMStereo(MsgHapticPCMStereo* packet) {
  constexpr size_t size = sizeof(MsgHapticPCMStereo);

  unsigned char buff[size + 1] = {0};
  buff[0] = PCM_STEREO;
  memcpy(&buff[1], packet, size);
  return sendRaw(buff, sizeof(buff));
}

// will return -1 if controller disconnects, otherwise length of bytes read
int TritonController::sendRaw(uint8_t packet[], size_t length) {
  int r = hid_write(this->hid_handle, packet, length);
  if (r < 0) {
    printf("Send Error, hid_error: %ls\n", hid_error(this->hid_handle));
    stopPoll();
    close();
    return -1;
  }
  return r;
}

// will return -1 if controller disconnects, otherwise length of bytes read
int TritonController::readRaw(uint8_t buff[], size_t length) {
  int r = hid_read(this->hid_handle, buff, length);
  if (r < 0) {
    printf("Read Error, hid_error: %ls\n", hid_error(this->hid_handle));
    stopPoll();
    close();
    return -1;
  }
  return r;
}

TritonMTUFull_t TritonController::getFullReport() {
  std::lock_guard<std::mutex> lock(stateMutex);
  return _state;
}

TritonBatteryStatus_t TritonController::getBatteryStatus() {
  std::lock_guard<std::mutex> lock(batteryMutex);
  return _battery;
}

bool TritonController::isPressed(TritonButtons btn) {
  std::lock_guard<std::mutex> lock(stateMutex);
  return (_state.buttons & btn) != 0;
}

void TritonController::setupPCMStreaming(TritonPCMMode mode) {
  // old implementation
  /*//std::cout << "Running setup for pcm streaming! You may hear some weird noises" << std::endl;
  // not entirely sure how these 2 actually make the pcm streaming work
  // channels:
  // 0 - only left audio plays
  // 1 - only right audio plays
  // 3-5 on their own, nothing plays
  // but then 0,3,4,5 seems to sound okay??
  // 0xff param i believe causes the steam controller firmware to crash (bitmask?)
  // idk im just leaving it at this to not risk audio quality, which they do seem to affect from testing
  // only needs to be setup once though per restart of the controller. then any pcm streamed to it will play just fine

  uint8_t channels[] = {0};
  uint8_t params[] = {4};
  int reps = 0;

  int totalSteps = (sizeof(channels) / sizeof(channels[0])) * (sizeof(params) / sizeof(params[0])) * reps;
  std::string aou = "Setup: ";
  Utils::ProgressHelper helper(totalSteps, &aou, 1, Utils::Mode::PROGRESSBAR);

  // send one 0
 // MsgHapticPCMMode modePacket;
 // modePacket.operation = 0x02;
 // modePacket.side = 0;
 // modePacket.param = 0;

 // sendPCMMode(&modePacket);


  for (uint8_t ch : channels) {
    for (uint8_t p : params) {
      MsgHapticPCMMode modePacket;
      modePacket.operation = 0x02;
      modePacket.side = ch;
      modePacket.param = p;

      sendPCMMode(&modePacket);
      for (int rep = 0; rep < reps; rep++) {
        MsgHapticPCMStereo packet;
        packet.length = 31;

        for (int i = 0; i < 31; i++) {
          packet.left[i] = 0x00;
          packet.right[i] = 0x00;
        }
        sendPCMStereo(&packet);
        helper.step();
        // fun fact, windows likes to lie if it spent 1ms waiting when in reality it was spending >10ms
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        }
        }
        }
        std::cout << std::endl
        << "Setup finished."
        << std::endl
        << "If your audio sounds good, you can skip this step with -s"
        << std::endl;*/
  // new imp from data by iczero (ty :D)

  // play on all actuators
  // i would make this an enum but i still think its wrong, 0,1 is clearly playing on internal haptics
  // 3,4 sounds like left tp and right internal, no idea
  // 0 TP_LEFT, 1 TP_RIGHT, 3 INT_LEFT, 4 INT_RIGHT

  uint8_t channels[] = {2, 5};

  for (uint8_t ch : channels) {
    MsgHapticPCMMode packetDisable;
    packetDisable.operation = static_cast<uint8_t>(TritonPCMOperation::DISABLE);
    packetDisable.side = ch;
    packetDisable.param = 0;

    sendPCMMode(&packetDisable);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  for (uint8_t ch : channels) {
    MsgHapticPCMMode packet;
    packet.operation = static_cast<uint8_t>(TritonPCMOperation::ENABLE);
    packet.side = ch;
    packet.param = static_cast<uint8_t>(mode);
    sendPCMMode(&packet);
  }
}

void TritonController::startPoll() {
  if (pollThread.joinable()) return;
  running.store(true);
  pollThread = std::thread([this]() {
    pollLoop();
  });
}

void TritonController::stopPoll() {
  running.store(false);
  if (pollThread.joinable()) pollThread.join();
}

void TritonController::pollLoop() {
  while (running.load()) {
    uint8_t buff[64] = {0};
    int read = readRaw(buff, 64);
    if (read == 0) continue;
    uint8_t reportID = buff[0];

    // remove report id;
    std::memmove(buff, buff + 1, 63);

    switch (static_cast<ETritonReportIDTypes>(reportID)) {
      case ID_TRITON_CONTROLLER_STATE:
        {
          TritonMTUFull_t packet{};
          memcpy(&packet, buff, 53);
          std::lock_guard<std::mutex> lock(stateMutex);
          this->_state = packet;
          stateCounter++;
          break;
        }
      case ID_TRITON_BATTERY_STATUS:
        {
          TritonBatteryStatus_t battPacket{};
          memcpy(&battPacket, buff, 14);
          std::lock_guard<std::mutex> lock(batteryMutex);
          this->_battery = battPacket;
          batteryCounter++;
          break;
        }
      case ID_TRITON_CONTROLLER_STATE_BLE:
      case ID_TRITON_CONTROLLER_STATE_TIMESTAMP:
      case ID_TRITON_WIRELESS_STATUS:
      case ID_TRITON_WIRELESS_STATUS_X:
      default:
        break;
    }
  }
}
