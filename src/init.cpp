/*
 * init.cpp - Implementation of initialization class
 */
#include "init.h"

int Init::initRobot()
{
  int ret;
  ArRobot robot;
  ArSick sick;
  ArSerialConnection laserCon;
  ArSerialConnection con;
  std::string str;

  // Manditory init call
  Aria::init();

  // Setup connector
  //ArSimpleConnector connector(&argc, argv);
  // connector.parseArgs();

  // Add the laser device
  robot.addRangeDevice(&sick);

  // Open the connection using the defaults, if it fails, exit
  if ((ret = con.open("/dev/ttyUSB0")) != 0)
  {
    str = con.getOpenMessage(ret);
    printf("Open failed: %s\n", str.c_str());
    Aria::shutdown();
    return 1;
  }
  
   // Set the robot to use the given connection
  robot.setDeviceConnection(&con);

  // Do a blocking connect, if it fails exit
  if (!robot.blockingConnect())
  {
    printf("Could not connect to robot... exiting\n");
    Aria::shutdown();
    return 1;
  }

  // Connect to the laser 
  sick.setDeviceConnection(&laserCon);
  if((ret=laserCon.open("/dev/ttyUSB1")) !=0){
	  Aria::shutdown();
	  return 1;
  }
  sick.configureShort(false);

  sick.runAsync();
  if(!sick.blockingConnect()){
	  printf("Could not get sick...exiting\n");
	  Aria::shutdown();
	  return 1;
  }
  printf("We are connected to the laser!");
  
  // Return 0 for successful initialization
  return 0;
}
// EOF
