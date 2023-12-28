int num_pressed_keys = 0;

void tap_key(uint16_t keycode);
bool is_special_key(uint16_t keycode);

void tap_key(uint16_t keycode) {
    register_code16(keycode);
    unregister_code16(keycode);
}