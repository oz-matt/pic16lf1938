
#include <stdbool.h>

#include <ichex.h>
#include <eeprom.h>

bool is_this_the_first_pwrup (void) {
    if (eeprom_single_read(EEPROM_FIRST_PWRUP_ADDR) != 1) {
        return true;
    }
    return false;
}
