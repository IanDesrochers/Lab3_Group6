#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

struct Orientation {
	float roll;
	float pitch;
	float yaw;
};

void getOrientation(struct Orientation *orientation);
void init_accel(void);
