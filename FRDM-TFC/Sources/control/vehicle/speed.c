#include "control/vehicle/speed.h"
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
#define VEHICLE_CORNERING_SPEED_LOW 4.5f
#define VEHICLE_CORNERING_SPEED_HIGH 6.5f
#define VEHICLE_STRAIGHT_SPEED 10.0f

VehicleSpeedControl_s VehicleSpeedControl;

void InitVehicleSpeedControl()
{
	VehicleSpeedControl_s * const speed = &VehicleSpeedControl;
	
	speed->cmd = 0;
	speed->sensor = &VehicleSpeedSensor;
	speed->left = &WheelSpeedControls[REAR_LEFT];
	speed->right = &WheelSpeedControls[REAR_RIGHT];
	speed->slip = &VehicleSlipControl;
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

void SetVehicleSpeed(float command)
{
	VehicleSpeedControl_s * const speed = &VehicleSpeedControl;
	
	speed->cmd = command;
	UpdateVehicleSpeed();
	
	float diff = fabsf(speed->sensor->value - command);
	float thres = command * VEHICLE_SPEED_ACC;
	
	if (speed->sensor->value < 2 || diff < thres) _setByWheelSpeed(command);
	else _setByVehicleSlip(command);
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
