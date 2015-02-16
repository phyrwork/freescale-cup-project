#include "control/vehicle/speed.h"
#include "control/motor/torque.h"
#include "config.h"
#include "support/carState_s.h"
#include <math.h>

#define PX_OPP_CONST 0.7345f   //estimated relationship between steering angle and servo position
#define WIDTH_DIST 0.138f //distance between wheels
#define LENGTH_DIST 0.201f
#define RADIUS(center) ((PX_OPP_CONST * LENGTH_DIST) / center)
#define RADIUS_MIN1(center) (center * (1 / (PX_OPP_CONST * LENGTH_DIST)))

#define VEHICLE_SPEED_ACC 0.1f
#define VEHICLE_ACCEL_SLIP 0.15f
#define VEHICLE_DECEL_SLIP -0.15f


#define VEHICLE_TURN_MOD_LOW 5.0f
#define VEHICLE_TURN_MOD_HIGH 30.0f
#define VEHICLE_CORNERING_SPEED_LOW 6.0f
#define VEHICLE_CORNERING_SPEED_HIGH 6.5f
#define VEHICLE_STRAIGHT_SPEED 9.0f

#define VSPEED_KP 0.0080f
#define VSPEED_KI 0.0060f
#define VSPEED_KD 0.0f

VehicleSpeedControl_s VehicleSpeedControl;
PID_s pid;

void InitVehicleSpeedControl()
{
	VehicleSpeedControl_s * const speed = &VehicleSpeedControl;
	
	speed->cmd = 0;
	speed->sensor = &VehicleSpeedSensor;
	speed->left = &WheelSpeedControls[REAR_LEFT];
	speed->right = &WheelSpeedControls[REAR_RIGHT];
	speed->slip = &VehicleSlipControl;
	
	speed->pid = &pid;
	speed->pid->Kp = VSPEED_KP;
	speed->pid->Ki = VSPEED_KI;
	speed->pid->Kd = VSPEED_KD;
	speed->pid->time = 0;
	speed->pid->value = 0;
	speed->pid->value_max = 0.007;
	speed->pid->value_min = -0.007;
}

void _setByWheelSpeed(float command)
{
	VehicleSpeedControl_s * const speed = &VehicleSpeedControl;
	
	//calculate differential modifier coefficients
	float delta_w = (WIDTH_DIST / 2) * RADIUS_MIN1(carState.servoPosition) * command;
	float cmd_l = command - delta_w;
	float cmd_r = command + delta_w;
			
	//apply differential modified wheel speeds to wheels
	SetWheelSpeed(speed->left, cmd_l);
	SetWheelSpeed(speed->right, cmd_r);
}

void _setByVehicleSlip(float command)
{
	VehicleSpeedControl_s * const speed = &VehicleSpeedControl;
	
	if (command > speed->sensor->value) SetVehicleSlip(VEHICLE_ACCEL_SLIP);
	else SetVehicleSlip(VEHICLE_DECEL_SLIP);
}

void _setByVehicleSpeed(float command)
{
	VehicleSpeedControl_s * const speed = &VehicleSpeedControl;
	MotorTorque_s * const left = &MotorTorque[REAR_LEFT];
	MotorTorque_s * const right = &MotorTorque[REAR_RIGHT];
	
	UpdatePID(speed->pid, command, speed->sensor->value);
	SetMotorTorque(left, speed->pid->value);
	SetMotorTorque(right, speed->pid->value);
}

void SetVehicleSpeed(float command)
{
	VehicleSpeedControl_s * const speed = &VehicleSpeedControl;
	
	speed->cmd = command;
	UpdateVehicleSpeed();
	
	float diff = fabsf(speed->sensor->value - command);
	float thres = command * VEHICLE_SPEED_ACC;
	
	if (command < 1 && VehicleSpeedSensor.value < 0.5) _setByWheelSpeed(command);
	else _setByVehicleSpeed(command);
}

float GetVehicleSpeed(float modifier)
{
	//VehicleSpeedControl_s * const speed = &VehicleSpeedControl;

	modifier = fabsf(modifier);

	if ( modifier > VEHICLE_TURN_MOD_LOW )
	{
		float oscale = VEHICLE_CORNERING_SPEED_HIGH - VEHICLE_CORNERING_SPEED_LOW;
		float iscale = VEHICLE_TURN_MOD_HIGH - VEHICLE_TURN_MOD_LOW;
		float iratio = (modifier - VEHICLE_TURN_MOD_LOW)/iscale; //linear speed change between low and high cornerning
		return VEHICLE_CORNERING_SPEED_HIGH - (iratio * oscale);
	}
	else return VEHICLE_STRAIGHT_SPEED;
}
