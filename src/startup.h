#pragma once
#include <QCoreApplication>
#include <QDir>
#include <QString>
#ifdef _WIN32
#include <QSettings>
#endif

namespace Startup {
  bool startsAtLogin();
  bool setStartsAtLogin(bool enabled);
};