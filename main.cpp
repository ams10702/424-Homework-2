#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
#include "planeSensor.h"
#include <chrono>
#include <atomic>
#include <condition_variable>


std::mutex m1, m2, m3, m4, m5;
std::condition_variable ATCtalking;
std::unique_lock<std::mutex> ATC;


//This is the dumbest possible way of doing this but I tried to do it in a clever way and it didn't work
//couldn't figure out how to make the function know which mutex to use
void robot1() {
	std::lock(m1,m2);
	std::cout << "Robot 1 is getting tools \n";
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "Robot 1 has gotten tools and is completing its task \n";
	std::this_thread::sleep_for(std::chrono::seconds(5));
	std::cout << "Robot 1 has completed its task and is returning its tools\n";
	std::this_thread::sleep_for(std::chrono::seconds(1));
	m1.unlock();
	m2.unlock();
}
void robot2() {
	std::lock(m2, m3);
	std::cout << "Robot 2 is getting tools \n";
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "Robot 2 has gotten tools and is completing its task \n";
	std::this_thread::sleep_for(std::chrono::seconds(5));
	std::cout << "Robot 2 has completed its task and is returning its tools\n";
	std::this_thread::sleep_for(std::chrono::seconds(1));
	m2.unlock();
	m3.unlock();
}
void robot3() {
	std::lock(m3, m4);
	std::cout << "Robot 3 is getting tools \n";
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "Robot 3 has gotten tools and is completing its task \n";
	std::this_thread::sleep_for(std::chrono::seconds(5));
	std::cout << "Robot 3 has completed its task and is returning its tools\n";
	std::this_thread::sleep_for(std::chrono::seconds(1));
	m3.unlock();
	m4.unlock();
}
void robot4() {
	std::lock(m4, m5);
	std::cout << "Robot 4 is getting tools \n";
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "Robot 4 has gotten tools and is completing its task \n";
	std::this_thread::sleep_for(std::chrono::seconds(5));
	std::cout << "Robot 4 has completed its task and is returning its tools\n";
	std::this_thread::sleep_for(std::chrono::seconds(1));
	m4.unlock();
	m5.unlock();
}
void robot5() {
	std::lock(m5, m1);
	std::cout << "Robot 5 is getting tools \n";
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "Robot 5 has gotten tools and is completing its task \n";
	std::this_thread::sleep_for(std::chrono::seconds(5));
	std::cout << "Robot 5 has completed its task and is returning its tools\n";
	std::this_thread::sleep_for(std::chrono::seconds(1));
	m1.unlock();
	m5.unlock();
}
// defining 5 identical functions is dumb but I seriously tried to make it work with one function and couldnt

void airplaneApproach(std::atomic<int>& landingQueue, std::atomic<bool>& talking, int planeNum) {
	std::cout << "Aircraft " << planeNum << " is requesting clearance for landing.\n";
	if (talking == false) {
		ATC.lock();
		talking = true;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "Aircraft " << planeNum << " is cleared to land.\n";
		std::cout << "Runway is now free.\n";
		talking = false;
		ATC.unlock();
		ATCtalking.notify_one();
	}
	else if ((talking == true) && (landingQueue < 3)) {
		landingQueue++;
		ATCtalking.wait(ATC);
		ATC.lock();
		talking = true;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "Aircraft " << planeNum << " is cleared to land.\n";
		std::cout << "Runway is now free.\n";
		talking = false;
		ATC.unlock();
		ATCtalking.notify_one();
	}
	else {
		std::cout << "Approach pattern full. Aircraft " << planeNum << " is diverting to another airport.\n";
	}
}




int main() {
	//Problem 1
	AerospaceControlSystem ctrlSys;
	ctrlSys.addSensor(sensorFactory::createSensor("Altimeter"));
	ctrlSys.addSensor(sensorFactory::createSensor("Accelerometer"));
	ctrlSys.addSensor(sensorFactory::createSensor("PitotTube"));
	ctrlSys.MonitorAndAdjust();

	//Problem 2

	std::jthread r1(robot1); //jthreads to avoid having to tell them to join later
	std::jthread r2(robot2);
	std::jthread r3(robot3);
	std::jthread r4(robot4);
	std::jthread r5(robot5);

	//Problem 3

	std::atomic<int> queue = 0;
	std::atomic<bool> ATCawake = false;
	std::vector<std::jthread> airplanes;
	for (int i = 0; i++; i < 10) {
		airplanes.push_back(std::jthread(airplaneApproach, std::ref(queue), std::ref(ATCawake),i+1));
	}


	//Problem 4



	return 0;
}