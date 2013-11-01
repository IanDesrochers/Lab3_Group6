#include "stm32f4xx.h"

#ifndef MAX_PWM_INTENSITY
#define MAX_PWM_INTENSITY 500
#endif

#ifndef PWM_FREQUENCY
#define PWM_FREQUENCY 500
#endif

#ifndef PWM_PULSE_SPEED
#define PWM_PULSE_SPEED 2000
#endif

#ifndef PWM_UPDATE_INTENSITY_FREQUENCY
#define PWM_UPDATE_INTENSITY_FREQUENCY 5*MAX_PWM_INTENSITY
#endif

struct LED_PWM {
	uint32_t pwm_pulse_speed;
	uint32_t pwm_intensity;
	uint32_t max_pwm_intensity;
	uint32_t pwm_direction;
	uint32_t led_pwm_update_pulse_count;
	uint32_t CCR;
};

void init_PWM(void);
void init_LED_PWM(struct LED_PWM *led_pwm, uint32_t max_pwm_intensity, uint32_t pwm_pulse_speed, uint32_t phase, uint32_t CCR);
void update_led_pwm_intensity_pulse(struct LED_PWM *led_pwm);
void update_led_intensities(uint32_t led_intensities[], uint32_t length);
