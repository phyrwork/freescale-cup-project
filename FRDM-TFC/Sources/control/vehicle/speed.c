#include "control/vehicle/speed.h"
#include "config.h"
#include "support/carState_s.h"
#include <math.h>

#define PX_OPP_CONST 65.0f   //estimated relationship between steering angle and line offset (px)
#define WIDTH_DIST 0.138f //distance between wheels
#define LENGTH_DIST 0.201f
#define RADIUS(center) ((PX_OPP_CONST * LENGTH_DIST) / center)
#define RADIUS_MIN1(center) (center * (1 / (PX_OPP_CONST * LENGTH_DIST)))

VehicleSpeedControl_s VehicleSpeedControl;

void InitVehicleSpeedControl()
{
	VehicleSpeedControl_s * const speed = &VehicleSpeedControl;
	
	speed->cmd = 0;
	speed->left = &WheelSpeedControls[REAR_LEFT];
	speed->right = &WheelSpeedControls[REAR_RIGHT];
}

void SetVehicleSpeed(float command)
{
	VehicleSpeedControl_s * const speed = &VehicleSpeedControl;
	
	speed->cmd = command;
	
	//calculate differential modifier coefficients
	float delta_w = (WIDTH_DIST / 2) * RADIUS_MIN1(carState.lineCenter) * command;
	float cmd_l = command - delta_w;
	float cmd_r = command + delta_w;
	
	//no slip control yet, so directly apply differential modified wheel speeds to wheels
	SetWheelSpeed(speed->left, cmd_l);
	SetWheelSpeed(speed->right, cmd_r);
}

