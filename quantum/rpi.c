/* Copyright 2025 Raspberry Pi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "rpi.h"
#include <string.h>
#include "quantum.h"
#include "qmk_settings.h"
#include "nvm_rpi.h"
#include "vialrgb.h"


#ifdef RGB_MATRIX_ENABLE

//Include VialRGB effects to access VIALRGB_EFFECT_* constants
typedef struct {
    uint16_t vialrgb_id;
    uint16_t qmk_id;
} vialrgb_supported_mode_t;
#include "vialrgb_effects.inc"

#ifdef RGB_MATRIX_EFFECT_VIALRGB_DIRECT
extern HSV g_direct_mode_colors[RGB_MATRIX_LED_COUNT];
#endif

uint8_t current_mode_index;

#endif //#ifdef RGB_MATRIX_ENABLE

__attribute__((weak)) void eeconfig_init_rpi_kb(void) {}

__attribute__((weak)) bool compatible_eeprom_version(uint16_t old_version, uint16_t new_version) {
    return false;
}

#ifdef RGB_MATRIX_ENABLE
void init_rpi_rgb_modes(void) {
    dprintf("[RPI] Initializing RGB modes\n");

    rpi_rgb_mode_t led_off_mode = {RGB_MATRIX_DEFAULT_FLAGS, VIALRGB_EFFECT_OFF, RGB_MATRIX_DEFAULT_SPD, true, START_ANIM_B_FADE_VAL, RGB_MATRIX_DEFAULT_HUE, RGB_MATRIX_DEFAULT_SAT};
    rpi_rgb_mode_t empty_mode = {RGB_MATRIX_DEFAULT_FLAGS, SKIP_MODE, 0, false, START_ANIM_B_FADE_VAL, 0, 0};
    rpi_rgb_mode_t direct_led_mode = {LED_FLAG_ALL,VIALRGB_EFFECT_DIRECT,255,true,START_ANIM_B_FADE_VAL,0,0};

    configure_rpi_rgb_mode(0, &led_off_mode);
    dprintf("[RPI] Set led_off_mode at index 0\n");
    for (uint8_t i = 1; i < RPI_RGB_SEQUENCE_MODES_MAX; i++) {
        configure_rpi_rgb_mode(i, &empty_mode);
    }
    dprintf("[RPI] Initialized %d empty RGB modes\n", RPI_RGB_SEQUENCE_MODES_MAX - 1);
    nvm_rpi_set_rgb_mode_index(0);
    current_mode_index = 0;
    nvm_rpi_set_rgb_hue(255);
    dprintf("[RPI] Set RGB mode index to 0 and hue to 255\n");

    configure_rpi_rgb_mode(RPI_RGB_SEQUENCE_MODE_CUSTOM_INDEX, &direct_led_mode);
    dprintf("[RPI] Set direct_mode at custom index\n");
}

void init_rpi_rgb_custom_leds(void) {
    dprintf("[RPI] Initializing %d custom LEDs\n", RGB_MATRIX_LED_COUNT);
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        g_direct_mode_colors[i].h = 0;
        g_direct_mode_colors[i].s = 0;
        g_direct_mode_colors[i].v = 0;
        nvm_rpi_set_custom_led(i, g_direct_mode_colors[i].h, g_direct_mode_colors[i].s, g_direct_mode_colors[i].v);
    }
    dprintf("[RPI] Custom LEDs initialized to black (HSV: 0,0,0)\n");
}

void save_rpi_direct_leds(void) {
    dprintf("[RPI] Saving %d direct LEDs to EEPROM\n", RGB_MATRIX_LED_COUNT);
    uint8_t led_index;
    for (led_index = 0; led_index < RGB_MATRIX_LED_COUNT; led_index++) {
        nvm_rpi_set_custom_led(led_index, g_direct_mode_colors[led_index].h, g_direct_mode_colors[led_index].s, g_direct_mode_colors[led_index].v);
    }
    dprintf("[RPI] Direct LEDs saved successfully\n");
}

void load_rpi_direct_leds(void) {
    dprintf("[RPI] Loading %d direct LEDs from EEPROM\n", RGB_MATRIX_LED_COUNT);
    uint8_t led_index;
    for (led_index = 0; led_index < RGB_MATRIX_LED_COUNT; led_index++) {
        nvm_rpi_get_custom_led(led_index, &g_direct_mode_colors[led_index].h, &g_direct_mode_colors[led_index].s, &g_direct_mode_colors[led_index].v);
    }
    dprintf("[RPI] Direct LEDs loaded successfully\n");
}

void reload_rpi_rgb_hue(void) {
    rpi_rgb_mode_t mode;
    nvm_rpi_get_rgb_mode(current_mode_index, &mode);
    if (mode.fixed_hue) {
        dprintf("[RPI] Using fixed hue: %d, sat: %d\n", mode.h, mode.s);
        rgb_matrix_sethsv(mode.h, mode.s, rgb_matrix_get_val());
    }
    else {
        uint8_t hue;
        nvm_rpi_get_rgb_hue(&hue);
        dprintf("[RPI] Using stored hue: %d, sat: %d\n", hue, mode.s);
        rgb_matrix_sethsv(hue, mode.s, rgb_matrix_get_val());
    }
}

void set_rpi_rgb_brightness(uint8_t brightness) {
    dprintf("[RPI] Setting RGB brightness to: %d\n", brightness);
    rgb_matrix_sethsv(rgb_matrix_get_hue(), rgb_matrix_get_sat(), brightness);
}

void set_rpi_rgb_mode(uint8_t mode_index, bool save_index) {
    dprintf("[RPI] Setting RGB mode to index: %d\n", mode_index);
    rpi_rgb_mode_t mode;
    current_mode_index = mode_index;

    if (save_index) {
        nvm_rpi_set_rgb_mode_index(mode_index);
    }

    nvm_rpi_get_rgb_mode(mode_index, &mode);

    if (mode.effect == VIALRGB_EFFECT_DIRECT) {
        dprintf("[RPI] Mode is direct mode, loading direct LEDs\n");
        load_rpi_direct_leds();
    }
    if (mode.effect == SKIP_MODE) {
        dprintf("[RPI] Mode is empty, setting mode 0\n");
        mode_index = 0;
        if (save_index) {
            nvm_rpi_set_rgb_mode_index(mode_index);
        }
        nvm_rpi_get_rgb_mode(mode_index, &mode);
    }

    dprintf("[RPI] Setting RGB mode to vial code: %lu,qmk code: %lu\n", (unsigned long)mode.effect, (unsigned long)vialrgb_id_to_qmk_id(mode.effect));
    if (mode.effect == VIALRGB_EFFECT_OFF) {
        rgb_matrix_set_flags(RGB_MATRIX_DEFAULT_FLAGS);
        rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
        rgb_matrix_set_speed(RGB_MATRIX_DEFAULT_SPD);
        rgb_matrix_sethsv(RGB_MATRIX_DEFAULT_HUE, RGB_MATRIX_DEFAULT_SAT, rgb_matrix_get_val());
    } else {
        rgb_matrix_mode(vialrgb_id_to_qmk_id(mode.effect));
        rgb_matrix_set_speed(mode.speed);
        rgb_matrix_set_flags(mode.flags);
        reload_rpi_rgb_hue();
    }

    eeconfig_force_flush_rgb_matrix();

    dprintf("[RPI] RGB Mode Updated to: %i\n", mode_index);
}

void reload_rpi_rgb_mode_from_eeprom(void) {
    uint8_t mode_index;
    nvm_rpi_get_rgb_mode_index(&mode_index);
    dprintf("[RPI] Updating RGB mode, current index: %d\n", mode_index);
    set_rpi_rgb_mode(mode_index, true);
}

void change_rpi_rgb_mode(bool increase) {
    dprintf("[RPI] Changing RGB mode, current: %d, direction: %s\n", current_mode_index, increase ? "increase" : "decrease");
    if (increase) {
        if (current_mode_index < RPI_RGB_SEQUENCE_MODES_MAX - 1) {
            current_mode_index++;
        }
        else {
            current_mode_index = 0;
        }
    }
    else {
        if (current_mode_index > 0) {
            current_mode_index--;
        }
        else {
            current_mode_index = RPI_RGB_SEQUENCE_MODES_MAX - 1;
        }
    }
    rpi_rgb_mode_t mode;
    nvm_rpi_get_rgb_mode(current_mode_index, &mode);
    if (mode.effect == SKIP_MODE) {
        dprintf("[RPI] Mode is empty, wrapping to mode 0\n");
        current_mode_index = 0;
    }

    dprintf("[RPI] Final mode index: %d\n", current_mode_index);
    set_rpi_rgb_mode(current_mode_index, true);
}

void configure_rpi_rgb_mode(uint8_t mode_index, rpi_rgb_mode_t *mode) {
    dprintf("[RPI] Setting RGB mode at index %d, effect: %lu\n", mode_index, (unsigned long)mode->effect);
    if (mode_index >= RPI_RGB_MODES_MAX) {
        dprintf("[RPI] Invalid mode index %d, max index is %d\n", mode_index, RPI_RGB_MODES_MAX - 1 );
        return;
    }
    if (mode_index == 0 && mode->effect == SKIP_MODE) {
        dprintf("[RPI] Cannot set mode 0 to empty mode\n");
        return;
    }
    if (mode->effect == VIALRGB_EFFECT_OFF) {
        mode->flags = RGB_MATRIX_DEFAULT_FLAGS;
        mode->speed = RGB_MATRIX_DEFAULT_SPD;
        mode->fixed_hue = true;
        mode->h = RGB_MATRIX_DEFAULT_HUE;
        mode->s = RGB_MATRIX_DEFAULT_SAT;
    }
    nvm_rpi_set_rgb_mode(mode_index, mode);
    dprintf("[RPI] RGB mode saved successfully\n");
}

void change_rpi_rgb_hue(bool increase) {
    rpi_rgb_mode_t mode;
    nvm_rpi_get_rgb_mode(current_mode_index, &mode);
    if (!mode.fixed_hue) {
        if (increase) {
            rgb_matrix_increase_hue();
        }
        else {
            rgb_matrix_decrease_hue();
        }
        nvm_rpi_set_rgb_hue(rgb_matrix_get_hue());
        reload_rpi_rgb_hue();
        dprintf("[RPI] Hue updated to %i\n", rgb_matrix_get_hue());
    }
    else {
        dprintf("[RPI] Mode has fixed hue, skipping hue change\n");
    }
}

bool rpi_rgb_current_mode_is_a_preset(void) {
    return current_mode_index < RPI_RGB_SEQUENCE_MODES_MAX;
}

uint8_t rpi_rgb_current_mode_startup_animation(void) {
    rpi_rgb_mode_t mode;
    nvm_rpi_get_rgb_mode(current_mode_index, &mode);
    return mode.startup_animation;
}

bool rpi_rgb_current_mode_is_fixed_hue(void) {
    rpi_rgb_mode_t mode;
    nvm_rpi_get_rgb_mode(current_mode_index, &mode);
    return mode.fixed_hue;
}
#endif //#ifdef RGB_MATRIX_ENABLE

void rpi_handle_cmd(uint8_t *msg, uint8_t length) {
    dprintf("[RPI] Handling command, length: %d\n", length);
    // All packets must be 32 bytes
    if (length != RPI_HID_COMMAND_LENGTH) {
        dprintf("[RPI] Invalid command length %d, expected %d\n", length, RPI_HID_COMMAND_LENGTH);
        return;
    }
    // msg[0] is the rpi_command_id 0xFC

    uint8_t *command_id   = &(msg[1]);
    uint8_t *command_data = &(msg[2]);

    dprintf("[RPI] Command ID: %d\n", *command_id);
    switch (*command_id) {
        case id_rpi_get_version: {
            dprintf("[RPI] GET_VERSION command\n");
            command_data[0] = DEVICE_VER >> 8;
            command_data[1] = DEVICE_VER & 0xFF;
            dprintf("[RPI] Version sent: %d.%d\n", command_data[0], command_data[1]);
            break;
        }
        case id_rpi_reset_eeprom: {
            dprintf("[RPI] RESET_EEPROM command\n");
            eeconfig_init_rpi();
#ifdef RGB_MATRIX_ENABLE
            reload_rpi_rgb_mode_from_eeprom();
#endif
            break;
        }

#ifdef RGB_MATRIX_ENABLE
        case id_rpi_get_current_mode_index: {
            dprintf("[RPI] GET_CURRENT_MODE_INDEX command\n");
            uint8_t mode_index;
            nvm_rpi_get_rgb_mode_index(&mode_index);
            command_data[0] = current_mode_index;
            command_data[1] = mode_index;
            dprintf("[RPI] Current mode index: %d\n", mode_index);
            break;
        }
        case id_rpi_set_current_mode_index: {
            dprintf("[RPI] SET_CURRENT_MODE_INDEX command\n");
            uint8_t mode_index = command_data[0];
            bool save_index = command_data[1];
            dprintf("[RPI] Requested mode index: %d\n", mode_index);
            if (mode_index >= RPI_RGB_MODES_MAX) {
                dprintf("[RPI] Mode index %d >= max %d, setting to 0\n", mode_index, RPI_RGB_MODES_MAX);
                mode_index = 0;
            }
            set_rpi_rgb_mode(mode_index, save_index);
            break;
        }
        case id_rpi_get_mode: {
            dprintf("[RPI] GET_MODE command\n");
            uint8_t mode_index = command_data[0];
            dprintf("[RPI] Requested mode index: %d\n", mode_index);
            if (mode_index >= RPI_RGB_MODES_MAX) {
                dprintf("[RPI] Mode index %d >= max %d, setting to 0\n", mode_index, RPI_RGB_MODES_MAX);
                mode_index = 0;
            }
            rpi_rgb_mode_t mode;
            nvm_rpi_get_rgb_mode(mode_index, &mode);
            command_data[0] = mode_index;
            command_data[1] = mode.flags;
            command_data[2] = mode.effect & 0xFF;
            command_data[3] = (mode.effect >> 8) & 0xFF;
            command_data[4] = mode.speed;
            command_data[5] = mode.fixed_hue;
            command_data[6] = mode.startup_animation;
            command_data[7] = mode.h;
            command_data[8] = mode.s;
            break;
        }
        case id_rpi_set_mode: {
            dprintf("[RPI] SET_MODE command\n");
            uint8_t mode_index = command_data[0];
            dprintf("[RPI] Setting mode at index: %d\n", mode_index);
            rpi_rgb_mode_t mode;
            nvm_rpi_get_rgb_mode(mode_index, &mode);
            mode.flags = command_data[1];
            mode.effect = command_data[2] | (command_data[3] << 8);
            mode.speed = command_data[4];
            mode.fixed_hue = command_data[5];
            mode.startup_animation = command_data[6];
            mode.h = command_data[7];
            mode.s = command_data[8];
            configure_rpi_rgb_mode(mode_index, &mode);
            if (mode_index == current_mode_index) {
                set_rpi_rgb_mode(mode_index, false);
            }
            break;
        }
        case id_rpi_get_hue: {
            dprintf("[RPI] GET_HUE command\n");
            uint8_t hue;
            nvm_rpi_get_rgb_hue(&hue);
            command_data[0] = hue;
            dprintf("[RPI] Current hue: %d\n", hue);
            break;
        }
        case id_rpi_set_hue: {
            dprintf("[RPI] SET_HUE command\n");
            uint8_t hue = command_data[0];
            dprintf("[RPI] Setting hue to: %d\n", hue);
            nvm_rpi_set_rgb_hue(hue);
            reload_rpi_rgb_hue();
            break;
        }
        case id_rpi_get_brightness: {
            dprintf("[RPI] GET_BRIGHTNESS command\n");
            command_data[0] = rgb_matrix_get_val();
            break;
        }
        case id_rpi_set_brightness: {
            dprintf("[RPI] SET_BRIGHTNESS command\n");
            uint8_t brightness = command_data[0];
            dprintf("[RPI] Setting brightness to: %d\n", brightness);
            set_rpi_rgb_brightness(brightness);
            break;
        }
        case id_rpi_get_current_direct_leds: {
            dprintf("[RPI] GET_CURRENT_DIRECT_LEDS command\n");
            uint16_t first_index = command_data[0] | (command_data[1] << 8);
            uint8_t num_leds = command_data[2];
            dprintf("[RPI] Getting %d LEDs starting from index %d\n", num_leds, first_index);
            length -= 3;
            command_data += 3;

            if (num_leds * 3 > length) break;

            for (size_t i = 0; i < num_leds; ++i) {
                if (i + first_index >= RGB_MATRIX_LED_COUNT)
                    break;
                command_data[i * 3 + 0] = g_direct_mode_colors[i + first_index].h;
                command_data[i * 3 + 1] = g_direct_mode_colors[i + first_index].s;
                command_data[i * 3 + 2] = g_direct_mode_colors[i + first_index].v;
            }
            break;
        }
        case id_rpi_get_saved_direct_leds: {
            dprintf("[RPI] GET_SAVED_DIRECT_LEDS command\n");
            uint16_t first_index = command_data[0] | (command_data[1] << 8);
            uint8_t num_leds = command_data[2];
            dprintf("[RPI] Getting %d saved LEDs starting from index %d\n", num_leds, first_index);
            length -= 3;
            command_data += 3;

            if (num_leds * 3 > length) break;

            for (size_t i = 0; i < num_leds; ++i) {
                if (i + first_index >= RGB_MATRIX_LED_COUNT)
                    break;
                uint8_t h, s, v;
                nvm_rpi_get_custom_led(i + first_index, &h, &s, &v);
                command_data[i * 3 + 0] = h;
                command_data[i * 3 + 1] = s;
                command_data[i * 3 + 2] = v;
            }
            break;
        }
        case id_rpi_save_direct_leds: {
            dprintf("[RPI] SAVE_DIRECT_LEDS command\n");
            save_rpi_direct_leds();
            break;
        }
        case id_rpi_load_direct_leds: {
            dprintf("[RPI] LOAD_DIRECT_LEDS command\n");
            load_rpi_direct_leds();
            break;
        }
#endif //#ifdef RGB_MATRIX_ENABLE

        default:
            dprintf("[RPI] Unknown command ID: %d\n", *command_id);
            break;
    }
    dprintf("[RPI] Command processing complete\n");
    return;
}

bool rpi_eeprom_is_valid(void) {
    dprintf("[RPI] Checking EEPROM validity\n");
    uint8_t correct_magic0 = VENDOR_ID & 0xFF;
    uint8_t correct_magic1 = DEVICE_VER & 0xFF;
    uint8_t correct_magic2 = (DEVICE_VER >> 8) & 0xFF;

    uint8_t magic0, magic1, magic2;
    nvm_rpi_read_magic(&magic0, &magic1, &magic2);
    dprintf("[RPI] Expected magic: [%02X,%02X,%02X], Found: [%02X,%02X,%02X]\n",
            correct_magic0, correct_magic1, correct_magic2, magic0, magic1, magic2);
    bool valid = (magic0 == correct_magic0 && magic1 == correct_magic1 && magic2 == correct_magic2);
    if (!valid && magic0 == correct_magic0) {
        uint16_t eeprom_version = magic1 | (magic2 << 8);
        if (compatible_eeprom_version(eeprom_version, DEVICE_VER)) {
            valid = true;
            nvm_rpi_update_magic(correct_magic0, correct_magic1, correct_magic2);
            dprintf("[RPI] EEPROM is valid, but version is compatible, updating magic\n");
        }
    }
    dprintf("[RPI] EEPROM is %s\n", valid ? "valid" : "invalid");
    return valid;
}

void rpi_eeprom_set_valid(bool valid) {
    dprintf("[RPI] Setting EEPROM validity to: %s\n", valid ? "valid" : "invalid");
    if (valid) {
        uint8_t correct_magic0 = VENDOR_ID & 0xFF;
        uint8_t correct_magic1 = DEVICE_VER & 0xFF;
        uint8_t correct_magic2 = (DEVICE_VER >> 8) & 0xFF;
        dprintf("[RPI] Writing valid magic: [%02X,%02X,%02X]\n", correct_magic0, correct_magic1, correct_magic2);
        nvm_rpi_update_magic(correct_magic0, correct_magic1, correct_magic2);
    }
    else {
        dprintf("[RPI] Writing invalid magic: [FF,FF,FF]\n");
        nvm_rpi_update_magic(0xFF, 0xFF, 0xFF); // invalid magic
    }
}

void eeconfig_init_rpi(void) {
    dprintf("[RPI] Initializing EEPROM configuration\n");
    rpi_eeprom_set_valid(false);
#ifdef RGB_MATRIX_ENABLE
    init_rpi_rgb_modes();
    init_rpi_rgb_custom_leds();
#endif
    eeconfig_init_rpi_kb();
    rpi_eeprom_set_valid(true);
    dprintf("[RPI] EEPROM configuration initialization complete\n");
}

void rpi_init(void) {
    dprintf("[RPI] Starting RPI initialization\n");
    if (!rpi_eeprom_is_valid()) {
        dprintf("[RPI] EEPROM is invalid, initializing\n");
        eeconfig_init_rpi();
    } else {
        dprintf("[RPI] EEPROM is valid, skipping initialization\n");
    }
#ifdef RGB_MATRIX_ENABLE
    nvm_rpi_get_rgb_mode_index(&current_mode_index);
    dprintf("[RPI] Current mode index: %d\n", current_mode_index);
#endif
    dprintf("[RPI] RPI initialization complete\n");
}
