#include "control/motor/pwm.h"
#include "sensors/motor/current.h"
#include "control/motor/torque.h"
#include "control/motor/speed.h"

typedef struct {
	MotorPWM_s *pwm;
	MotorCurrent_s *current;
	MotorTorque_s *torque;
	MotorSpeed_s *speed;
} Motor_s;

void InitMotorControl();