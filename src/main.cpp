#include "icons.h"
#include "startup.h"
#include <ControllerFinder.h>
#include <QAction>
#include <QApplication>
#include <QCoreApplication>
#include <QIcon>
#include <QMenu>
#include <QObject>
#include <QStyle>
#include <QSystemTrayIcon>
#include <QTimer>
#include <TritonController.h>

TritonController* c = nullptr;
ControllerFinder finder;
IconGenerator igen;

QString getStatusMessage(const TritonBatteryStatus_t* battery) {
  if (battery == nullptr) return "Status: Disconnected.";
  switch (static_cast<EChargeState>(battery->ucChargeState)) {
    case EChargeState::k_EChargeStateCharging: {
      return QString("Status: Charging.");
    }
    case EChargeState::k_EChargeStateChargingDone: {
      return QString("Status: Finished charging.");
    }
    case EChargeState::k_EChargeStateDischarging: {
      return QString("Status: Discharging.");
    }
    case EChargeState::k_EChargeStateReset: {
      // no idea
      return QString("Status: Charge state reset.");
    }
    case EChargeState::k_EChargeStateSrcValidate: {
      // absolutely no idea what this one is, guessing
      return QString("Status: Validating power source.");
    }
    default: {
      return QString("Status: Unknown state.");
    }
  }
}

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  app.setQuitOnLastWindowClosed(false);
#ifndef SANITIZER_BUILD
  if (!QSystemTrayIcon::isSystemTrayAvailable()) {
    qCritical() << "No system tray is available";
    return 1;
  }
#endif

  QMenu menu;
  QAction* percentageAction = menu.addAction("Monitoring battery");
  QAction* statusAction = menu.addAction("Status: Starting monitor...");
  percentageAction->setEnabled(false);
  statusAction->setEnabled(false);
  
  menu.addSeparator();
  QAction* startupAction = menu.addAction("Start at login");
  startupAction->setCheckable(true);
  startupAction->setChecked(Startup::startsAtLogin());
  QAction* quitAction = menu.addAction("Quit");

  QSystemTrayIcon trayIcon;
  trayIcon.setToolTip("Loading...");
  trayIcon.setContextMenu(&menu);

  QIcon nextIcon = igen.createBatteryIcon(nullptr);
  trayIcon.setIcon(nextIcon);

  QObject::connect(quitAction, &QAction::triggered, &app, &QCoreApplication::quit);
  
  QObject::connect(startupAction, &QAction::toggled, &app, [&](bool enabled) {
    bool success = Startup::setStartsAtLogin(enabled);
    if (success) return;
    // fail
    QSignalBlocker block(startupAction);
    startupAction->setChecked(!enabled);
    trayIcon.showMessage("Steam controller battery monitor", "Could not change startup setting.", QSystemTrayIcon::Warning);
  });
  
  trayIcon.show();



  c = finder.getController();

  // maybe future optimisation when i can be bothered, only read when actually needing the data, feels annoying though due to blocking and battery info only being every 3.5secs and not on request
  if (c != nullptr) c->startPoll();
  //double counter = 0.0;

  auto updatefunc = [&]() {
    //counter++;
    //std::cout << (counter * 3.5) / 60.0 << std::endl;
    if (c == nullptr || c->disconnected.load()) {
      delete c;
      c = nullptr;
      TritonController* cont = finder.getController();
      if (cont == nullptr) {
        nextIcon = igen.createBatteryIcon(nullptr);
        if (trayIcon.icon().cacheKey() != nextIcon.cacheKey()) trayIcon.setIcon(nextIcon);

        QString disabledStr = "Steam Controller disconnected.";
        QString disabledStr2 = "Status: Disconnected.";
        if (percentageAction->text() != disabledStr) percentageAction->setText(disabledStr);
        if (trayIcon.toolTip() != disabledStr) trayIcon.setToolTip(disabledStr);
        if (statusAction->text() != disabledStr2) statusAction->setText(disabledStr2);
        return;
      }
      c = cont;
      c->startPoll();
      return;
    }
    TritonBatteryStatus_t batt = c->getBatteryStatus();
    QString percStr = QString("Steam Controller battery: %1%").arg(batt.ucBatteryLevel);
    QString statusStr = getStatusMessage(&batt);
    if (percentageAction->text() != percStr) percentageAction->setText(percStr);
    if (trayIcon.toolTip() != percStr) trayIcon.setToolTip(percStr);
    if (statusAction->text() != statusStr) statusAction->setText(statusStr);

    nextIcon = igen.createBatteryIcon(&batt);
    if (trayIcon.icon().cacheKey() != nextIcon.cacheKey()) trayIcon.setIcon(nextIcon);
  };

  QTimer update;
  QObject::connect(&update, &QTimer::timeout, &app, updatefunc);
  update.start(3500);

#ifdef SANITIZER_BUILD
  QTimer::singleShot(120000, &app, &QCoreApplication::quit);
#endif

  const int res = app.exec();
  delete c;
  c = nullptr;
  return res;
}