#include "icons.h"
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPixmapCache>

IconGenerator::IconGenerator() {
  QPixmapCache::setCacheLimit(24);
}

IconGenerator::~IconGenerator() {}

// i hate all of this please help someone
// this is awful, i hate image gen code, its just numbers with barely any visualisation

QIcon IconGenerator::createBatteryIcon(const TritonBatteryStatus_t* battery) {

  bool disconnected = battery == nullptr || (battery->sSystemVoltage == 0 && battery->ucBatteryLevel == 0);

  if (disconnected) {
    QIcon& cachedIcon = iconCache[28];
    if (cachedIcon.isNull()) {
      cachedIcon = renderIcon(0, false, disconnected, 0);
    }
    return cachedIcon;
  }

  int perc = std::clamp(static_cast<int>(battery->ucBatteryLevel), 0, 100);

  int filledPixels = (static_cast<int>(perc) * 13) / 100;
  bool charging = battery->ucChargeState == EChargeState::k_EChargeStateCharging;

  int cacheIdx = filledPixels;
  if (charging) cacheIdx += 14;

  QIcon& cachedIcon = iconCache[cacheIdx];
  if (cachedIcon.isNull()) {
    cachedIcon = renderIcon(filledPixels, charging, disconnected, perc);
  }
  return cachedIcon;
}

QIcon IconGenerator::renderIcon(int filledPixels, bool charging, bool disconnected, uint8_t batteryLevel) {

  const double maxFilledPixels = 13.0;

  QIcon icon;
  double scale = 0.0625;
  QPixmap pixmap(16, 16);
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing);

  // 256x256 scale

  const QRectF body(7.0 * scale, 48.0 * scale, 221.0 * scale, 160.0 * scale);
  const QRectF terminal(228.0 * scale, 96.0 * scale, 22.0 * scale, 64.0 * scale);

  const double outlineWidth = 12.0 * scale;
  const double cornerRadius = 20.0 * scale;

  painter.setPen(Qt::NoPen);
  painter.setBrush(Qt::black);
  // draw nub thing on end
  painter.drawRoundedRect(terminal, 7.0 * scale, 7.0 * scale);

  const QRectF interior = body.adjusted(6.0 * scale, 6.0 * scale, -6.0 * scale, -6.0 * scale);
  QPainterPath interiorPath;
  interiorPath.addRoundedRect(interior, 10.0 * scale, 10.0 * scale);

  // draw inside
  if (!disconnected && filledPixels > 0) {
    painter.save();
    painter.setClipPath(interiorPath);

    QColor levelColour = QColor(0, 255, 35);
    if (batteryLevel <= 0.1) levelColour = QColor(125, 0, 0);
    double level = static_cast<double>(filledPixels) / maxFilledPixels;

    painter.fillRect(QRectF(interior.left(), interior.top(), interior.width() * level, interior.height()), levelColour);
    painter.restore();
  }

  // draw outside
  painter.setBrush(Qt::NoBrush);
  painter.setPen(QPen(Qt::black, outlineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

  painter.drawRoundedRect(body, cornerRadius, cornerRadius);

  if (disconnected) {
    painter.setPen(QPen(Qt::red, outlineWidth, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(20 * scale, 240 * scale, 240 * scale, 20 * scale);
  }

  if (!disconnected && charging) {
    QPainterPath bolt;

    bolt.moveTo(60.0 * scale, 15.0 * scale);
    bolt.lineTo(18.0 * scale, 135.0 * scale);
    bolt.lineTo(45.0 * scale, 135.0 * scale);
    bolt.lineTo(32.0 * scale, 240.0 * scale);
    bolt.lineTo(88.0 * scale, 105.0 * scale);
    bolt.lineTo(58.0 * scale, 105.0 * scale);
    bolt.closeSubpath();

    painter.save();
    painter.setOpacity(0.7);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawPath(bolt);
    painter.restore();
  }

  painter.end();
  // pixmap.save("output.png", "PNG");
  return QIcon(pixmap);
}
