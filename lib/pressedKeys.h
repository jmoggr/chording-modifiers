//
// exposes
//

typedef struct PressedKey {
    uint16_t keycode;
    uint8_t row;
    uint8_t col;
} PressedKey;

#define MAX_PRESSED_KEYS 26
struct PressedKey pressed_keys[MAX_PRESSED_KEYS];

int num_pressed_keys = 0;

void press_key(uint16_t keycode, uint8_t row, uint8_t col);
void release_key(uint16_t keycode);


//
// implementation
//

void press_key(uint16_t keycode, uint8_t row, uint8_t col) {
    if (num_pressed_keys == MAX_PRESSED_KEYS) {
        return;
    }

    pressed_keys[num_pressed_keys].keycode = keycode;
    pressed_keys[num_pressed_keys].row = row;
    pressed_keys[num_pressed_keys].col = col;

    num_pressed_keys += 1;
}

void release_key(uint16_t keycode) {
    int index = -1;
    for (int i = 0; i < num_pressed_keys; i++) {
        if (pressed_keys[i].keycode == keycode) {
            index = i;
            break;
        }
    }

    if (index < 0) {
        return;
    }

    for(int i = index; i < num_pressed_keys - 1; i++) {
        pressed_keys[i].keycode = pressed_keys[i + 1].keycode;
        pressed_keys[i].row = pressed_keys[i + 1].row;
        pressed_keys[i].col = pressed_keys[i + 1].col;
    }

    num_pressed_keys -= 1;
}