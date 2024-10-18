#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>
#include <stdio.h>
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "boards.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04  // Enable bit
#define Rw 0x02  // Read/Write bit
#define Rs 0x01  // Register select bit

void LCD_TWI_init();
void LCD_init_priv();
void LCD_send(uint8_t, uint8_t);
void LCD_write4bits(uint8_t);
void LCD_expanderWrite(uint8_t);
void LCD_pulseEnable(uint8_t);
void LCD_command(uint8_t);
void LCD_write(uint8_t);
void LCD_begin(uint8_t cols, uint8_t rows, uint8_t charsize);
void LCD_clear();
void LCD_home();
void LCD_noDisplay();
void LCD_display();
void LCD_noBlink();
void LCD_blink();
void LCD_noCursor();
void LCD_cursor();
void LCD_scrollDisplayLeft();
void LCD_scrollDisplayRight();
void LCD_printLeft();
void LCD_printRight();
void LCD_leftToRight();
void LCD_rightToLeft();
void LCD_shiftIncrement();
void LCD_shiftDecrement();
void LCD_noBacklight();
void LCD_backlight();
void LCD_autoscroll();
void LCD_noAutoscroll(); 
void LCD_createChar(uint8_t, uint8_t[]);
void LCD_setCursor(uint8_t, uint8_t); 
void LCD_print(uint8_t*, int);
  
#endif