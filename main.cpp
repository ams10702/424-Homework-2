#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
#include "planeSensor.h"
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <GLFW/glfw3.h>
#include <GL/glew.h>


std::mutex m1, m2, m3, m4, m5, m6;
std::condition_variable ATCtalking;


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

void airplaneApproach(int& landingQueue, bool& talking, int planeNum) {
	m1.lock();
	std::cout << "Aircraft " << planeNum << " is requesting clearance for landing.\n";
	m1.unlock();
	if (talking == false) {
		std::unique_lock<std::mutex> ATC(m6);
		talking = true;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		talking = false;
		ATC.unlock();
		ATCtalking.notify_one();
		m1.lock();
		std::cout << "Aircraft " << planeNum << " is cleared to land.\n";
		std::cout << "Runway is now free.\n";
		m1.unlock();
	
	}
	else if ((m6.try_lock() == false) && landingQueue++ < 3) {
		std::unique_lock<std::mutex> ATC(m6);
		m1.lock();
		std::cout << "Aircraft " << planeNum << " is in the holding pattern\n";
		m1.unlock();
		ATCtalking.wait_for(ATC,std::chrono::seconds(5));
		talking = true;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		talking = false;
		ATC.unlock();
		ATCtalking.notify_one();
		m1.lock();
		std::cout << "Aircraft " << planeNum << " is cleared to land.\n";
		std::cout << "Runway is now free.\n";
		landingQueue--;
		m1.unlock();
	}
	else {
		m1.lock();
		std::cout << "Approach pattern full. Aircraft " << planeNum << " is diverting to another airport.\n";
		m1.unlock();
	}
}




int main() {
	//Problem 1
	AerospaceControlSystem ctrlSys;
	ctrlSys.addSensor(sensorFactory::createSensor("Altimeter"));
	ctrlSys.addSensor(sensorFactory::createSensor("Accelerometer"));
	ctrlSys.addSensor(sensorFactory::createSensor("Pitot Tube"));
	ctrlSys.MonitorAndAdjust();

	//Problem 2
	/*
	auto startTime = std::chrono::high_resolution_clock::now();
	std::thread r1(robot1); //jthreads to avoid having to tell them to join later
	std::thread r2(robot2);
	std::thread r3(robot3);
	std::thread r4(robot4);
	std::thread r5(robot5);
	r1.join();
	r2.join();
	r3.join();
	r4.join();
	r5.join();
	auto stopTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(stopTime - startTime);
	std::cout << "Duration was " << duration << " \n";
	*/
	//Problem 3

	int queue = 0;
	bool ATCawake = false;
	//startTime = std::chrono::high_resolution_clock::now();
	std::thread airplane1(airplaneApproach, std::ref(queue), std::ref(ATCawake), 1);
	std::thread airplane2(airplaneApproach, std::ref(queue), std::ref(ATCawake), 2);
	std::thread airplane3(airplaneApproach, std::ref(queue), std::ref(ATCawake), 3);
	std::thread airplane4(airplaneApproach, std::ref(queue), std::ref(ATCawake), 4);
	std::thread airplane5(airplaneApproach, std::ref(queue), std::ref(ATCawake), 5);
	std::thread airplane6(airplaneApproach, std::ref(queue), std::ref(ATCawake), 6);
	std::thread airplane7(airplaneApproach, std::ref(queue), std::ref(ATCawake), 7);
	std::thread airplane8(airplaneApproach, std::ref(queue), std::ref(ATCawake), 8);
	std::thread airplane9(airplaneApproach, std::ref(queue), std::ref(ATCawake), 9);
	std::thread airplane10(airplaneApproach, std::ref(queue), std::ref(ATCawake), 10);
	airplane1.join();
	airplane2.join();
	airplane3.join();
	airplane4.join();
	airplane5.join();
	airplane6.join();
	airplane7.join();
	airplane8.join();
	airplane9.join();
	airplane10.join();
	//stopTime = std::chrono::high_resolution_clock::now();
	//duration = std::chrono::duration_cast<std::chrono::seconds>(stopTime - startTime);


	//Problem 4 using OpenGL



	return 0;
}