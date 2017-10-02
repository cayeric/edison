#include "mraa.h"
#include <stdlib.h>

#define INTERVAL 30

int main(int argc, char * argv[])
{
    if (argc!=2)
	{
		printf("Reading analog value from input number X\n"); 
		printf("USAGE: analog ANX\n");
		return 1;
	}

	int input = 0;
	sscanf(argv[1],"%i",&input);
	mraa_aio_context sensor = mraa_aio_init(input);
	mraa_aio_set_bit(sensor, 12);

	printf("%i\n", mraa_aio_read(sensor));
}
