#include "brushstroke.h"

BrushStroke::BrushStroke() {}

bool BrushStroke::isEmpty(){
    if(path.isEmpty()) return 1;
    return 0;
}
