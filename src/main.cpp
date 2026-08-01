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
  trayIcon.setToolTip("SC Battery mon");
  trayIcon.setContextMenu(&menu);
  trayIcon.setIcon(app.style()->standardIcon(QStyle::SP_ComputerIcon));

  QObject::connect(quitAction, &QAction::triggered, &app, &QCoreApplication::quit);
  trayIcon.show();
  // will need to make this go on loop if there is no controller, as this will probably start with the pc for convenience, also could end up being memory leak hell
  TritonController* c = finder.getController();
  if (c == nullptr) {
    return 1;
  }
  c->startPoll();

  
  auto updatefunc = [&]() {
    TritonBatteryStatus_t batt = c->getBatteryStatus();

    statusAction->setText(QString("Battery: %1%").arg(batt.ucBatteryLevel));

    trayIcon.setToolTip(QString("Steam Controller battery: %1%").arg(batt.ucBatteryLevel));
  };

  QTimer update;
  QObject::connect(&update, QTimer::timeout, &app, updatefunc);
  update.start(5000);

  return app.exec();
}