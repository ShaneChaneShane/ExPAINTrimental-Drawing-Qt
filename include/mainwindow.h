#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "brushstroke.h"
#include <QColor>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    bool undoAvai;
    bool redoAvai;
    Ui::MainWindow *ui;
    QColor currentColor;
    BrushStroke::Tool currentTool = BrushStroke::Pencil;

private slots: void updateColor(QColor color);

};
#endif // MAINWINDOW_H
