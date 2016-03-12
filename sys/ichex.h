#ifndef ICHEX_H
#define	ICHEX_H

#define GET_INSTRUCTION_MODE() eeprom_single_read(EEPROM_INSTRUCTION_MODE);

bool is_this_the_first_pwrup (void);

#endif	/* ICHEX_H */
