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
#define VEHICLE_TURN_MOD_HIGH 20.0f
#define VEHICLE_TURN_MOD_MAX 30.0f
#define VEHICLE_CORNERING_SPEED_LOW 7.0f  //7.5
#define VEHICLE_CORNERING_SPEED_HIGH 9.0f //9.5
#define VEHICLE_STRAIGHT_SPEED 13.5f      //15.5

#define VSPEED_CONT_REGION(speed) ( 1.0f + (speed * 0.05f) )

#define VSPEED_KP_UNCONT 0.0085f //0.0085
#define VSPEED_KI_UNCONT 0.0000f
#define VSPEED_KD_UNCONT 0.0000f

#define VSPEED_KP_CONT 0.0012f //0.0012
#define VSPEED_KI_CONT 0.0060f //0.0060
#define VSPEED_KD_CONT 0.0000f

#define TORQUE_VECTORING_CONST 0.2f
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
	speed->pid->Kp = VSPEED_KP_UNCONT;
	speed->pid->Ki = VSPEED_KI_UNCONT;
	speed->pid->Kd = VSPEED_KD_UNCONT;
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
	
	//recalculate controller saturation point
	/*
	float tspd = speed->sensor->value > 31 ? 31 : speed->sensor->value;
	float tsat = (TORQUE_SPEED_M * tspd) + TORQUE_SPEED_C;
	speed->pid->value_max = tsat;
	speed->pid->value_min = -tsat;
	*/
	
	//set pid according to controllable region rules
	float sdiff = command - speed->sensor->value;
	if ( fabsf(sdiff) < VSPEED_CONT_REGION(speed->sensor->value) )
	{
		//speed in controllable region
		speed->pid->clamped = KI_ACTIVE;
		speed->pid->Kp = VSPEED_KP_CONT;
		speed->pid->Ki = VSPEED_KI_CONT;
		speed->pid->Kd = VSPEED_KD_CONT;
	}
	else
	{
		//speed in uncontrollable region
		speed->pid->clamped = KI_CLAMPED;
		speed->pid->Kp = VSPEED_KP_UNCONT;
		speed->pid->Ki = VSPEED_KI_UNCONT;
		speed->pid->Kd = VSPEED_KD_UNCONT;
	}
	
	UpdatePID(speed->pid, command, speed->sensor->value);
	
	//torque vectoring
	float lt = speed->pid->value * (1 - ( ( carState.servoPosition / VEHICLE_TURN_MOD_MAX ) * TORQUE_VECTORING_CONST ) );
	float rt = speed->pid->value * (1 + ( ( carState.servoPosition / VEHICLE_TURN_MOD_MAX ) * TORQUE_VECTORING_CONST ) );
		
	
	SetMotorTorque(left, lt);
	SetMotorTorque(right, rt);
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
	
	if ( modifier > VEHICLE_TURN_MOD_HIGH ) return VEHICLE_CORNERING_SPEED_LOW;
	if ( modifier > VEHICLE_TURN_MOD_LOW )
	{
		float oscale = VEHICLE_CORNERING_SPEED_HIGH - VEHICLE_CORNERING_SPEED_LOW;
		float iscale = VEHICLE_TURN_MOD_HIGH - VEHICLE_TURN_MOD_LOW;
		float iratio = (modifier - VEHICLE_TURN_MOD_LOW)/iscale; //linear speed change between low and high cornerning
		return VEHICLE_CORNERING_SPEED_HIGH - (iratio * oscale);
	}
	else return VEHICLE_STRAIGHT_SPEED;
}
