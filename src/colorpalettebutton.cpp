#include "colorpalettebutton.h"
#include <QDebug>
ColorPaletteButton::ColorPaletteButton(QWidget *parent)
        : QPushButton(parent) {
    connect(this, &QPushButton::pressed, this, &ColorPaletteButton::emitColorPicked);

}
void ColorPaletteButton::emitColorPicked() {
    emit colorpicked(color);
}



void ColorPaletteButton::setColor(QColor col){
    color = col;
}
