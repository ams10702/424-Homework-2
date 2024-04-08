#pragma once

#include <memory>
#include <iostream>
#include <string>
#include <vector>

//Question 1 classes

class Sensor {
public:
	Sensor();
	~Sensor();
	virtual void gatherData();
	virtual void processData();
};

class PitotTube : public Sensor {
public:
	void gatherData() {
		std::cout << "Gathering data from pitot tube\n";
	}
	void processData() {
		std::cout << "Processing data from pitot tube\n";
	}
	PitotTube();
	~PitotTube();
};

class Accelerometer : public Sensor {
public:
	void gatherData() {
		std::cout << "Gathering data from accelerometer \n";
	}
	void processData() {
		std::cout << "Processing data from accelerometer\n";
	}
	Accelerometer();
	~Accelerometer();
};

class Altimeter : public Sensor {
public:
	void gatherData() {
		std::cout << "Gathering data from altimeter \n";
	}
	void processData() {
		std::cout << "Processing data from altimeter \n";
	}
	Altimeter();
	~Altimeter();
};

class sensorFactory {
public:
	sensorFactory();
	~sensorFactory();
	static std::shared_ptr<Sensor> createSensor(std::string sensor) { //shared pointers b/c im not sure how many need to exist
		std::shared_ptr<Sensor> sensorPtr = nullptr;
		if (sensor == "Altimeter"){
			sensorPtr = std::make_shared<Altimeter>();
		}
		else if (sensor == "Pitot Tube") {
			sensorPtr = std::make_shared<PitotTube>();
		}
		else if (sensor == "Accelerometer") {
			sensorPtr = std::make_shared<Accelerometer>();
		}
		if (sensorPtr == nullptr) {
			std::cout << "Err: Invalid sensor string, nullptr assigned \n";
		}
		return sensorPtr;
	}
};

class AerospaceControlSystem {
	std::vector<std::shared_ptr<Sensor>> sensors;
public:
	AerospaceControlSystem();
	~AerospaceControlSystem();
	void addSensor(std::shared_ptr<Sensor> sensor) {
		sensors.push_back(sensor);
	}
	void MonitorAndAdjust() {
		for (int i = 0; i < sensors.size(); i++) {
			(*sensors.at(i)).gatherData();
		}
		for (int i = 0; i < sensors.size(); i++) {
			(*sensors.at(i)).processData();
		}
		std::cout << "Adjusting controls based on sensor data \n";
	}
};
