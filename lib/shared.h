enum layers {
    BASE,  // default layer
    SYMBOLS,
    LAYER1,
    LAYER2,
    LAYER3,
    LAYER4,
};

int num_pressed_keys = 0;

enum custom_keycodes {
    KC_LYR1 = SAFE_RANGE,
    KC_LYR2,
    KC_LYR3,
    KC_LYR4,
    KC_RNGC,
    KC_RNGN,
    KC_TEST,
    KC_TEST2,
    KC_GMNG,
    KC_TEST3,
    KC_TEST4,
};

struct PressedKey {
    // Should only be used to check if it's a modifier key
    uint16_t keycode;
    uint8_t row;
    uint8_t col;
    uint16_t keyup_timer;
};

uint8_t last_row = 0;
uint8_t last_col = 0;
int last_layer = 0;

#define MAX_PRESSED_KEYS 26
struct PressedKey pressed_keys[MAX_PRESSED_KEYS];

void tap_key(uint16_t keycode);
bool is_normal_pressed(void);
bool is_special_key(uint16_t keycode);

void tap_key(uint16_t keycode) {
    register_code16(keycode);
    unregister_code16(keycode);
}

bool is_normal_pressed() {
    for (int i = num_pressed_keys - 1; i >= 0; i--) {
        if (!is_special_key(pressed_keys[i].keycode)) {
            return true;
        }
    }

    return false;
}

bool is_special_key(uint16_t keycode) {
    switch (keycode) {
        case KC_LYR1:
        case KC_SPC:
        case KC_LCTL:
        case KC_LALT:
        case KC_LSFT:
        case KC_RGUI:
            return true;
        default:
            return false;
    }
}