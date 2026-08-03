#pragma once
#include <Qicon>
#include <TritonController.h>
#include <Qfont>

class IconGenerator
{
private:
  QFont font;
public:
  IconGenerator(/* args */);
  ~IconGenerator();

  
  // im making it all in one function i do not care
  QIcon createBatteryIcon(const TritonBatteryStatus_t* battery);
};