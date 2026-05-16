#include "History.h"

namespace image {

History::History() : currentIndex(-1) {}

History::~History() {
    clear();
}

void History::push(PixelBuffer* state) {
    if (!state) return;

    while (static_cast<int>(states.size()) > currentIndex + 1) {
        delete states.back();
        states.pop_back();
    }

    states.push_back(state->clone());
    currentIndex++;
}

PixelBuffer* History::undo() {
    if (!canUndo()) return nullptr;
    currentIndex--;
    return states[currentIndex];
}

PixelBuffer* History::redo() {
    if (!canRedo()) return nullptr;
    currentIndex++;
    return states[currentIndex];
}

bool History::canUndo() const {
    return currentIndex > 0;
}

bool History::canRedo() const {
    return currentIndex < static_cast<int>(states.size()) - 1;
}

PixelBuffer* History::getCurrent() {
    if (currentIndex < 0 || currentIndex >= static_cast<int>(states.size())) return nullptr;
    return states[currentIndex];
}

const PixelBuffer* History::getCurrent() const {
    if (currentIndex < 0 || currentIndex >= static_cast<int>(states.size())) return nullptr;
    return states[currentIndex];
}

void History::clear() {
    for (PixelBuffer* state : states) {
        delete state;
    }
    states.clear();
    currentIndex = -1;
}

}
