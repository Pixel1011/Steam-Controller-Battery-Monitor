#pragma once
#include <mutex>
#include <thread>
#include <atomic>
#include <hidapi.h>
#include <cstdint>
#define HID_FEATURE_REPORT_BYTES 64
// have not tested 0x87 and 0x89. i could be completely wrong about them
#pragma region structs
#pragma pack(push, 1)

/*Structs taken directly from SDL code*/
// ill just add this to not be shot
/*
  Simple DirectMedia Layer
  Copyright (C) 2020 Valve Corporation

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

// feature report commands
// Header for all host <==> target messages
typedef struct
{
  unsigned char type;
  unsigned char length;
} FeatureReportHeader;

// Generic controller settings structure
typedef struct
{
  unsigned char settingNum;
  unsigned short settingValue;
} ControllerSetting;

// Generic controller attribute structure
typedef struct
{
  unsigned char attributeTag;
  uint32_t attributeValue;
} ControllerAttribute;

// Generic controller settings structure
typedef struct
{
  ControllerSetting settings[(HID_FEATURE_REPORT_BYTES - sizeof(FeatureReportHeader)) / sizeof(ControllerSetting)];
} MsgSetSettingsValues, MsgGetSettingsValues, MsgGetSettingsDefaults, MsgGetSettingsMaxs;

// Generic controller settings structure
typedef struct
{
  ControllerAttribute attributes[(HID_FEATURE_REPORT_BYTES - sizeof(FeatureReportHeader)) / sizeof(ControllerAttribute)];
} MsgGetAttributes;

typedef struct
{
  unsigned char attributeTag;
  char attributeValue[20];
} MsgGetStringAttribute;

typedef struct
{
  unsigned char mode;
} MsgSetControllerMode;

typedef struct
{
  FeatureReportHeader header;
  union {
    MsgSetSettingsValues setSettingsValues;
    MsgGetSettingsValues getSettingsValues;
    MsgGetSettingsMaxs getSettingsMaxs;
    MsgGetSettingsDefaults getSettingsDefaults;
    MsgGetAttributes getAttributes;
    MsgSetControllerMode controllerMode;
    MsgGetStringAttribute getStringAttribute;
  } payload;

} FeatureReportMsg;

// hid commands

enum EChargeState {
  k_EChargeStateReset,
  k_EChargeStateDischarging,
  k_EChargeStateCharging,
  k_EChargeStateSrcValidate,
  k_EChargeStateChargingDone,
};

enum ETritonReportIDTypes {
  ID_TRITON_CONTROLLER_STATE = 0x42,
  ID_TRITON_BATTERY_STATUS = 0x43,
  ID_TRITON_CONTROLLER_STATE_BLE = 0x45,
  ID_TRITON_WIRELESS_STATUS_X = 0x46,
  ID_TRITON_CONTROLLER_STATE_TIMESTAMP = 0x47,

  ID_TRITON_WIRELESS_STATUS = 0x79,
};

enum ETritonWirelessState {
  k_ETritonWirelessStateDisconnect = 1,
  k_ETritonWirelessStateConnect = 2,
};

typedef enum {

  TRITON_LBUTTON_A = 0x00000001,
  TRITON_LBUTTON_B = 0x00000002,
  TRITON_LBUTTON_X = 0x00000004,
  TRITON_LBUTTON_Y = 0x00000008,

  TRITON_HBUTTON_QAM = 0x00000010,
  TRITON_LBUTTON_R3 = 0x00000020,
  TRITON_LBUTTON_VIEW = 0x00000040,
  TRITON_HBUTTON_R4 = 0x00000080,

  TRITON_LBUTTON_R5 = 0x00000100,
  TRITON_LBUTTON_R = 0x00000200,
  TRITON_LBUTTON_DPAD_DOWN = 0x00000400,
  TRITON_LBUTTON_DPAD_RIGHT = 0x00000800,

  TRITON_LBUTTON_DPAD_LEFT = 0x00001000,
  TRITON_LBUTTON_DPAD_UP = 0x00002000,
  TRITON_LBUTTON_MENU = 0x00004000,
  TRITON_LBUTTON_L3 = 0x00008000,

  TRITON_LBUTTON_STEAM = 0x00010000,
  TRITON_HBUTTON_L4 = 0x00020000,
  TRITON_LBUTTON_L5 = 0x00040000,
  TRITON_LBUTTON_L = 0x00080000,

  TRITON_RIGHT_JOYSTICK_TOUCH = 0x00100000,
  TRITON_RIGHT_TOUCHPAD_TOUCH = 0x00200000,
  TRITON_RIGHT_TOUCHPAD_CLICK = 0x00400000,
  TRITON_RIGHT_TRIGGER_CLICK = 0x00800000,

  TRITON_LEFT_JOYSTICK_TOUCH = 0x01000000,
  TRITON_LEFT_TOUCHPAD_TOUCH = 0x02000000,
  TRITON_LEFT_TOUCHPAD_CLICK = 0x04000000,
  TRITON_LEFT_TRIGGER_CLICK = 0x08000000,

  TRITON_RIGHT_GRIP_TOUCH = 0x10000000,
  TRITON_LEFT_GRIP_TOUCH = 0x20000000,
} TritonButtons;

typedef enum {
  RUMBLE = 0x80,
  PULSE = 0x81,
  COMMAND = 0x82,
  LFO_TONE = 0x83,
  LOG_SWEEP = 0x84,
  HAPTIC_SCRIPT = 0x85,
  // guesses, cannot confirm names directly from SDL or otherwise

  PCM_MODE = 0x86,
  PCM_MONO = 0x87,
  PCM_STEREO = 0x88,
  PCM_MONO_WITH_LENGTH = 0x89,
} TritonReportIDs;

typedef struct
{
  uint8_t type;
  uint16_t intensity;
  struct
  {
    uint16_t speed;
    int8_t gain;
  } left, right;
} MsgHapticRumble;

typedef struct
{
  uint8_t side;
  uint16_t on_us;
  uint16_t off_us;
  uint16_t repeat_count;
  uint16_t gain_db;
} MsgHapticPulse;

typedef struct
{
  uint8_t side;
  uint8_t command;
  int8_t gain_db;
} MsgHapticCommand;

typedef struct
{
  uint8_t side;
  int8_t gain_db;
  uint16_t frequency;
  uint16_t duration_ms;
  uint16_t lfo_freq;
  uint8_t lfo_depth;
} MsgHapticLfoTone;

typedef struct
{
  uint8_t side;
  int8_t gain_db;
  uint16_t duration_ms;
  struct
  {
    uint16_t frequency;
  } start, end;
} MsgHapticLogSweep;

typedef struct
{
  uint8_t side;
  uint8_t script_id;
  int8_t gain_db;
} MsgHapticScript;

//14
typedef struct
{
  unsigned char ucChargeState; // EChargeState
  unsigned char ucBatteryLevel;
  unsigned short sBatteryVoltage;
  unsigned short sSystemVoltage;
  unsigned short sInputVoltage;
  unsigned short sCurrent;
  unsigned short sInputCurrent;
  unsigned short sTemperature;
} TritonBatteryStatus_t;

//1
typedef struct
{
  unsigned char state;
} TritonWirelessStatus_t;

//24
typedef struct
{
  uint32_t timestamp;
  short sAccelX;
  short sAccelY;
  short sAccelZ;

  short sGyroX;
  short sGyroY;
  short sGyroZ;

  short sGyroQuatW;
  short sGyroQuatX;
  short sGyroQuatY;
  short sGyroQuatZ;
} TritonMTUIMU_t;

//16
typedef struct {
  uint32_t timestamp;
  short sAccelX;
  short sAccelY;
  short sAccelZ;

  short sGyroX;
  short sGyroY;
  short sGyroZ;
} TritonMTUIMUNoQuat_t;

//14
typedef struct
{
  uint16_t timestamp;
  short sAccelX;
  short sAccelY;
  short sAccelZ;

  short sGyroX;
  short sGyroY;
  short sGyroZ;
} TritonMTUIMUNoQuat32usTS_t;

//29+24 53
typedef struct
{
  uint8_t seq_num;
  uint32_t buttons;
  short sTriggerLeft;
  short sTriggerRight;

  short sLeftStickX;
  short sLeftStickY;
  short sRightStickX;
  short sRightStickY;

  short sLeftPadX;
  short sLeftPadY;
  unsigned short unPressureLeft;

  short sRightPadX;
  short sRightPadY;
  unsigned short unPressureRight;
  TritonMTUIMU_t imu;
} TritonMTUFull_t;

//45
typedef struct {
  uint8_t seq_num;
  uint32_t buttons;
  short sTriggerLeft;
  short sTriggerRight;

  short sLeftStickX;
  short sLeftStickY;
  short sRightStickX;
  short sRightStickY;

  short sLeftPadX;
  short sLeftPadY;
  unsigned short unPressureLeft;

  short sRightPadX;
  short sRightPadY;
  unsigned short unPressureRight;
  TritonMTUIMUNoQuat_t imu;
} TritonMTUNoQuat_t;

// New Ibex packet that adds a timestamp to the trackpad sampling
// and reduces the size of the IMU timestamp.  Timestamps are now 16 bits

//45
typedef struct
{
  uint8_t seq_num;
  uint32_t buttons;
  short sTriggerLeft;
  short sTriggerRight;

  short sLeftStickX;
  short sLeftStickY;
  short sRightStickX;
  short sRightStickY;

  unsigned short unTrackpadTimestamp;
  short sLeftPadX;
  short sLeftPadY;
  unsigned short unPressureLeft;

  short sRightPadX;
  short sRightPadY;
  unsigned short unPressureRight;

  TritonMTUIMUNoQuat32usTS_t imu;
} TritonMTUNoQuat32TS_t;
// end of sdl stuff



enum class TritonPCMMode {
  Khz8_16Bit,
  Khz4_16Bit,
  Khz2_16Bit,
  Khz1_16Bit,
  
  Khz8_8Bit,
  Khz4_8Bit,
  Khz2_8Bit,
  Khz1_8Bit,
  
  Khz8_8Bit_ulaw,
  Khz4_8Bit_ulaw,
  Khz2_8Bit_ulaw,
  Khz1_8Bit_ulaw
};

enum class TritonPCMOperation {
  DISABLE = 1,
  ENABLE = 2
};

enum class TritonInterface {
  PUCK,
  WIRED
};
/*Structs found via RE*/

typedef struct
{
  // still dont quite know what any of these 3 do, however these are best guesses
  uint8_t operation;
  uint8_t side;
  uint8_t param; // i believe this is some sort of bitfield or Enum, no idea.
} MsgHapticPCMMode;

typedef struct
{
  uint8_t side;
  char data[62];
} MsgHapticPCMMono;

typedef struct
{
  // <=31
  uint8_t length;

  char left[31];
  char right[31];

} MsgHapticPCMStereo;

typedef struct
{
  uint8_t length;
  uint8_t side;
  char data[61];
} MsgHapticPCMMonoWithLength;

#pragma pack(pop)
#pragma endregion

// yes im kinda turning this into a general purpose class for the sc2
// idk im bored
class TritonController {
private:
  hid_device* hid_handle;

  std::atomic<bool> running = false;
  std::thread pollThread;
  std::mutex stateMutex;
  TritonMTUFull_t _state;
  
  std::mutex batteryMutex;
  TritonBatteryStatus_t _battery;
  
  void pollLoop();
  
  public:
  std::atomic<uint64_t> stateCounter{0};
  std::atomic<uint64_t> batteryCounter{0};
  TritonInterface connectionType;

  // use to check if the controller has disconnected, if true, delete this class and use controller finder to try connect again
  bool disconnected = false;
  
  TritonController(hid_device* handle, TritonInterface connection);
  ~TritonController();
  void close();
  int playNote(int channel, int note, int velocity);
  int playFrequency(int channel, double frequency, int velocity);
  int sendPCMMode(MsgHapticPCMMode* packet);

  // maybe make a function that can just take in a mode and massive byte buffer and play it via another thread so execution can still occurs
  int sendPCMStereo(MsgHapticPCMStereo* packet);
  int sendRaw(uint8_t bytes[], size_t length);
  void setupPCMStreaming(TritonPCMMode mode);
  // reading
  void startPoll();
  void stopPoll();
  int readRaw(uint8_t buff[], size_t length);

  TritonMTUFull_t getFullReport();
  TritonBatteryStatus_t getBatteryStatus();
  bool isPressed(TritonButtons btn);
  // maybe i do pressed/released event queue if bored
};