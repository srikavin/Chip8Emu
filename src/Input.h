#ifndef INPUT_H
#define INPUT_H

#include <cstdint>

class Input {
public:
    Input() = default;

    bool keys[16];

    void onKeyDown(uint8_t key);

    void onKeyUp(uint8_t key);

    bool triggered() const;

    uint8_t triggeredKey() const;

    void clearTriggered();

private:
    bool isTriggered;
    uint8_t triggerKey;
};

#endif //INPUT_H
