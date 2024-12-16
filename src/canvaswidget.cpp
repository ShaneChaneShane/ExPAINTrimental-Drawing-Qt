#include "canvaswidget.h"
#include "brushstroke.h"
#include <QDebug>
CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent)
    ,canvasImageBuffer(size(), QImage::Format_ARGB32_Premultiplied) // Initialize image buffer
{
    currentStroke.size=20;
    currentStroke.color.setHsv(0,255/2,255/2,255);
    currentStroke.tool = BrushStroke::Pencil;
    selectedLayer = 0;
    canvasImageBuffer.fill(Qt::white);
    connect(undoRedoManager, &UndoRedoManager::undoStackIsEmpty, this, [this](){ emit undoUnavai();} );
    connect(undoRedoManager, &UndoRedoManager::redoStackIsEmpty, this, [this](){ emit redoUnavai();} );
}

void CanvasWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    canvasImageBuffer =  QImage(width(),height(), QImage::Format_ARGB32_Premultiplied);
    canvasImageBuffer.fill(Qt::white);
    QPainter canvasPainterBuffer(&canvasImageBuffer);
    painter.drawImage(0,0,canvasImageBuffer);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    int i =0;
    for (auto& lay : layers) {
        if(!lay.second) continue;
        QImage imgBuffer(width(),height(), QImage::Format_ARGB32_Premultiplied);
        imgBuffer.fill(Qt::transparent);

        QPainter painterBuffer(&imgBuffer); //pass by reference ***
        painterBuffer.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painterBuffer.setRenderHint(QPainter::Antialiasing);
        for (auto& strok : lay.first) {
            if(strok.tool==BrushStroke::Pencil){

                painterBuffer.setPen(QPen(strok.color, strok.size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

                painterBuffer.drawPath(strok.path);
            }
            else if(strok.tool == BrushStroke::Eraser){
                painterBuffer.setCompositionMode(QPainter::CompositionMode_Clear);
                painterBuffer.setPen(QPen(strok.color, strok.size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                painterBuffer.drawPath(strok.path);
                painterBuffer.setCompositionMode(QPainter::CompositionMode_SourceOver);
            }
        }
        if(selectedLayer==i){

            // Overlay the current stroke being drawn
            if(currentStroke.tool==BrushStroke::Pencil && !currentPath.isEmpty()){

                painterBuffer.setPen(QPen(currentStroke.color, currentStroke.size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                painterBuffer.drawPath(currentPath);
            }
            else if(currentStroke.tool == BrushStroke::Eraser && !currentPath.isEmpty()){
                painterBuffer.setCompositionMode(QPainter::CompositionMode_Clear);
                painterBuffer.setPen(QPen(currentStroke.color, currentStroke.size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                painterBuffer.drawPath(currentPath);
                painterBuffer.setCompositionMode(QPainter::CompositionMode_SourceOver);

            }
        }
        canvasPainterBuffer.drawImage(0,0,imgBuffer);
        i++;
    }
    painter.drawImage(0,0,canvasImageBuffer);


}

void CanvasWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // Start a new stroke
        currentPath.moveTo(event->pos());
        emit canvasPressed();
    }
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        // Add to the current path as the mouse moves
        currentPath.lineTo(event->pos());
        update(); // Repaint the canvas with the new stroke overlay
    }
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {

        // Save the stroke to the current layer
        currentStroke.path = currentPath;
        layers[selectedLayer].first.push_back(currentStroke);
        undoRedoManager->newAction(selectedLayer, currentStroke);

        // Reset the path for the next stroke
        currentPath = QPainterPath();
        currentStroke.path = currentPath;
        emit redoUnavai();
        emit canvasReleased();
        update();
    }
}

void CanvasWidget::changedselectedlayer(int currentRow)
{

    selectedLayer = layers.size()-1-currentRow;

}
void CanvasWidget::changeLayerVisibility(int row, bool checkstate){

    layers[layers.size()-1-row].second = checkstate;
    update();
}

void CanvasWidget::addLayer()
{
    // Add a new empty layer
    layers.push_back({{},1});

    undoRedoManager->layersAreAddDel();
}



void CanvasWidget::deleteLayer(int sel)
{
    sel = layers.size()-1-sel;
    if (layers.size()>1 && sel < layers.size()) {
        layers.erase(layers.begin() + sel);
        selectedLayer = sel-1;
        if (selectedLayer <0) {
            selectedLayer=0;
        }


        update();
        undoRedoManager->layersAreAddDel();
    }
}

void CanvasWidget::moveLayer(int dir)
{
    if(dir == 1){
        if(selectedLayer >= layers.size()-1) return;
        std::swap(layers[selectedLayer], layers[selectedLayer + 1]);

    }
    else{
        if(selectedLayer <= 0) return;
        std::swap(layers[selectedLayer], layers[selectedLayer - 1]);
    }
    update();
}

void CanvasWidget::brushUpdate(int size, BrushStroke::Tool tool, QColor color)
{
    currentStroke.size = size;
    currentStroke.color = color;
    currentStroke.tool = tool;
}

bool CanvasWidget::saveCanvas(const QString &filePath)
{

    return canvasImageBuffer.save(filePath);
}

int CanvasWidget::undo()
{
    auto [idx, brushStroke] = undoRedoManager->undo();
    if( idx==-1) return -1;
    layers[idx].first.pop_back();
    update();
    return layers.size()-1-idx;
}
int CanvasWidget::redo()
{

    auto [idx, brushStroke] = undoRedoManager->redo();
    if( idx==-1) return -1;

    layers[idx].first.push_back(brushStroke);
    update();
    return layers.size()-1-idx;
}
