/*
 * autoPark.cpp
 * - The main program to run the parking function.
 */
#include "Aria.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;

// Constants
#define TURNING_RADIUS 1000
#define ROBOT_RADIUS 227.5
#define DEPTH_BOUND 1000
#define MAR_ERR 50
#define VMAX 500
#define LASER_ANGLE 90
#define OMEGA_MAX 2.618
#define TRUE 1
#define FALSE 0

// Global variables for robot and laser
ArRobot robot;
ArSick sick;
FILE *logfp;

/*
 * initialize
 * - A function to initialize the robot.
 */
int initialize(int *argc, char **argv) {
    int ret;
    std::string str;
    ArSerialConnection laserCon;
    ArSerialConnection con;
    
    // Manditory init call
    Aria::init();

    // Parse all our args
    ArSimpleConnector connector(argc, argv);
    connector.parseArgs();
    
    // Add the laser device
    robot.addRangeDevice(&sick);
    
    // Connect to the robot using default port, exit on failure
    if ((ret = con.open("/dev/ttyUSB1")) != 0)
    {
        str = con.getOpenMessage(ret);
        printf("Open failed: %s\n", str.c_str());
        Aria::shutdown();
        return 1;
    }

    // Set the robot to use the given connection
    robot.setDeviceConnection(&con);
    
    // Do a blocking connect, exit on failure
    if (!robot.blockingConnect())
    {
        printf("Could not connect to robot... exiting\n");
        Aria::shutdown();
        return 1;
    }
    
    // Set robot to stop the run if the connection is broken
    robot.runAsync(true);
    
    // Connect to the laser using default port, exit on failure
    sick.setDeviceConnection(&laserCon);
    if((ret = laserCon.open("/dev/ttyUSB0")) !=0){
        Aria::shutdown();
        return 1;
    }
    
    // Set laser to stop the run if connection is broken
    sick.configureShort(false);
    sick.runAsync();
    
    // Do a blocking connect, exit on failure
    if(!sick.blockingConnect()){
        printf("Could not get sick...exiting\n");
        Aria::shutdown();
        return 1;
    }
    printf("We are connected to the laser!");
    
    // Setup actions
    ArActionConstantVelocity constantVelocity("Constant Velocity", 400);
    // TODO: Turning actions
    // TODO: Reverse action

    // Add the actions
    robot.addAction(&constantVelocity, 20);

    // Return 0 for successful initialization
    return 0;
}


/*
 * scanForSpace
 * - A function to search for an open space using the SICK laser.
 */
void scanForSpace() {
    int i;
    double laser_dist[900], laser_angle[900];
    const std::list<ArSensorReading *> *readingsList;
    std::list<ArSensorReading *>::const_iterator it;

    // Initialize vars
    i = -1;
    readingsList = sick.getRawReadings();
    printf("Scanning...");

    // Store readings in array
    for (it = readingsList->begin(); it != readingsList->end(); it++) {
        i++;
	laser_dist[i] = (*it)->getRange();
	laser_angle[i] = (*it)->getSensorTh();
    }

    // Print results to logfile
    for (i = 0; i < 190; i++) { 
        fprintf(logfp, "Reading %d:\tLaser Dist: %f\tAngle: %f\n", 
                i, laser_dist[i], laser_angle[i]); 
    }
    fprintf(logfp, "\n");
    printf("done\n");
    return;
}


/*
 * parkRobot
 * - Function to park the robot.
 */
void parkRobot() {
    // TODO: Calculate center of circle one
    
    // TODO: Calculate triangle angle
    
    // TODO: Calculate x distance to move for alignment
    
    // TODO: Back up proper distance
    
    // TODO: First circle turn
    
    // TODO: Second circle turn
    
    return;
}


/*
 * openLogFile
 * - Function to open a logfile and write header.
 */
void openLogFile() {
    logfp = fopen("logfile.txt", "w");
    fprintf(logfp, "######################################################\n");
    fprintf(logfp, "## AUTO-PARK LOGFILE                                ##\n");
    fprintf(logfp, "## - This file contains data for a run of Auto-Park ##\n");
    fprintf(logfp, "######################################################\n\n");
    return;
}


/*
 * main
 * - Main function for the parking program.
 */
int main(int argc, char **argv) {
    // Open the logfile
    openLogFile();

    // Initialize the Robot
    fprintf(logfp, "## INITIALIZATION ##\n");
    if (initialize(&argc, argv) == 0) {
        fprintf(logfp, "Robot: Initialized\n");
        fprintf(logfp, "SICK: Initialized\n\n");
    }
    else {
        fprintf(logfp, "Initialization failed\n\n");
    }
  
    // Scan for parking space
    fprintf(logfp, "## SCAN FOR SPACE ##\n");
    scanForSpace();
  
    // When parking space is found, execute park function
    parkRobot();
    
    // Shutdown the robot
    //robot.waitForRunExit();
    Aria::shutdown();
    fclose(logfp);
    return 0;
}

// EOF
