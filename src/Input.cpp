#include "Input.h"

void Input::onKeyDown(uint8_t key) {
    keys[key] = true;
    isTriggered = true;
    triggerKey = key;
}

void Input::onKeyUp(uint8_t key) {
    keys[key] = false;
}

bool Input::triggered() const {
    return isTriggered;
}

void Input::clearTriggered() {
    isTriggered = false;
}

uint8_t Input::triggeredKey() const {
    return triggerKey;
}
