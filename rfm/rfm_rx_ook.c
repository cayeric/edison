#include <mraa.h>
#include <stdint.h>
#include <stdio.h>

#include "rfm_cmd.h"

#define TICK_INTERVAL 100
#define TICK usleep(TICK_INTERVAL);
#define END_OF_SIGNAL 50

#define AIO_ARSSI 3
#define GPIO_OOK 18

#define ARSSI_OFFSET 0x200
#define ARSSI_SCALE 2

mraa_aio_context arssi;
mraa_gpio_context ook;

void init()
{
    //config rfm spi
    rfm_init();

    rfm_cmd(0x8017); // 433 Mhz;disable tx register; disable RX fifo buffer; xtal cap 12pf
    rfm_cmd(0x82c0); // enable receiver ; enable basebandblock        = 430.2400 - 439.7575
    rfm_cmd(0xa680); // default freq offset 1.16 MHz (results in f=434.16)
    rfm_cmd(0xc691); // datarate 2395 kbps 0xc647 = 4.8kbps
    rfm_cmd(0xC220); // datafiltercommand ; ** not documented command **
    rfm_cmd(0xCA00); // FiFo and resetmode command ; FIFO fill disabeld
    rfm_cmd(0xC473); // AFC run only once ; enable AFC ;enable frequency offset register ; +3 -4
    rfm_cmd(0xCC67); // pll settings command
    rfm_cmd(0xB800); // TX register write command NOT USED
    rfm_cmd(0xC800); // low dutycycle disabeld
    rfm_cmd(0xC040); // 1.66MHz,2.2V NOT USED see 82c0
    rfm_cmd(0x948C); // VDI; FAST;200khz;GAIN -6db ;DRSSI 79dbm

    //enable arrsi
    arssi = mraa_aio_init(AIO_ARSSI);
    mraa_aio_set_bit(arssi, 12);

    //monitor ook input
    ook = mraa_gpio_init(GPIO_OOK);
    mraa_gpio_dir(ook, MRAA_GPIO_IN);
}

void rfm_ook_data()
{
    //wait until rf threshold
    while(mraa_gpio_read(ook) == 0)
    {
        TICK;
    }

    //loop until exit condition
    while(1)
    {
        while(mraa_gpio_read(ook) != 0)
        {
            printf("X");
            
            TICK;
        }

        int pulse_count = 0;
        while(mraa_gpio_read(ook) == 0)
        {
            if (pulse_count == END_OF_SIGNAL)
            {
                return;
            }
            
            printf(".");
            pulse_count++;

            TICK;
        }
    }

    return; // but will never be reached...
}


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("USAGE: rfm_rx_ook command [-f]\n");
        printf("commands:\ndata [repeat] - returns character graph of rfm DATA (repeat until ^C)\n");
        printf("arssi [repeat] - returns signal strength from rfm ARSSI (repeat until ^C)\n");
        return 0;
    }
    
    init();
    
    if (strcmp(argv[1], "data") == 0)
    {
        do
        {
            rfm_ook_data();
            printf("\n");
        } while ((argc > 2) && (strcmp(argv[2], "repeat") == 0));
        
        return 0;
    }

    if (strcmp(argv[1], "arssi") == 0)
    {
        do
        {
    	    unsigned int result = mraa_aio_read(arssi);
    	    result = (result - ARSSI_OFFSET)/ ARSSI_SCALE;
    	    for (;result > 0; result--) printf("X");
    	    printf("\n");
            TICK;
        } while ((argc > 2) && (strcmp(argv[2], "repeat") == 0));
        
        return 0;
    }
    return 0;
}
