#include "UndoRedoManager.h"

void UndoRedoManager::newAction(int index, BrushStroke brushStroke) {
    Action action{index, brushStroke};
    undoStack.push(action);
    redoStack = std::stack<Action>(); // Clear redo stack
}

void UndoRedoManager::layersAreAddDel() {
    undoStack = std::stack<Action>();
    redoStack = std::stack<Action>();
}

std::pair<int, BrushStroke> UndoRedoManager::undo() {
    if (undoStack.empty()) return {-1, BrushStroke()};

    Action action = undoStack.top();
    undoStack.pop();
    redoStack.push(action); // Push the undone action to redo stack
    return {action.layerIndex, action.brushStroke};
    if(undoStack.empty()) emit UndoRedoManager::undoStackIsEmpty();
}

std::pair<int, BrushStroke> UndoRedoManager::redo() {
    if (redoStack.empty())  return {-1, BrushStroke()};

    Action action = redoStack.top();
    redoStack.pop();
    if(redoStack.empty()) emit UndoRedoManager::redoStackIsEmpty();

    undoStack.push(action); // Push the redone action to undo stack
    return {action.layerIndex, action.brushStroke};
}
