#ifndef UNDOREDOMANAGER_H
#define UNDOREDOMANAGER_H

#include <qwidget.h>
#include <stack>
#include "brushstroke.h"
#include <QObject>
class UndoRedoManager :public QWidget {
    Q_OBJECT
private:
    struct Action {
        int layerIndex;
        BrushStroke brushStroke;
    };

    std::stack<Action> undoStack;
    std::stack<Action> redoStack;

public:
    void newAction(int index, BrushStroke brushStroke);
    void layersAreAddDel();
    std::pair<int, BrushStroke> undo();
    std::pair<int, BrushStroke> redo();
signals:
    void undoStackIsEmpty();
    void redoStackIsEmpty();

};

#endif // UNDOREDOMANAGER_H
