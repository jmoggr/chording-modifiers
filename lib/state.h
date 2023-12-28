typedef struct SpecialKey {
    uint16_t keycode;
    bool is_pressed;
    bool is_active;
    bool is_cooling;
    bool is_latched;
    bool is_registered;
    bool is_modifier;
} SpecialKey;

struct PressedKey {
    // Should only be used to check if it's a modifier key
    uint16_t keycode;
    uint8_t row;
    uint8_t col;
};

#define MAX_PRESSED_KEYS 26
struct PressedKey pressed_keys[MAX_PRESSED_KEYS];