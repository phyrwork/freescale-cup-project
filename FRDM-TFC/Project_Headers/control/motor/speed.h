//#include "sensors/speed.h"
#include "support/PID.h"
#include "sensors/motor/speed.h"

#ifndef MOTOR_SPEED_CONTROL_H_
#define MOTOR_SPEED_CONTROL_H_

#include "sensors/cadence.h"

typedef struct {
	float value;
	CadenceSensor_s *sensor;
	PID_s *pid;
} MotorSpeed_s;

void InitMotorSpeedControl();
void UpdateMotorSpeed(MotorSpeed_s *speed);
void SetMotorSpeed(MotorSpeed_s *speed, float command);


/////////////////////////////////////////
// LEGACY CODE: To be depreciated ASAP //
/////////////////////////////////////////

struct persistantPIDVariables_s{
	float PWM;
	float previousPWM;
	float error;
	float previousError;
	float dError;
	float errorSum;
};

float getSpeedEstimate(float measurement, volatile isNewMeasurementAvailable_t* isNewMeasurementAvailable, float acceleration, float dt);
float getDesiredMotorPWM(float setpoint, float measurement, volatile isNewMeasurementAvailable_t* isNewMeasurementAvailable, uint8_t channel);
void PID(float setpoint, float measurement, struct persistantPIDVariables_s* PIDVariables, uint8_t incrementIntegral, float dt);


#endif /* SPEEDCONTROL_H_ */