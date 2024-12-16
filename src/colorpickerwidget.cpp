#include "ColorPickerWidget.h"
#include <QtMath>
#include <QPainterPath>

ColorPickerWidget::ColorPickerWidget(QWidget *parent)
    : QWidget(parent), currentColor(Qt::white), wheelRadius(110), currentInteractionMode(None) {
    wheelCenter = QPointF(110, 110);
    selectedWheelPoint = QPointF(wheelCenter.x() + wheelRadius, wheelCenter.y());
    selectedTrianglePoint = QPointF(110,110);
    updateTriangleVertices();
    currentColor.setHsv(0,255/2,255/2);
    trianglePixmap = QPixmap(width(), height());  // Create the pixmap with widget's width and height
    trianglePixmap.fill(Qt::transparent);  // Fill with transparency initially
}

QColor ColorPickerWidget::getSelectedColor() const {
    return currentColor;
}

void ColorPickerWidget::receiveColor(const QColor &color){
    currentColor = color;
    ColorPickerWidget::setTrianglePointFromColor(color);
    ColorPickerWidget::setWheelPointFromColor(color);
    update();
    emit colorChanged(currentColor);
}

void ColorPickerWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawColorWheel(painter);
    drawColorTriangle(painter);
    drawSelectionPoints(painter);
}

void ColorPickerWidget::mousePressEvent(QMouseEvent *event) {
    QPointF pos = event->pos();
    if (isOnWheel(pos)) {
        currentInteractionMode = Wheel;
        setHueFromWheel(pos);
    } else if (isInTriangle(pos)) {
        currentInteractionMode = Triangle;
        setSaturationLightnessFromTriangle(pos);
    }
    update();
}

void ColorPickerWidget::mouseMoveEvent(QMouseEvent *event) {
    if (currentInteractionMode == None) return;

    QPointF pos = event->pos();
    if (currentInteractionMode == Wheel && isOnWheel(pos)) {
        setHueFromWheel(pos);
    } else if (currentInteractionMode == Triangle && isInTriangle(pos)) {
        setSaturationLightnessFromTriangle(pos);
    }
    update();
}

void ColorPickerWidget::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    currentInteractionMode = None;
}

bool ColorPickerWidget::isOnWheel(const QPointF &pos) const {
    double distance = qSqrt(qPow(pos.x() - wheelCenter.x(), 2) + qPow(pos.y() - wheelCenter.y(), 2));
    return distance <= wheelRadius && distance >= wheelRadius - 20;
}

bool ColorPickerWidget::isInTriangle(const QPointF &pos) const {
    QPolygonF triangle;
    triangle << triangleVertices[0] << triangleVertices[1] << triangleVertices[2];
    return triangle.containsPoint(pos, Qt::OddEvenFill);
}

QPointF ColorPickerWidget::projectOntoWheel(const QPointF &pos) const {
    double angle = qAtan2(pos.y() - wheelCenter.y(), pos.x() - wheelCenter.x());
    return QPointF(wheelCenter.x() + wheelRadius * qCos(angle),
                   wheelCenter.y() + wheelRadius * qSin(angle));
}

void ColorPickerWidget::setHueFromWheel(const QPointF &pos) {
    selectedWheelPoint = projectOntoWheel(pos);
    double angle = -qAtan2(selectedWheelPoint.y() - wheelCenter.y(), selectedWheelPoint.x() - wheelCenter.x());
    int hue = static_cast<int>(qRadiansToDegrees(angle));
    if (hue < 0) hue += 360;
    currentColor.setHsv(hue, currentColor.saturation(), currentColor.value());
    updateTriangleVertices(); // Update triangle gradient for new hue
    emit colorChanged(currentColor);
}

void ColorPickerWidget::setSaturationLightnessFromTriangle(const QPointF &pos) {
    selectedTrianglePoint = pos;

    // Convert triangle position to saturation and lightness
    QPointF v0 = triangleVertices[1] - triangleVertices[0];
    QPointF v1 = triangleVertices[2] - triangleVertices[0];
    QPointF v2 = pos - triangleVertices[0];

    double d00 = QPointF::dotProduct(v0, v0);
    double d01 = QPointF::dotProduct(v0, v1);
    double d11 = QPointF::dotProduct(v1, v1);
    double d20 = QPointF::dotProduct(v2, v0);
    double d21 = QPointF::dotProduct(v2, v1);

    double denom = d00 * d11 - d01 * d01;
    double u = (d11 * d20 - d01 * d21) / denom;
    double v = (d00 * d21 - d01 * d20) / denom;

    int saturation = static_cast<int>(u * 255);
    int lightness = static_cast<int>((1 - v) * 255);
    currentColor.setHsv(currentColor.hue(), qBound(0, saturation, 255), qBound(0, lightness, 255));
    emit colorChanged(currentColor);
}


void ColorPickerWidget::setWheelPointFromColor(const QColor &color) {
    int hue = color.hue();

    // Convert hue to radians
    double angle = qDegreesToRadians(static_cast<double>(-hue));

    // Calculate the position on the wheel
    double radius = wheelRadius;
    selectedWheelPoint = QPointF(
        wheelCenter.x() + radius * qCos(angle),
        wheelCenter.y() + radius * qSin(angle)
        );

}

void ColorPickerWidget::setTrianglePointFromColor(const QColor &color) {
    // Extract saturation and lightness from the QColor
    int saturation = color.saturation();
    int lightness = color.value();

    // Normalize saturation and lightness to range [0, 1]
    double u = static_cast<double>(saturation) / 255.0; // Corresponds to weight for v0 -> v1
    double v = 1.0 - static_cast<double>(lightness) / 255.0; // Corresponds to weight for v0 -> v2

    // Ensure weights are within valid ranges
    u = qBound(0.0, u, 1.0);
    v = qBound(0.0, v, 1.0);
    if (u + v > 1.0) {
        u = 1.0 - v;
    }

    // Calculate the position using barycentric interpolation
    QPointF v0 = triangleVertices[0];
    QPointF v1 = triangleVertices[1];
    QPointF v2 = triangleVertices[2];

    QPointF position = v0 * (1 - u - v) + v1 * u + v2 * v;
    if(position.y() > triangleVertices[1].y()) position.setY(triangleVertices[1].y());

    selectedTrianglePoint = position;

}




void ColorPickerWidget::updateTriangleVertices() {
    double innerRadius = wheelRadius - 20;
    double angleStep = 2 * M_PI / 3;

    for (int i = 0; i < 3; ++i) {
        double angle = i * angleStep - M_PI / 2;
        triangleVertices[i] = QPointF(
            wheelCenter.x() + innerRadius * qCos(angle),
            wheelCenter.y() + innerRadius * qSin(angle)
            );
    }

}

void ColorPickerWidget::drawColorWheel(QPainter &painter) {
    QConicalGradient gradient(wheelCenter, 0);
    for (int i = 0; i <= 360; i += 1) {
        QColor color;
        color.setHsv(i, 255, 255);
        gradient.setColorAt(i / 360.0, color);
    }

    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);

    QPainterPath outerCircle, innerCircle;
    outerCircle.addEllipse(wheelCenter, wheelRadius, wheelRadius);
    innerCircle.addEllipse(wheelCenter, wheelRadius - 20, wheelRadius - 20);

    QPainterPath wheelPath = outerCircle - innerCircle;
    painter.drawPath(wheelPath);
}

void ColorPickerWidget::drawColorTriangle(QPainter &painter) {
    // Define the colors at each vertex
    QColor color1 = Qt::white;
    QColor color2 = QColor::fromHsv(currentColor.hue(), 255, 255);  
    QColor color3 = QColor::fromHsv(0, 0, 0);

    QPolygonF triangle;
    triangle << triangleVertices[0] << triangleVertices[1] << triangleVertices[2];

    QPainterPath trianglePath;
    trianglePath.addPolygon(triangle);

    // Set the painter to fill the triangle
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::NoBrush);

    // Iterate over each pixel inside the triangle and apply the color interpolation
    for (int x = triangle.boundingRect().left(); x < triangle.boundingRect().right(); ++x) {
        for (int y = triangle.boundingRect().top(); y < triangle.boundingRect().bottom(); ++y) {
            QPointF point(x, y);

            // Check if the point is inside the triangle
            if (triangle.containsPoint(point, Qt::OddEvenFill)) {
                // Calculate barycentric coordinates (u, v) of the point relative to the triangle
                double denominator = (triangleVertices[1].y() - triangleVertices[2].y()) * (triangleVertices[0].x() - triangleVertices[2].x()) + (triangleVertices[2].x() - triangleVertices[1].x()) * (triangleVertices[0].y() - triangleVertices[2].y());
                double a = ((triangleVertices[1].y() - triangleVertices[2].y()) * (point.x() - triangleVertices[2].x()) + (triangleVertices[2].x() - triangleVertices[1].x()) * (point.y() - triangleVertices[2].y())) / denominator;
                double b = ((triangleVertices[2].y() - triangleVertices[0].y()) * (point.x() - triangleVertices[2].x()) + (triangleVertices[0].x() - triangleVertices[2].x()) * (point.y() - triangleVertices[2].y())) / denominator;
                double c = 1.0 - a - b;

                // Interpolate the color based on barycentric coordinates
                QColor interpolatedColor;
                interpolatedColor.setRed(a * color1.red() + b * color2.red() + c * color3.red());
                interpolatedColor.setGreen(a * color1.green() + b * color2.green() + c * color3.green());
                interpolatedColor.setBlue(a * color1.blue() + b * color2.blue() + c * color3.blue());

                // Set the pixel color
                painter.setPen(interpolatedColor);
                painter.drawPoint(x, y);
            }
        }
    }
}

void ColorPickerWidget::drawSelectionPoints(QPainter &painter) {
    painter.setBrush(Qt::black);
    painter.setPen(Qt::white);

    // Draw selected hue point on the wheel
    painter.drawEllipse(selectedWheelPoint, 5, 5);

    // Draw selected point in the triangle
    painter.drawEllipse(selectedTrianglePoint, 5, 5);
}
