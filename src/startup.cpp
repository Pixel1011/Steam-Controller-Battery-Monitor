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

#else
#include <QStandardPaths>
#include <QSaveFile>

namespace Startup {

const QString STARTFILE_NAME = "steam-controller-battery-monitor.desktop";

QString escapeDesktopExec(QString value) {
  value.replace(u'\\', QStringLiteral("\\\\"));
  value.replace(u'"', QStringLiteral("\\\""));
  value.replace(u'`', QStringLiteral("\\`"));
  value.replace(u'$', QStringLiteral("\\$"));
  value.replace(u'%', QStringLiteral("%%"));

  return QStringLiteral("\"%1\"").arg(value);
}

QString autostartDir() {
  // should be ~/.config/ i think, or whatever its set to
  QString configDirectory = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
  return QDir(configDirectory).filePath(QString("autostart"));
}

QString autostartFilePath() {
  return QDir(autostartDir()).filePath(STARTFILE_NAME);
}

QString autostartFileContents() {
  QString executable = QCoreApplication::applicationFilePath();

  return QString("[Desktop Entry]\n"
                 "Type=Application\n"
                 "Name=Steam Controller Battery Monitor\n"
                 "Comment=Steam Controller Battery Monitor tray app\n"
                 "Exec=%1\n"
                 "Terminal=false\n"
                 "X-GNOME-Autostart-enabled=true\n").arg(escapeDesktopExec(executable));
}

bool startsAtLogin() {
  return QFile::exists(autostartFilePath());
}

bool setStartsAtLogin(bool enabled) {
  QString path = autostartFilePath();

  if (!enabled) {
    if (!QFile::exists(path)) return true;
    return QFile::remove(path);
  }

  QDir directory;
  if (!directory.mkpath(autostartDir())) return false;

  QSaveFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

  QByteArray contents = autostartFileContents().toUtf8();

  if (file.write(contents) != contents.size()) {
    file.cancelWriting();
    return false;
  }

  return file.commit();
}

}

#endif