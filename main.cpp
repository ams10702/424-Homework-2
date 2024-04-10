#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
#include "planeSensor.h"
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>


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
// this implementation always does it in the fastest possible manner, but the specific robots vary based on thread timing

void airplaneApproach(int& landingQueue, bool& talking, int planeNum) { //this is incomprehensible but it works
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
		m1.lock(); //Unlocks the unique mutex since it just needs to cout some stuff
		std::cout << "Aircraft " << planeNum << " is cleared to land.\n";
		std::cout << "Runway is now free.\n";
		m1.unlock();
	}
	else if ((m6.try_lock() == false) && landingQueue++ < 3) { //preincrement makes sure that it goes up before the lock
		std::unique_lock<std::mutex> ATC(m6); //different instance of unique lock
		m1.lock();
		std::cout << "Aircraft " << planeNum << " is in the holding pattern\n";
		m1.unlock();
		ATCtalking.wait_for(ATC,std::chrono::seconds(5)); //avoids having all of them freeze here
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
		m1.unlock(); //I don't know why this has an error here since nothing ever happens
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
	std::cout << "Duration was " << duration.count() << " seconds \n";
	
	//Problem 3

	int queue = 0;
	bool ATCawake = false;
	startTime = std::chrono::high_resolution_clock::now();
	std::thread airplane1(airplaneApproach, std::ref(queue), std::ref(ATCawake), 1); //I tried to make a vector of threads but it didn't work
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
	stopTime = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::seconds>(stopTime - startTime);
	std::cout << "Duration was " << duration.count() << " seconds \n";

	
	//Problem 4 using OpenGL

	if (!glfwInit()) { //basic error handling code from slides
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(500, 500, "Sine Curve using OpenGL with GLEW and GLFW", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_LINES);
		// Draw X and Y axis
		glColor3f(1.0, 1.0, 1.0);
		glVertex2f(-1.0, 0.0);
		glVertex2f(1.0, 0.0);
		glVertex2f(0.0, 1.0);
		glVertex2f(0.0, -1.0);

		// Plot sine curve
		glColor3f(0.0, 1.0, 0.0);
		float xtemp = 0; //records place of previous stuff
		float ytemp = 0;
		for (float i = 0; i < 1; i += 0.001) {
			float x = i;
			float y = (log(4*i))/4; // I chose the log function because it shows up everywhere, and fundamentally relates
			//anything that changes with reference to itself, such as the rocket equation.
			glVertex2f(xtemp, ytemp); //graphs line between previous point and itself
			glVertex2f(x, y);
			xtemp = x;
			ytemp = y;
		}//thankfully log(0) doesn't break this whole equation
		glEnd();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();


	return 0;
}