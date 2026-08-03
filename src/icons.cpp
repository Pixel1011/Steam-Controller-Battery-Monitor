#include "icons.h"
#include <QPainter>
#include <QPainterPath>
#include <QPen>
IconGenerator::IconGenerator() {
  font = QFont("Segoe UI Symbol");
  font.setStyleHint(QFont::SansSerif);
}

IconGenerator::~IconGenerator() {}

// i hate all of this please help someone
// this is awful, i hate image gen code, its just numbers with barely any visualisation

QIcon IconGenerator::createBatteryIcon(const TritonBatteryStatus_t* battery) {
  bool disconnected = false; //, charging = false;
  double level = 0.0;
  // just in case a controller is just connected and data hasnt been filled yet
  if (battery == nullptr || (battery->sSystemVoltage == 0 && battery->ucBatteryLevel == 0)) {
    disconnected = true;
  } else {
    level = static_cast<double>(battery->ucBatteryLevel) / 100.0;
  }

  QIcon icon;

  QPixmap pixmap(256, 256);
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing);

  const QRectF body(7.0, 48.0, 221.0, 160.0);

  const QRectF terminal(228.0, 96.0, 22.0, 64.0);

  const double outlineWidth = 12.0;
  const double cornerRadius = 0; // 20.0;

  painter.setPen(Qt::NoPen);
  painter.setBrush(Qt::black);
  // draw nub thing on end
  painter.drawRoundedRect(terminal, 7.0, 7.0);

  const QRectF interior = body.adjusted(6.0, 6.0, -6.0, -6.0);
  QPainterPath interiorPath;

  interiorPath.addRoundedRect(interior, 10.0, 10.0);

  painter.save();
  painter.setClipPath(interiorPath);

  QColor levelColour = QColor(0, 255, 35);
  if (level <= 0.1) levelColour = QColor(125, 0, 0);

  // draw inside
  painter.fillRect(QRectF(interior.left(), interior.top(), interior.width() * level, interior.height()), levelColour);
  painter.restore();

  // draw outside
  painter.setBrush(Qt::NoBrush);
  painter.setPen(QPen(Qt::black, outlineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

  painter.drawRoundedRect(body, cornerRadius, cornerRadius);

  if (disconnected) {
    painter.setPen(QPen(Qt::red, outlineWidth, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(20, 240, 240, 20);
  }

  if (battery->ucChargeState == EChargeState::k_EChargeStateCharging) {
    painter.save();
    QFont f = font;
    f.setPointSize(120);
    painter.setOpacity(0.7);
    painter.setFont(f);

    QRectF chargeRect(0.0, -10.0, 100.0, 255.0);
    QFontMetricsF fm(f);
    QRectF br = fm.boundingRect(QStringLiteral("⚡"));

    QPointF origin(chargeRect.center().x() - br.width() / 2.0, chargeRect.center().y() + br.height() / 2.0 - fm.descent());

    QPainterPath path;
    path.addText(origin, f, QStringLiteral("⚡"));

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawPath(path);
    painter.restore();
  }

  painter.end();
  pixmap.save("output.png", "PNG");

  icon.addPixmap(pixmap);
  return icon;
}
