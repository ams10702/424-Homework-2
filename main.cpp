#include <iostream>
#include <thread>
#include <memory>
#include "planeSensor.h"




int main() {
	//Problem 1
	AerospaceControlSystem ctrlSys;
	ctrlSys.addSensor(sensorFactory::createSensor("Altimeter"));
	ctrlSys.addSensor(sensorFactory::createSensor("Accelerometer"));
	ctrlSys.addSensor(sensorFactory::createSensor("PitotTube"));
	ctrlSys.MonitorAndAdjust();
	//Problem 2
	return 0;
}