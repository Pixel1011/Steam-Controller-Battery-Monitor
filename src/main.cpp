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
#include <Qapplication>
#include <TritonController.h>

TritonController* c = nullptr;
ControllerFinder finder;
IconGenerator igen;

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  app.setQuitOnLastWindowClosed(false);

  if (!QSystemTrayIcon::isSystemTrayAvailable()) {
    qCritical() << "No system tray is available";
    return 1;
  }

  QMenu menu;
  QAction* statusAction = menu.addAction("Monitoring battery");
  statusAction->setEnabled(false);
  menu.addSeparator();
  QAction* quitAction = menu.addAction("Quit");
  QSystemTrayIcon trayIcon;
  trayIcon.setToolTip("SC26 Battery monitor");
  trayIcon.setContextMenu(&menu);
  trayIcon.setIcon(igen.createBatteryIcon(nullptr));

  QObject::connect(quitAction, &QAction::triggered, &app, &QCoreApplication::quit);
  trayIcon.show();
  // will need to make this go on loop if there is no controller, as this will probably start with the pc for convenience, also could end up being memory leak hell
  c = finder.getController();
  if (c != nullptr) c->startPoll();

  auto updatefunc = [&]() {
    if (c == nullptr || c->disconnected.load()) {
      delete c;
      c = nullptr;
      TritonController* cont = finder.getController();
      if (cont == nullptr) {
        trayIcon.setIcon(igen.createBatteryIcon(nullptr));
        statusAction->setText(QString("Steam Controller disconnected."));
        trayIcon.setToolTip(QString("Steam Controller disconnected."));
        return;
      }
      c = cont;
      c->startPoll();
      return;
    }
    TritonBatteryStatus_t batt = c->getBatteryStatus();

    statusAction->setText(QString("Steam Controller battery: %1%").arg(batt.ucBatteryLevel));

    trayIcon.setToolTip(QString("Steam Controller battery: %1%").arg(batt.ucBatteryLevel));
    trayIcon.setIcon(igen.createBatteryIcon(&batt));
  };

  QTimer update;
  QObject::connect(&update, QTimer::timeout, &app, updatefunc);
  update.start(3500);

  return app.exec();
}