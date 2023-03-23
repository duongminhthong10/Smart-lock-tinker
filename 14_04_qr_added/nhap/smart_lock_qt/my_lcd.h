#ifndef my_lcd_h
#define my_lcd_h

#include<stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * API
 ******************************************************************************/
// initialize hardware
void my_lcd_init();

//set cursor (x: 0-15, y: 0 - 1)
void my_lcd_set_cursor(int x, int y);

//write char at current cursor
void my_lcd_write_char(char c);

//update display
void my_lcd_update_dipslay();

//clear display
void my_lcd_clear_line(char line);
//write number 1-2 chữ số
void my_lcd_write_number(int8_t number);

#ifdef __cplusplus
}
#endif

#endif // my_lcd_h
