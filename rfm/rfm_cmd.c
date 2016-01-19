#include <stdint.h>
#include <mraa.h>

#define SPI_CLK 13
#define SPI_MOSI 11
#define SPI_MISO 12
#define SPI_CS 10

#define EXTERN_CALL

mraa_gpio_context clk, mosi, miso, cs;

void rfm_init()
{
    //config gpio
    clk = mraa_gpio_init(SPI_CLK);
    mraa_gpio_use_mmaped(clk, 1);
    mraa_gpio_dir(clk, MRAA_GPIO_OUT);
    mraa_gpio_write(clk, 0);
    mosi = mraa_gpio_init(SPI_MOSI);
    mraa_gpio_use_mmaped(mosi, 1);
    mraa_gpio_dir(mosi, MRAA_GPIO_OUT);
    mraa_gpio_write(mosi, 0);
    miso = mraa_gpio_init(SPI_MISO);
    mraa_gpio_use_mmaped(miso, 1);
    mraa_gpio_dir(miso, MRAA_GPIO_IN);
    mraa_gpio_write(miso, 0);
    cs = mraa_gpio_init(SPI_CS);
    mraa_gpio_use_mmaped(cs, 1);
    mraa_gpio_dir(cs, MRAA_GPIO_OUT);
    mraa_gpio_write(cs, 1);
}

uint16_t rfm_cmd(uint16_t data)
{
    //select the device
    mraa_gpio_write(cs, 0);
    
    //reset incoming data
    uint16_t response = 0;

    int i;    
    for (i = 15; i>=0; i--)
    {
        response <<=1;
    
        //clock data out and in
        mraa_gpio_write(mosi, (data >> i) & 0x1);
        mraa_gpio_write(clk, 1);
        mraa_gpio_write(clk, 0);
        response = response | mraa_gpio_read(miso);
    }
    
    mraa_gpio_write(cs,1);
    
    return response;
}

#ifndef EXTERN_CALL
int main(int argc, char * argv[])
{
    rfm_init();
    uint16_t data;
    
    if  ((argc !=2) || (sscanf(argv[1],"%X",&data)!=1))
    {
        printf("argument error\n");
        return 1;
    }
    
    printf("0x%04X\n", rfm_cmd(data));
    
    return 0;
}
#endif
