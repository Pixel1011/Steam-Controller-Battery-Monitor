#include "startup.h"

#ifdef _WIN32

namespace Startup {

const QString APP_REGISTRY_NAME = "Steam-Controller-Battery-Monitor";
const QString REGISTRY_KEY = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";


QSettings getSettings() {
  return QSettings(REGISTRY_KEY, QSettings::NativeFormat);
}


QString startupCommand() {
  QString executable = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
  return QStringLiteral("\"%1\"").arg(executable);
}

bool startsAtLogin() {
  QSettings settings = getSettings();
  return settings.value(APP_REGISTRY_NAME).toString() == startupCommand();
}

bool setStartsAtLogin(bool enabled) {
  QSettings settings = getSettings();

  if (enabled) {
    settings.setValue(APP_REGISTRY_NAME, startupCommand());
  } else {
    settings.remove(APP_REGISTRY_NAME);
  }

  settings.sync();
  return settings.status() == QSettings::NoError;
}

} // namespace

#elif defined(__linux__)

namespace Startup {
  bool startsAtLogin() {
    return false;
  }
  bool setStartsAtLogin(bool enabled) {
    return false;
  }
}

#endif