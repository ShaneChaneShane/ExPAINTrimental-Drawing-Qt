#ifndef BRUSHSTROKE_H
#define BRUSHSTROKE_H

#include <QPainter>
#include <QColor>
#include <QPainterPath>
class BrushStroke
{
public:
    enum Tool { Pencil, Eraser};
    QPainterPath path;
    Tool tool;
    QColor color;
    int size;

    BrushStroke();
    bool isEmpty();
};

#endif // BRUSHSTROKE_H
