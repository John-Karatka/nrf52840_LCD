#include "LCD.h"

uint8_t _Addr;
uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;
uint8_t _numlines;
uint8_t _cols;
uint8_t _rows;
uint8_t _backlightval;

const nrf_drv_twi_t twi = NRF_DRV_TWI_INSTANCE(0);
const nrf_drv_twi_config_t twi_config = {
  .scl = ARGO_TWI_SCL,
  .sda = ARGO_TWI_SDA,
  .frequency = NRF_TWI_FREQ_400K,
  .interrupt_priority = 2,
  .clear_bus_init = true,
  .hold_bus_uninit = true};

void LCD_TWI_init(){
  nrf_drv_twi_init(&twi, &twi_config, NULL, NULL);
  nrf_drv_twi_enable(&twi);
  _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
}

void LCD_begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {

      
	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	nrf_delay_ms(50); 
  
	// Now we pull both RS and R/W low to begin commands
	LCD_expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	nrf_delay_ms(1000);

  	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46
	
	  // we start in 8bit mode, try to set 4 bit mode
   LCD_write4bits(0x03 << 4);
   nrf_delay_us(4500); // wait min 4.1ms
   
   // second try
   LCD_write4bits(0x03 << 4);
   nrf_delay_us(4500); // wait min 4.1ms
   
   // third go!
   LCD_write4bits(0x03 << 4); 
   nrf_delay_us(150);
   
   // finally, set to 4-bit interface
   LCD_write4bits(0x02 << 4); 


	// set # lines, font size, etc.
	LCD_command(LCD_FUNCTIONSET | _displayfunction);  
	
	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	LCD_display();
	
	// clear it off
	LCD_clear();
	
	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	
	// set the entry mode
	LCD_command(LCD_ENTRYMODESET | _displaymode);
	
	LCD_home();
  
}

/********** high level commands, for the user! */
void LCD_clear(){
	LCD_command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	nrf_delay_us(2000);  // this command takes a long time!
}

void LCD_home(){
	LCD_command(LCD_RETURNHOME);  // set cursor position to zero
	nrf_delay_us(2000);  // this command takes a long time!
}

void LCD_setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > (_numlines-1) ) {
		row = _numlines-1;    // we count rows starting w/0
	}
	LCD_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LCD_noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCD_display() {
	_displaycontrol |= LCD_DISPLAYON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LCD_noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCD_cursor() {
	_displaycontrol |= LCD_CURSORON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LCD_noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCD_blink() {
	_displaycontrol |= LCD_BLINKON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LCD_scrollDisplayLeft(void) {
	LCD_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LCD_scrollDisplayRight(void) {
	LCD_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LCD_leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	LCD_command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LCD_rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	LCD_command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LCD_autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	LCD_command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LCD_noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	LCD_command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LCD_createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	LCD_command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		//LCD_write(charmap[i]);
	}
}

// Turn the (optional) backlight off/on
void LCD_noBacklight(void) {
	_backlightval=LCD_NOBACKLIGHT;
	LCD_expanderWrite(0);
}

void LCD_backlight(void) {
	_backlightval=LCD_BACKLIGHT;
	LCD_expanderWrite(0);
}



/*********** mid level commands, for sending data/cmds */

inline void LCD_command(uint8_t value) {
	LCD_send(value, 0);
}

inline void LCD_write(uint8_t value) {
	LCD_send(value, Rs);
}

void LCD_print(uint8_t *string, int size) {
    for (int i = 0; i < size - 1; i++) {
      LCD_write(string[i]);
    }
}

/************ low level data pushing commands **********/

// write either command or data
void LCD_send(uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
       LCD_write4bits((highnib)|mode);
       LCD_write4bits((lownib)|mode); 
}

void LCD_write4bits(uint8_t value) {
	LCD_expanderWrite(value);
	LCD_pulseEnable(value);
}

void LCD_expanderWrite(uint8_t _data){                                        
        _data = _data | _backlightval;
        uint8_t charArray[1];
        charArray[0] = _data;
        nrf_drv_twi_tx(&twi, 0x27, charArray, 1, false);
        nrf_delay_ms(2);
}

void LCD_pulseEnable(uint8_t _data){
	LCD_expanderWrite(_data | En);	// En high
	nrf_delay_us(1);		// enable pulse must be >450ns
	
	LCD_expanderWrite(_data & ~En);	// En low
	nrf_delay_us(50);		// commands need > 37us to settle
} 


// Alias functions

void LCD_cursor_on(){
	LCD_cursor();
}

void LCD_cursor_off(){
	LCD_noCursor();
}

void LCD_blink_on(){
	LCD_blink();
}

void LCD_blink_off(){
	LCD_noBlink();
}

void LCD_load_custom_character(uint8_t char_num, uint8_t *rows){
		LCD_createChar(char_num, rows);
}

void LCD_setBacklight(uint8_t new_val){
	if(new_val){
		LCD_backlight();		// turn backlight on
	}else{
		LCD_noBacklight();		// turn backlight off
	}
}