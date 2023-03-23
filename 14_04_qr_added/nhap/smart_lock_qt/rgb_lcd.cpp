/*
    rgb_lcd.c
*/

#include <stdio.h>
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port

#include <wiringPi.h>

#include "rgb_lcd.h"

    void send(uint8_t, uint8_t);
    

    uint8_t _displayfunction;
    uint8_t _displaycontrol;
    uint8_t _displaymode;

    uint8_t _initialized;

    uint8_t _numlines, _currline;

int file_i2c;

void i2c_send_byte(unsigned char dta) {
    //Wire.beginTransmission(LCD_ADDRESS);        // transmit to device #4
    //fd = wiringPiI2CSetup(LCD_ADDRESS);
    //Wire.write(dta);                            // sends five bytes
    // wiringPiI2CWrite(fd, dta);
    //wiringPiI2CWriteReg8(fd, LCD_ADDRESS, dta);
	//----- OPEN THE I2C BUS -----
	char *filename = (char*)"/dev/i2c-1";
	if ((file_i2c = open(filename, O_RDWR)) < 0)
	{
		//ERROR HANDLING: you can check errno to see what went wrong
		printf("Failed to open the i2c bus");
		return;
	}
	
	int addr = LCD_ADDRESS;          //<<<<<The I2C address of the slave
	if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
	{
		printf("Failed to acquire bus access and/or talk to slave.\n");
		//ERROR HANDLING; you can check errno to see what went wrong
		return;
	}
	
	//----- WRITE BYTES -----
	if (write(file_i2c, &dta, 1) != 1)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to write to the i2c bus.\n");
	}

close(file_i2c);
    //Wire.endTransmission();                     // stop transmitting
}

void i2c_send_byteS(unsigned char* dta, unsigned char len) {
    //Wire.beginTransmission(LCD_ADDRESS);        // transmit to device #4
    //fd = wiringPiI2CSetup(LCD_ADDRESS);
    // for (int i = 0; i < len; i++) {
    //     //Wire.write(dta[i]);
    //     // wiringPiI2CWrite (fd, dta[i]);
    //     wiringPiI2CWriteReg8(fd, LCD_ADDRESS, dta[i]);
    // }
    //Wire.endTransmission();                     // stop transmitting
    char *filename = (char*)"/dev/i2c-1";
	if ((file_i2c = open(filename, O_RDWR)) < 0)
	{
		//ERROR HANDLING: you can check errno to see what went wrong
		printf("Failed to open the i2c bus");
		return;
	}
	
	int addr = LCD_ADDRESS;          //<<<<<The I2C address of the slave
	if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
	{
		printf("Failed to acquire bus access and/or talk to slave.\n");
		//ERROR HANDLING; you can check errno to see what went wrong
		return;
	}
	
	//----- WRITE BYTES -----
	if (write(file_i2c, dta, len) != len)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to write to the i2c bus.\n");
	}

    close(file_i2c);
}


void lcd_begin(uint8_t cols, uint8_t rows, uint8_t charsize){
wiringPiSetup () ;
    //Wire.begin();
    //fd = wiringPiI2CSetup(LCD_ADDRESS);
    //     char *filename = (char*)"/dev/i2c-1";
	// if ((file_i2c = open(filename, O_RDWR)) < 0)
	// {
	// 	//ERROR HANDLING: you can check errno to see what went wrong
		// printf("Failed to open the i2c bus");
	// 	return;
	// }

    if (rows > 1) {
        _displayfunction |= LCD_2LINE;
    }
    _numlines = rows;
    _currline = 0;

    // for some 1 line displays you can select a 10 pixel high font
    if ((charsize != 0) && (rows == 1)) {
        _displayfunction |= LCD_5x10DOTS;
    }

    // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    // according to datasheet, we need at least 40ms after power rises above 2.7V
    // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
    delayMicroseconds(50000);

    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    command(LCD_FUNCTIONSET | _displayfunction);
    delayMicroseconds(4500);  // wait more than 4.1ms

    // second try
    command(LCD_FUNCTIONSET | _displayfunction);
    delayMicroseconds(150);
    // third go
    command(LCD_FUNCTIONSET | _displayfunction);

    // finally, set # lines, font size, etc.
    command(LCD_FUNCTIONSET | _displayfunction);

    // turn the display on with no cursor or blinking default
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();
    // clear it off
    clear();
    // Initialize to default text direction (for romance languages)
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    // set the entry mode
    command(LCD_ENTRYMODESET | _displaymode);

// printf("%d\n",count_1);
//     // backlight init
//     setReg(REG_MODE1, 0);
//     // set LEDs controllable by both PWM and GRPPWM registers
//     setReg(REG_OUTPUT, 0xFF);
//     // set MODE2 values
//     // 0010 0000 -> 0x20  (DMBLNK to 1, ie blinky mode)
//     setReg(REG_MODE2, 0x20);
// printf("%d\n",count_1);
//     setColorWhite();

}

/********** high level commands, for the user! */
void clear() {
    command(LCD_CLEARDISPLAY);        // clear display, set cursor position to zero
    delayMicroseconds(2000);          // this command takes a long time!
}

void home() {
    command(LCD_RETURNHOME);        // set cursor position to zero
    delayMicroseconds(2000);        // this command takes a long time!
}

void setCursor(uint8_t col, uint8_t row) {

    col = (row == 0 ? col | 0x80 : col | 0xc0);
    unsigned char dta[2] = {0x80, col};

    i2c_send_byteS(dta, 2);

}

// Turn the display on/off (quickly)
void noDisplay() {
    _displaycontrol &= ~LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void display() {
    _displaycontrol |= LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void noCursor() {
    _displaycontrol &= ~LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void cursor() {
    _displaycontrol |= LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void noBlink() {
    _displaycontrol &= ~LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void blink() {
    _displaycontrol |= LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void scrollDisplayLeft(void) {
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void scrollDisplayRight(void) {
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void leftToRight(void) {
    _displaymode |= LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void rightToLeft(void) {
    _displaymode &= ~LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void autoscroll(void) {
    _displaymode |= LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void noAutoscroll(void) {
    _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void createChar(uint8_t location, uint8_t charmap[]) {

    location &= 0x7; // we only have 8 locations 0-7
    command(LCD_SETCGRAMADDR | (location << 3));


    unsigned char dta[9];
    dta[0] = 0x40;
    for (int i = 0; i < 8; i++) {
        dta[i + 1] = charmap[i];
    }
    i2c_send_byteS(dta, 9);
}

// Control the backlight LED blinking
void blinkLED(void) {
    // blink period in seconds = (<reg 7> + 1) / 24
    // on/off ratio = <reg 6> / 256
    setReg(0x07, 0x17);  // blink every second
    setReg(0x06, 0x7f);  // half on, half off
}

void noBlinkLED(void) {
    setReg(0x07, 0x00);
    setReg(0x06, 0xff);
}

/*********** mid level commands, for sending data/cmds */

// send command
inline void command(uint8_t value) {
    unsigned char dta[2] = {0x80, value};
    i2c_send_byteS(dta, 2);
}

// send data
int lcd_write(uint8_t value) {

    unsigned char dta[2] = {0x40, value};
    i2c_send_byteS(dta, 2);
    return 1; // assume sucess
}

void lcd_write_string(char *s){
    while(*s){
        lcd_write(*(s++));
    }
}

void setReg(unsigned char addr, unsigned char dta) {
    // //Wire.beginTransmission(RGB_ADDRESS); // transmit to device #4
    // fd = wiringPiI2CSetup(RGB_ADDRESS);
    // //Wire.write(addr);
    // wiringPiI2CWrite (fd, addr);
    // //Wire.write(dta);
    // wiringPiI2CWrite (fd, dta);
    // //Wire.endTransmission();    // stop transmitting
    //----- OPEN THE I2C BUS -----
	char *filename = (char*)"/dev/i2c-1";
	if ((file_i2c = open(filename, O_RDWR)) < 0)
	{
		//ERROR HANDLING: you can check errno to see what went wrong
		printf("Failed to open the i2c bus");
		return;
	}
	
	//int addr = RGB_ADDRESS;          //<<<<<The I2C address of the slave
	if (ioctl(file_i2c, I2C_SLAVE, RGB_ADDRESS) < 0)
	{
		printf("Failed to acquire bus access and/or talk to slave.\n");
		//ERROR HANDLING; you can check errno to see what went wrong
		return;
	}
	
	//----- WRITE BYTES -----
	if (write(file_i2c, &addr, 1) != 1)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to write to the i2c bus.\n");
	}
    //----- WRITE BYTES -----
	if (write(file_i2c, &dta, 1) != 1)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to write to the i2c bus.\n");
	}

close(file_i2c);
}

void lcd_setRGB(unsigned char r, unsigned char g, unsigned char b) {
    setReg(REG_RED, r);
    setReg(REG_GREEN, g);
    setReg(REG_BLUE, b);
}

const unsigned char color_define[4][3] = {
    {255, 255, 255},            // white
    {255, 0, 0},                // red
    {0, 255, 0},                // green
    {0, 0, 255},                // blue
};

void setColor(unsigned char color) {
    if (color > 3) {
        return ;
    }
    lcd_setRGB(color_define[color][0], color_define[color][1], color_define[color][2]);
}

void setColorWhite() {
  lcd_setRGB(255, 255, 255);
}

void setColorAll() {
  lcd_setRGB(0, 0, 0);
}
