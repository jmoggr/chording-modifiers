typedef struct SpecialKey {
    uint16_t keycode;
    bool is_pressed;
    bool is_active;
    bool is_cooling;
    bool is_latched;
    bool is_registered;
    bool is_modifier;
} SpecialKey;

#define SPECIAL_KEY_DEFAULT .is_pressed = false, \
                            .is_active = false, \
                            .is_cooling = false, \
                            .is_latched = false, \
                            .is_registered = false,


// defined in keymap.c
extern const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS];

#include "config.h"
#include "pressedKeys.h"
#include "special.h"
#include "lookupKeycode.h"
#include "eventHandlers.h"
