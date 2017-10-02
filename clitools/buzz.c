#include <unistd.h>
#include <signal.h>
#include <mraa.h>
int buzz_pin, buzz_tone, buzz_length;
float buzz_volume;

#define BUZZ_VOLUME 1
#define BUZZ_SUSTAIN 500000
#define BUZZ_NOTE 3800

void print_usage()
{
    printf("USAGE: buzz PIN TONE VOLUME LENGTH\n");
    printf(" PIN: IO pin for PWM (3/5/6/9)\n");
    printf(" TONE: period in microsec (octave: 3800, 3400, 3000, 2900, 2550, 2270, 2000, 1900)\n");
    printf(" VOLUME: 0..1\n");
    printf(" LENGTH: millisec\n");
}

int main(int argc, char **argv) 
{
    if (argc != 5)
    {
	print_usage();
	return 0;
    }

    sscanf(argv[1],"%i",&buzz_pin);
    sscanf(argv[2],"%i",&buzz_tone);
    sscanf(argv[3],"%f",&buzz_volume);
    sscanf(argv[4],"%i",&buzz_length);

    // init
    mraa_pwm_context m_pwm_context = mraa_pwm_init(buzz_pin);
    if (m_pwm_context == NULL)
    {
	printf("error while openening pin %i for pwm\n", buzz_pin);
	return 1;
    }
    mraa_pwm_enable (m_pwm_context, 1);

    // play
    mraa_pwm_period_us(m_pwm_context, buzz_tone);
    mraa_pwm_write(m_pwm_context, buzz_volume * 0.5);

    // sustain
    usleep(buzz_length*1000);

    // stop
    mraa_pwm_period_us(m_pwm_context, 1);
    mraa_pwm_write(m_pwm_context, 0);

    // clean up
    mraa_pwm_enable(m_pwm_context, 0);
    mraa_pwm_close(m_pwm_context);

    return 0;
}
