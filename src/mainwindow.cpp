#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include "colorpickerwidget.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QColor>
#include <QShortcut>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    int* layercount = new int();
    *layercount = 0;
    ui->setupUi(this);
    ui->layersListWidget->setCurrentItem(ui->layersListWidget->item(0));

    //---------------UNDO/REDO---------------
    ui->undoButton->setStyleSheet(QString("background-color: rgb(200, 200, 200);"));
    ui->redoButton->setStyleSheet(QString("background-color: rgb(200, 200, 200);"));

    connect(ui->canvasWidget, &CanvasWidget::undoUnavai, this, [this](){
        ui->undoButton->setStyleSheet(QString("background-color: rgb(200, 200, 200);"));
        ui->redoButton->setStyleSheet(QString(" "));
    });
    connect(ui->canvasWidget, &CanvasWidget::redoUnavai, this, [this](){
        ui->redoButton->setStyleSheet(QString("background-color: rgb(200, 200, 200);"));
        ui->undoButton->setStyleSheet(QString(" "));
    });

    connect(ui->undoButton, &QPushButton::pressed, this, [this](){
        int row = ui->canvasWidget->undo();
        if(row!=-1){
            ui->layersListWidget->setCurrentRow(row);
            ui->redoButton->setStyleSheet(QString(" "));
        }
        else{
            ui->undoButton->setStyleSheet(QString("background-color: rgb(200, 200, 200);"));
        }
    });
    connect(ui->redoButton, &QPushButton::pressed, this, [this](){
        int row = ui->canvasWidget->redo();
        if(row!=-1){
            ui->layersListWidget->setCurrentRow(row);
            ui->undoButton->setStyleSheet(QString(" "));
        }
        else{
            ui->redoButton->setStyleSheet(QString("background-color: rgb(200, 200, 200);"));
        }
    });
    //undo-redo shortcuts
    QAction *undoAction = new QAction("Undo", this);
    QList<QKeySequence> shortcutsUndo;
    shortcutsUndo << QKeySequence("Ctrl+Z") << QKeySequence("Ctrl+Alt+Z");
    undoAction->setShortcuts(shortcutsUndo);
    connect(undoAction, &QAction::triggered, ui->undoButton, &QPushButton::click);
    this->addAction(undoAction);

    QAction *redoAction = new QAction("Redo", this);
    QList<QKeySequence> shortcutsRedo;
    shortcutsRedo << QKeySequence("Ctrl+Y") << QKeySequence("Ctrl+Shift+Z");
    redoAction->setShortcuts(shortcutsRedo);
    connect(redoAction, &QAction::triggered, ui->redoButton, &QPushButton::click);
    this->addAction(redoAction);


    //--------------- COLOR PICKER ---------------
    ColorPickerWidget* colorpicker = new ColorPickerWidget(this);
    colorpicker->setFixedSize(220,220);
    colorpicker->setWindowFlags( Qt::Window | Qt::WindowTitleHint );
    colorpicker->setWindowIcon(QIcon(":/resources/color-wheel.svg"));
    colorpicker->setWindowTitle(QString("ColorPicker"));

    colorpicker->show();
    currentColor.setHsv(0,255/2,255/2,255);

    //color palette
    ui->colorPalette1->setColor(QColor(138, 87, 240));
    ui->colorPalette2->setColor(QColor(112, 161, 215));
    ui->colorPalette3->setColor(QColor(161, 222, 147));
    ui->colorPalette4->setColor(QColor(247, 244, 139));
    ui->colorPalette5->setColor(QColor(244, 124, 124));
    ui->colorPalette6->setColor(QColor(95, 52, 128));
    ui->colorPalette7->setColor(QColor(0, 135, 205));
    ui->colorPalette8->setColor(QColor(78, 163, 42));
    ui->colorPalette9->setColor(QColor(249, 223, 0));
    ui->colorPalette10->setColor(QColor(177, 44, 33));

    connect(ui->colorPalette1, SIGNAL(colorpicked(QColor)), colorpicker, SLOT(receiveColor(QColor)));
    connect(ui->colorPalette2, SIGNAL(colorpicked(QColor)), colorpicker, SLOT(receiveColor(QColor)));
    connect(ui->colorPalette3, SIGNAL(colorpicked(QColor)), colorpicker, SLOT(receiveColor(QColor)));
    connect(ui->colorPalette4, SIGNAL(colorpicked(QColor)), colorpicker, SLOT(receiveColor(QColor)));
    connect(ui->colorPalette5, SIGNAL(colorpicked(QColor)), colorpicker, SLOT(receiveColor(QColor)));
    connect(ui->colorPalette6, SIGNAL(colorpicked(QColor)), colorpicker, SLOT(receiveColor(QColor)));
    connect(ui->colorPalette7, SIGNAL(colorpicked(QColor)), colorpicker, SLOT(receiveColor(QColor)));
    connect(ui->colorPalette8, SIGNAL(colorpicked(QColor)), colorpicker, SLOT(receiveColor(QColor)));
    connect(ui->colorPalette9, SIGNAL(colorpicked(QColor)), colorpicker, SLOT(receiveColor(QColor)));
    connect(ui->colorPalette10, SIGNAL(colorpicked(QColor)), colorpicker, SLOT(receiveColor(QColor)));


    //--------------- BRUSH UPDATE ---------------
    connect(colorpicker, SIGNAL(colorChanged(QColor)), this, SLOT(updateColor(QColor)));

    //change brush size
    connect(ui->brushSizeSlider, &QSlider::valueChanged,
            this, [this](int size){
                ui->canvasWidget->brushUpdate(
                    size, currentTool, currentColor);
                ui->brushSizeLabel->setText(QString("%1").arg(size));}
            );
    //change brush opacity
    connect(ui->brushOpacitySlider, &QSlider::valueChanged,
            this, [this](int opacity){
                currentColor.setAlpha(opacity);
                ui->canvasWidget->brushUpdate(
                    ui->brushSizeSlider->value(), currentTool, currentColor);
                ui->brushOpacityLabel->setText(QString("%1").arg((int)(opacity/255.0*100)));
                ui->colorPreviewTransparent->
                    setStyleSheet(QString("background-color: rgba(%1, %2, %3,%4);").arg(currentColor.red())
                                      .arg(currentColor.green()).arg(currentColor.blue()).arg(
                                          ui->brushOpacitySlider->value()/255.0));
            });

    //change to pencil tool
    connect(ui->pencilToolButton, &QPushButton::pressed, this, [this](){
        if(currentTool!=BrushStroke::Pencil){
            currentTool = BrushStroke::Pencil;
            ui->pencilToolButton->setStyleSheet("background-color: rgb(169, 169, 169);border: none;");
            ui->eraserToolButton->setStyleSheet("background-color: white;border: none;");
            ui->canvasWidget->brushUpdate(
                ui->brushSizeSlider->value(), currentTool, currentColor
                );
        }
    });
    //change to eraser tool
    connect(ui->eraserToolButton, &QPushButton::pressed, this, [this](){
        if(currentTool!=BrushStroke::Eraser){
            currentTool = BrushStroke::Eraser;
            ui->eraserToolButton->setStyleSheet("background-color: rgb(169, 169, 169);border: none;");
            ui->pencilToolButton->setStyleSheet("background-color: white;border: none;");
            ui->canvasWidget->brushUpdate(
                ui->brushSizeSlider->value(), currentTool, currentColor
                );

        }
    });


    // --------------- LAYER ---------------
    //change selected layer
    connect(ui->layersListWidget, SIGNAL(currentRowChanged(int))
            ,ui->canvasWidget, SLOT(changedselectedlayer(int)));

    //add layer button
    connect(ui->addLayerButton, &QPushButton::pressed, this, [this, layercount](){
        ui->canvasWidget->addLayer();
        (*layercount)++;
        int x = ui->layersListWidget->count();
        QListWidgetItem *aitem  = new QListWidgetItem(QString("layer%1").arg(*layercount));
        ui->layersListWidget->insertItem(0,aitem);
        ui->layersListWidget->item(0)->setCheckState(Qt::Checked);

        ui->undoButton->setStyleSheet(QString("background-color: rgb(200, 200, 200);"));
        ui->redoButton->setStyleSheet(QString("background-color: rgb(200, 200, 200);"));

        ui->statusbar->showMessage("Added layer, undo/redo history is cleared.");

    }  );
    //delete layer button
    connect(ui->deleteLayerButton, &QPushButton::pressed, this, [this](){
        if(ui->layersListWidget->count()>1){
            int x =ui->layersListWidget->currentRow();
            int y =x;
            ui->layersListWidget->takeItem(x);
            if(x>=ui->layersListWidget->count()) x--;
            ui->layersListWidget->setCurrentItem(ui->layersListWidget->item(x));

            ui->canvasWidget->deleteLayer(y);

            ui->undoButton->setStyleSheet(QString("background-color: rgb(200, 200, 200);"));
            ui->redoButton->setStyleSheet(QString("background-color: rgb(200, 200, 200);"));

            ui->statusbar->showMessage("Deleted layer, undo/redo history is cleared.");
        }
        else QMessageBox::critical(this,"Unable to delete", "Cannot delete the only layer in canvas!");

    }  );

    //visibility checkstate
    connect(ui->layersListWidget, &QListWidget::itemClicked, this,
                [this](QListWidgetItem *item){
                ui->canvasWidget->
            changeLayerVisibility(ui->layersListWidget->row(item) ,item->checkState());
    });

    //move layer up
    connect(ui->layerUpButton, &QPushButton::pressed, this, [this](){
        ui->canvasWidget->moveLayer(1);
        int x = ui->layersListWidget->currentRow();
        if(x>0){
            // Get the item at the row and remove it
            QListWidgetItem* item = ui->layersListWidget->takeItem(x);

            // Insert the item at the next row
            ui->layersListWidget->insertItem(x - 1, item);
            ui->layersListWidget->setCurrentItem(ui->layersListWidget->item(x-1));
        }

    }  );

    //move layer down
    connect(ui->layerDownButton, &QPushButton::pressed, this, [this](){
        ui->canvasWidget->moveLayer(-1);
        int x = ui->layersListWidget->currentRow();
        // Get the item at the row and remove it
        if(x<(ui->layersListWidget->count())-1){
            QListWidgetItem* item = ui->layersListWidget->takeItem(x);

            // Insert the item at the next row
            ui->layersListWidget->insertItem(x + 1, item);
            ui->layersListWidget->setCurrentItem(ui->layersListWidget->item(x+1));
        }
    }  );

    //---------------SAVE---------------
    connect(ui->saveButton, &QPushButton::pressed, this, [this](){
        QString filePath = QFileDialog::getSaveFileName(this, "Save image", "", "PNG Files (*.png);;JPEG Files (*.jpg)");
        if(ui->canvasWidget->saveCanvas(filePath))
             QMessageBox::information(this, "Success", QString("Image has been saved at %1").arg(filePath));
    });


    //---------------HONKK---------------
    QMediaPlayer* player = new QMediaPlayer(this);
    QAudioOutput* audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl("qrc:/resources/clown_horn.mp3"));
    connect(ui->canvasWidget, &CanvasWidget::canvasPressed, this, [this, player](){
        ui->statusbar->showMessage("HONK!",2000);
        player->stop();
        player->play();

    });
    connect(ui->canvasWidget, &CanvasWidget::canvasReleased, this, [this](){
        ui->statusbar->showMessage("");
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateColor(QColor color)
{
    color.setAlpha(ui->brushOpacitySlider->value());
    ui->canvasWidget->brushUpdate(ui->brushSizeSlider->value(), currentTool, color);
    ui->colorPreviewSolid->
        setStyleSheet(QString("background-color: rgb(%1, %2, %3);").arg(color.red())
                          .arg(color.green()).arg(color.blue()));
    ui->colorPreviewTransparent->
        setStyleSheet(QString("background-color: rgba(%1, %2, %3,%4);").arg(color.red())
                          .arg(color.green()).arg(color.blue()).arg(
                              ui->brushOpacitySlider->value()/255.0));
    currentColor = color;
}
