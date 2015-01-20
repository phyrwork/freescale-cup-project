#ifndef WHEEL_SLIP_SENSOR_H
#define WHEEL_SLIP_SENSOR_H

typedef struct {
	float value;
	WheelSpeedSensor_s  *front;
	WheelSpeedSensor_s  *rear;
} WheelSlipSensor_s

void InitWheelSlipSensors();
void UpdateWheelSlip(WheelSlipSensor_s *sensor);

#endif