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

#include "eeprom.h"
#include "nvm_rpi.h"
#include "nvm_eeprom_rpi_internal.h"

void nvm_rpi_read_magic(uint8_t *magic0, uint8_t *magic1, uint8_t *magic2) {
    *magic0 = eeprom_read_byte((void *)RPI_EEPROM_MAGIC_ADDR + 0);
    *magic1 = eeprom_read_byte((void *)RPI_EEPROM_MAGIC_ADDR + 1);
    *magic2 = eeprom_read_byte((void *)RPI_EEPROM_MAGIC_ADDR + 2);
}

void nvm_rpi_update_magic(uint8_t magic0, uint8_t magic1, uint8_t magic2) {
    eeprom_update_byte((void *)RPI_EEPROM_MAGIC_ADDR + 0, magic0);
    eeprom_update_byte((void *)RPI_EEPROM_MAGIC_ADDR + 1, magic1);
    eeprom_update_byte((void *)RPI_EEPROM_MAGIC_ADDR + 2, magic2);
}

#ifdef RGB_MATRIX_ENABLE
int nvm_rpi_get_rgb_mode(uint8_t mode_index, rpi_rgb_mode_t *mode) {
    if (mode_index * sizeof(rpi_rgb_mode_t) >= RPI_EEPROM_MODES_SIZE) return -1;
    void *target = (void *)(RPI_EEPROM_MODES_ADDR + mode_index * sizeof(rpi_rgb_mode_t));
    eeprom_read_block(mode, target, sizeof(rpi_rgb_mode_t));
    return 0;
}

int nvm_rpi_set_rgb_mode(uint8_t mode_index, rpi_rgb_mode_t *mode) {
    if (mode_index * sizeof(rpi_rgb_mode_t) >= RPI_EEPROM_MODES_SIZE) return -1;
    void *target = (void *)(RPI_EEPROM_MODES_ADDR + mode_index * sizeof(rpi_rgb_mode_t));
    eeprom_write_block(mode, target, sizeof(rpi_rgb_mode_t));
    return 0;
}

int nvm_rpi_get_custom_led(uint8_t led_index, uint8_t *hue, uint8_t *sat, uint8_t *val) {
    if (led_index * (sizeof(uint8_t) * 3) >= RPI_EEPROM_CUSTOM_LEDS_SIZE) return -1;
    void *target = (void *)(RPI_EEPROM_CUSTOM_LEDS_ADDR + led_index * (sizeof(uint8_t) * 3));
    *hue = eeprom_read_byte(target);
    *sat = eeprom_read_byte(target+1);
    *val = eeprom_read_byte(target+2);
    return 0;
}

int nvm_rpi_set_custom_led(uint8_t led_index, uint8_t hue, uint8_t sat, uint8_t val) {
    if (led_index * (sizeof(uint8_t) * 3) >= RPI_EEPROM_CUSTOM_LEDS_SIZE) return -1;
    void *target = (void *)(RPI_EEPROM_CUSTOM_LEDS_ADDR + led_index * (sizeof(uint8_t) * 3));
    eeprom_write_byte(target, hue);
    eeprom_write_byte(target+1, sat);
    eeprom_write_byte(target+2, val);
    return 0;
}

int nvm_rpi_get_rgb_mode_index(uint8_t *mode_index) {
    if (mode_index) {
        *mode_index = eeprom_read_byte((void *)RPI_EEPROM_MODE_INDEX_ADDR);
    }
    return 0;
}

int nvm_rpi_set_rgb_mode_index(uint8_t mode_index) {
    eeprom_update_byte((void *)RPI_EEPROM_MODE_INDEX_ADDR, mode_index);
    return 0;
}

int nvm_rpi_get_rgb_hue(uint8_t *hue) {
    if (hue) {
        *hue = eeprom_read_byte((void *)RPI_EEPROM_HUE_ADDR);
    }
    return 0;
}

int nvm_rpi_set_rgb_hue(uint8_t hue) {
    eeprom_update_byte((void *)RPI_EEPROM_HUE_ADDR, hue);
    return 0;
}
#endif
