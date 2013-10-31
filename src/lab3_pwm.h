#include "stm32f4xx.h"

#ifndef MAX_PWM_INTENSITY
#define MAX_PWM_INTENSITY 1000
#endif

#ifndef PWM_FREQUENCY
#define PWM_FREQUENCY 1000
#endif

struct LED_PWM {
	uint32_t pwm_intensity;
	uint32_t max_pwm_intensity;
	uint32_t pwm_direction;
};

void init_PWM(void);
void init_LED_PWM(struct LED_PWM *led_pwm, uint32_t max_pwm_intensity);
void update_led_pwm_intensities_pulse(struct LED_PWM *led_pwm);
void update_led_intensities(uint32_t led_intensities[], uint32_t length);
