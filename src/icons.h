#pragma once
#include <QIcon>
#include <TritonController.h>
#include <array>
#include <algorithm>

class IconGenerator
{
private:
  std::array<QIcon, 29> iconCache{};

public:
  IconGenerator();
  ~IconGenerator();

  
  // im making it all in one function i do not care
  QIcon createBatteryIcon(const TritonBatteryStatus_t* battery);
  QIcon renderIcon(int filledPixels, bool charging, bool disconnected, uint8_t batteryLevel);
};