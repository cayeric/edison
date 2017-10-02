#include "mraa.h"
#include "time.h"
#include <stdio.h>

#define INTERVAL 30

typedef struct
{
    unsigned long lastSwitch;
    double sumHigh;
    double sumLow;
    mraa_gpio_context sensor;
} Dusty;

void levelChange(void * data)
{
    Dusty * dusty = (Dusty*)data;

    //get the difference in milliseconds
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);

    unsigned long now = (t.tv_sec)*1000L + t.tv_nsec / 1000000L;

    double diff = ((double)(now - dusty->lastSwitch))/1000;

    if (mraa_gpio_read(dusty->sensor)==0)
    {
	dusty->sumLow += diff;
	mraa_gpio_write(dusty->led, 0);
    }
    else
    {
	dusty->sumHigh += diff;
	mraa_gpio_write(dusty->led, 1);
    }
    dusty->lastSwitch = now;
}

void resetDusty(Dusty * dusty)
{
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    dusty->lastSwitch = (t.tv_sec)*1000L + t.tv_nsec / 1000000L;

    dusty->sumHigh = 0;
    dusty->sumLow = 0;
}

int main(int argc, char * argv[])
{
    if (argc!=1)
    {
	 printf("Reading dust sensor Shinyei PPD42NS on given GPIO and print percent value for 30 sec interval\n");
	 printf("USAGE: dusty GPIO_PIN_NUMBER\n");
	 return 1;
    }
    Dusty dusty;

    int input = 0;
    sscanf(argv[1],"%i",&input);

    dusty.sensor = mraa_gpio_init(input);
    mraa_gpio_dir(dusty.sensor, MRAA_GPIO_IN);

    while(1)
    {
	resetDusty(&dusty);

        mraa_gpio_isr(dusty.sensor, MRAA_GPIO_EDGE_BOTH, levelChange, &dusty);
	sleep(INTERVAL);
	mraa_gpio_isr_exit(dusty.sensor);

	//finish reading
        if (dusty.sumLow + dusty.sumHigh < INTERVAL)
	{
	    if (mraa_gpio_read(dusty.sensor)==1)
	    {
		dusty.sumLow=INTERVAL-dusty.sumHigh;
	    }
	    else
	    {
		dusty.sumHigh=INTERVAL-dusty.sumLow;
	    }
	}

	char buf[1024];
	printf("%f\n", (dusty.sumLow/(dusty.sumLow+dusty.sumHigh)) * 100);
    }

    exit(0);
}