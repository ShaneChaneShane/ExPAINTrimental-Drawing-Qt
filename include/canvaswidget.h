#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <vector>
#include <QImage>
#include <QPixmap>
#include "brushstroke.h"
#include "UndoRedoManager.h"

class CanvasWidget : public QWidget
{
    Q_OBJECT
private:

    std::vector<std::pair<std::vector<BrushStroke>, bool>> layers = {{{},1}};
    int selectedLayer;
    QPainterPath currentPath;
    BrushStroke currentStroke;

    QImage canvasImageBuffer;
    std::vector<QImage> layersImageBuffer;
    UndoRedoManager* undoRedoManager = new UndoRedoManager();

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public:
    explicit CanvasWidget(QWidget *parent = nullptr);
    void brushUpdate(int size, BrushStroke::Tool tool, QColor color);

signals:
    void undoUnavai();
    void redoUnavai();
    void canvasPressed();
    void canvasReleased();
public slots:
    void changedselectedlayer(int currentRow);
    void changeLayerVisibility(int row, bool checkstate);
    void addLayer();
    void deleteLayer(int sel);
    void moveLayer(int dir);

    bool saveCanvas(const QString &filePath);

    int undo();
    int redo();

};

#endif // CANVASWIDGET_H
