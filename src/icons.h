#pragma once
#include <QIcon>
#include <TritonController.h>
#include <array>
#include <algorithm>

class IconGenerator
{
private:
  // cache required otherwise setIcon will increase in memory over time (assuming it has it's own cache or something)
  std::array<QIcon, 29> iconCache{};
  bool isDarkmode;
public:
  IconGenerator();
  ~IconGenerator();

  QIcon createBatteryIcon(const TritonBatteryStatus_t* battery);
  QIcon renderIcon(int filledPixels, bool charging, bool disconnected, uint8_t batteryLevel);
};