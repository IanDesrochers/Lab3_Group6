#include "stm32f4xx.h"

#include "lab3_filter.h"

#ifndef PI
#define PI 3.14159
#endif


//Calibration Matrix
#define ACC11 1.0177
#define ACC12 0.0202
#define ACC13 0.0003

#define ACC21 0.0076
#define ACC22 1.0109
#define ACC23 0.0030

#define ACC31 0.0052
#define ACC32 -0.0120
#define ACC33 0.9686


//Calibration Offsets
#define ACC10 18.5581
#define ACC20 21.9931
#define ACC30 86.8937

struct Orientation {
	int32_t rawx;
	int32_t rawy;
	int32_t rawz;
	int32_t x;
	int32_t y;
	int32_t z;
	float roll;
	float pitch;
	float yaw;
	struct Moving_Average moving_average_roll;
	struct Moving_Average moving_average_pitch;
	struct Moving_Average moving_average_yaw;
};

void update_orientation(struct Orientation *orientation);
void init_accelerometer(void);
void init_orientation(struct Orientation *orientation);
