#ifndef COLORPICKERWIDGET_H
#define COLORPICKERWIDGET_H

#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QMouseEvent>

class ColorPickerWidget : public QWidget {
    Q_OBJECT

public:
    explicit ColorPickerWidget(QWidget *parent = nullptr);
    QColor getSelectedColor() const;

signals:
    void colorChanged(const QColor &color);
private slots:
    void receiveColor(const QColor &color);


protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    enum InteractionMode { None, Wheel, Triangle };
    InteractionMode currentInteractionMode;
    QPixmap trianglePixmap;
    QColor currentColor;
    QPointF wheelCenter;
    int wheelRadius;
    QPointF triangleVertices[3];
    QPointF selectedWheelPoint;
    QPointF selectedTrianglePoint;

    bool isOnWheel(const QPointF &pos) const;
    bool isInTriangle(const QPointF &pos) const;
    QPointF projectOntoWheel(const QPointF &pos) const;
    void updateTriangleVertices();
    void setHueFromWheel(const QPointF &pos);
    void setSaturationLightnessFromTriangle(const QPointF &pos);
    void setWheelPointFromColor(const QColor &color);
    void setTrianglePointFromColor(const QColor &color);

    void drawColorWheel(QPainter &painter);
    void drawColorTriangle(QPainter &painter);
    void drawSelectionPoints(QPainter &painter);
};

#endif // COLORPICKERWIDGET_H
