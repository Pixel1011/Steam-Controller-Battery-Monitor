#include "icons.h"
#include <ControllerFinder.h>
#include <QAction>
#include <QCoreApplication>
#include <QIcon>
#include <QMenu>
#include <QObject>
#include <QStyle>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QApplication>
#include <TritonController.h>

TritonController* c = nullptr;
ControllerFinder finder;
IconGenerator igen;

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
  QAction* statusAction = menu.addAction("Monitoring battery");
  statusAction->setEnabled(false);
  menu.addSeparator();
  QAction* quitAction = menu.addAction("Quit");
  QSystemTrayIcon trayIcon;
  trayIcon.setToolTip("SC26 Battery monitor");
  trayIcon.setContextMenu(&menu);

  QIcon nextIcon = igen.createBatteryIcon(nullptr);

  trayIcon.setIcon(igen.createBatteryIcon(nullptr));

  QObject::connect(quitAction, &QAction::triggered, &app, &QCoreApplication::quit);
  trayIcon.show();

  c = finder.getController();
  if (c != nullptr) c->startPoll();
  double counter = 0.0;


  auto updatefunc = [&]() {
    counter++;
    std::cout << (counter * 3.5) / 60.0 << std::endl;
    if (c == nullptr || c->disconnected.load()) {
      delete c;
      c = nullptr;
      TritonController* cont = finder.getController();
      if (cont == nullptr) {
        nextIcon = igen.createBatteryIcon(nullptr);
        if (trayIcon.icon().cacheKey() != nextIcon.cacheKey()) trayIcon.setIcon(nextIcon);
        statusAction->setText(QString("Steam Controller disconnected."));
        trayIcon.setToolTip(QString("Steam Controller disconnected."));
        return;
      }
      c = cont;
      c->startPoll();
      return;
    }
    TritonBatteryStatus_t batt = c->getBatteryStatus();
    QString str = QString("Steam Controller battery: %1%").arg(batt.ucBatteryLevel);

    if (statusAction->toolTip() != str) statusAction->setText(QString("Steam Controller battery: %1%").arg(batt.ucBatteryLevel));
    if (trayIcon.toolTip() != str) trayIcon.setToolTip(QString("Steam Controller battery: %1%").arg(batt.ucBatteryLevel));

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