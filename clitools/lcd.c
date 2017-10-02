#include "mraa.h"
#include <stdlib.h>
#include <unistd.h>
#include "string.h"

void initLcd(mraa_i2c_context lcd)
{
    char buf[10] = { 
    0x80, // command
    0x28, // 2-line, 5x7 font

    0x80, // command
    0x0F, // display on, cursor/blink off

    0x80, // command
    0x01, // clear

    0x80, // command
    0x06, // cursor move right

    0x80, // command
    0x1C, // cursor return
    };

    //display initialization sequence
    usleep(50000);
    mraa_i2c_write(lcd, buf, 2);
    usleep(4500);
    mraa_i2c_write(lcd, buf, 2);
    usleep(150);
    mraa_i2c_write(lcd, buf, 2);
    mraa_i2c_write(lcd, buf, 2);

    //display configuration
    mraa_i2c_write(lcd, &buf[2], 2);
    mraa_i2c_write(lcd, &buf[4], 2);
    usleep(2000);
    mraa_i2c_write(lcd, &buf[6], 2);
//    mraa_i2c_write(lcd, &buf[8], 2);
//    usleep(2000);
}

int main(int argc, char * argv[])
{
    if (argc<2)
    {
	printf("rgb lcd module adapter\nusage: lcd [\"line1+2\"] [\"r g b\"]\
	    \n   line1+2: max 16 chars line 1 + max 16 chars line 2\
	    \n   r/g/b: value 0..255 \n");
	exit(0);
    }
    if (argc> 2)
    {
    int red, green, blue;
    sscanf(argv[2], "%i %i %i", &red, &green, &blue);

    mraa_i2c_context rgb = mraa_i2c_init(6);
    mraa_i2c_address(rgb, 0x62);
    
    char buf[100];
    buf[0] = 0;
    buf[1] =0;
    buf[2]= 1;
    buf[3]=0;
    buf[4]=0x08;
    buf[5]=0xAA;
    buf[6]=0x04;
    buf[7]=red;
    buf[8]=0x03;
    buf[9]=green;
    buf[10]=0x02;
    buf[11]=blue;
    
    mraa_i2c_write(rgb, buf, 2);
    mraa_i2c_write(rgb, &buf[2], 2);
    mraa_i2c_write(rgb, &buf[4], 2);
    mraa_i2c_write(rgb, &buf[6], 2);
    mraa_i2c_write(rgb, &buf[8], 2);
    mraa_i2c_write(rgb, &buf[10], 2);

        printf("set rgb: %i %i %i\n", red, green, blue);

    }
    
    //write text to lcd
    mraa_i2c_context lcd = mraa_i2c_init(6);
    mraa_i2c_address(lcd, 0x3e);
    
    int result = mraa_i2c_frequency(lcd, 0);
    printf("result = %i\n", result);

    initLcd(lcd);
    
    unsigned char text[34];
    strncpy(&text[1], argv[1], 32);
    text[0] = 0x40;
    int i= 0;
    int count = strlen(argv[1]);
    if (count>16)
    {
	count = 16;
    }

    uint8_t data[2] = { 0x40, text[i] };

    printf("data [%i, %i] ",data[0],data[1]);
    mraa_i2c_write(lcd, text, count+1);

    if (strlen(argv[1]) > 16)
    {
	count = strlen(argv[1]) - 16;
	if (count > 16)
	{
	    count = 16;
	}
	
	//set cursor to start line 2 at pos 0x40
	data[0] = 0x80;
	data[1] = 0xc0;
	
	mraa_i2c_write(lcd, data, 2);
	
	text[16] = 0x40;
	mraa_i2c_write(lcd, &text[16], count + 1);
	
	printf("textx:%s", &text[17]);
    }

    printf("text: %s \n", text);
}
