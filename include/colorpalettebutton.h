#ifndef COLORPALETTEBUTTON_H
#define COLORPALETTEBUTTON_H

#include <QPushButton>
#include <QColor>
class ColorPaletteButton : public QPushButton
{
    Q_OBJECT
public:
    ColorPaletteButton(QWidget *parent);
    void setColor(QColor col);
private:
    QColor color;

signals:
    void colorpicked(QColor col);
private slots:
    void emitColorPicked();
};

#endif // COLORPALETTEBUTTON_H
