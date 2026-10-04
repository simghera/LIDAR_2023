// lidar project
// Pan/tilt LIDAR controlled over serial: move the servos, take distance
// readings, and convert them to xyz coordinates.

// ---------- Libraries ----------
#include <Servo.h>
#include "LIDARLite_v4LED.h"
#include <Wire.h>

// ---------- Global variables ----------
char commandString[5];           // buffer for numbers typed into the serial monitor

Servo servoX;                    // pan servo (theta, left/right)
Servo servoY;                    // tilt servo (phi, up/down)
float angleX = 0;                // current pan angle (degrees)
float angleY = 0;                // current tilt angle (degrees)
int command = 0;                 // last command character received
LIDARLite_v4LED lidar;
float distance = 0;              // last distance reading (cm)

// Offsets of the sensor's position/orientation, applied to the coordinates
float offsetx = 0;
float offsety = 0;
float offsetz = 0;
float offsettheta = 0;
float offsetphi = 0;

// Calculated xyz coordinate of the last reading
float finalx = 0;
float finaly = 0;
float finalz = 0;

// ---------- Setup: runs once at power-up ----------
void setup() {

  servoX.attach(2); // <------------------------
  servoY.attach(3); // <------------------------
  Wire.begin();                  // start I2C (used by the LIDAR)
  Serial.begin(9600);            // start serial communication
  Serial.setTimeout(3000);       // wait up to 3 s when reading typed numbers

  // Move both servos to the starting position
  // Serial.println("Moving to initial position.");
  servoX.write(angleX);
  servoY.write(angleY);

  // LIDAR connection check (currently disabled)
  // if (lidar.begin() == false) {
  //   Serial.println("Device did not acknowledge! Freezing.");
  //   while(1);
  // }
  // Serial.println("LIDAR acknowledged!");

}

// ---------- Main loop: waits for a command and runs it ----------
void loop() {

  if (Serial.available() > 0) 
  {

    Serial.println("Enter command:");
    command = Serial.read();     // read one command character

    // 'a' : pan left by 10 degrees (stops at 0)
    if (command == 'a')
    {
      angleX -= 10;
      if (angleX < 0) 
      {      
        Serial.println("too far left");
        angleX = 0;
        return -1;
      }
      servoX.write(angleX);
      Serial.println("moving to the left");
      delay(100);
    }

    // 'd' : pan right by 10 degrees (stops at 180)
    else if (command == 'd')
    {
      angleX += 10;
      if (angleX > 180) 
      {      
        Serial.println("too far right");
        angleX = 180;
        return -1;
      }
      servoX.write(angleX);
      Serial.println("moving to the right");
      delay(100);
    }

  // 'w' : tilt up by 10 degrees (stops at 110)
  else if (command == 'w')
  {
    angleY += 10;
    if (angleY >= 110) 
    {      
      Serial.println("too far up");
      angleY = 110;
      return -1;
    }
    servoY.write(angleY);
    Serial.println("moving up");
    delay(100);
  }

  // 's' : tilt down by 10 degrees (stops at 0)
  else if (command == 's')
  {
    angleY -= 10;
    if (angleY < 0) 
    {      
      Serial.println("too far down");
      angleY = 0;
      return -1;
    }
    servoY.write(angleY);
    Serial.println("moving down");
    delay(100);
  }

  // 'r' : take a distance reading and convert it to an xyz coordinate
  else if (command == 'r')
  {
    Serial.println("Getting distance...");
    lidar.takeRange();           // start a measurement

    lidar.waitForBusy();         // wait until the measurement is done

    distance = lidar.readDistance();   // read the result (cm)

    // Spherical to Cartesian conversion (angles converted from degrees to radians), plus offsets
    finalx = offsetx+distance*cos((angleY+offsetphi)/180*PI)*sin((angleX+offsettheta)/180*PI);
    finaly = offsety+distance*sin((angleY+offsetphi)/180*PI);
    finalz = offsetz+distance*cos((angleY+offsetphi)/180*PI)*cos((angleX+offsettheta)/180*PI);

    // Print the distance and the coordinate
    Serial.print("Distance is "); Serial.println(distance);
    Serial.print("xyz coordinate is "); Serial.print(finalx); Serial.print("cm,");
    Serial.print(finaly); Serial.print("cm,");
    Serial.print(finalz); Serial.println("cm");

  }

  // 'o' : enter new offsets (x, y, z, theta, phi)
  else if (command == 'o')
  {
    clean();                     // clear leftover characters first

    Serial.println("Enter X offset:");
    Serial.readBytes(commandString, 5);
    offsetx = atof(commandString);

    Serial.println("Enter Y offset:");
    Serial.readBytes(commandString, 5);
    offsety = atof(commandString);

    Serial.println("Enter Z offset:");
    Serial.readBytes(commandString, 5);
    offsetz = atof(commandString);

    Serial.println("Enter theta offset:");
    Serial.readBytes(commandString, 5);
    offsettheta = atof(commandString);

    Serial.println("Enter phi offset:");
    Serial.readBytes(commandString, 5);
    offsetphi = atof(commandString);

  }

  // 'p' : print the current offsets and angles
  else if (command == 'p')
  {
    Serial.println("Current offset:");
    Serial.print(offsetx); Serial.print("cm,");
    Serial.print(offsety); Serial.print("cm,");
    Serial.print(offsetz); Serial.println("cm,");
    Serial.print(offsettheta); Serial.print("degrees theta,");
    Serial.print(offsetphi); Serial.println("degrees phi");
    Serial.println("Current angle:");
    Serial.print(angleX); Serial.print("degrees theta,");
    Serial.print(angleY); Serial.println("degrees phi");
  }

  // 't' : move to a typed target angle (theta, then phi)
  else if (command == 't')
  {
    clean();

    // Read and check the pan (theta) target
    Serial.println("Enter theta target:");
    Serial.readBytes(commandString, 5);
    angleX = atof(commandString);
    if (angleX < 0)  
      {      
        Serial.println("too far left");
        angleX = 0;
        return -1;
      }
    else if (angleX > 180) 
      {      
        Serial.println("too far right");
        angleX = 180;
        return -1;
      }

    // Read and check the tilt (phi) target
    Serial.println("Enter phi target:");
    Serial.readBytes(commandString, 5);
    angleY = atof(commandString);
    if (angleY < 0) 
    {      
      Serial.println("too far down");
      angleY = 0;
      return -1;
    }
    else if (angleY >= 110) 
    {      
      Serial.println("too far up");
      angleY = 110;
      return -1;
    }

    // Move both servos to the target
    servoX.write(angleX);
    servoY.write(angleY);
  }

  // 'i' : move by a typed increment (theta, then phi)
  else if (command == 'i')
  {
    clean();

    // Read and check the pan (theta) increment
    Serial.println("Enter theta increment:");
    Serial.readBytes(commandString, 5);
    angleX += atof(commandString);
    if (angleX < 0)  
      {      
        Serial.println("too far left");
        angleX = 0;
        return -1;
      }
    else if (angleX > 180) 
      {      
        Serial.println("too far right");
        angleX = 180;
        return -1;
      }

    // Read and check the tilt (phi) increment
    Serial.println("Enter phi increment:");
    Serial.readBytes(commandString, 5);
    angleY += atof(commandString);
    if (angleY < 0) 
    {      
      Serial.println("too far down");
      angleY = 0;
      return -1;
    }
    else if (angleY >= 110) 
    {      
      Serial.println("too far up");
      angleY = 110;
      return -1;
    }

    // Move both servos to the new position
    servoX.write(angleX);
    servoY.write(angleY);
  }

  // 'l' : sweep the full range row by row, then return to the previous position
  else if (command == 'l')
  {
    servoX.write(0);
    servoY.write(0);

    Serial.println("Starting to scan...");

    // Outer loop: step tilt up 10 degrees per row
    for (int j = 0; j < 110;)
    {
      j += 10;
      servoY.write(j);
      // Inner loop: pan across 1 degree at a time
      for (int i = 0; i < 180; i++)
      {
        servoX.write(i);
        delay(100);
      }
    }

    // Return to where the servos were before the scan
    Serial.println("Going back to initial position...");
    servoX.write(angleX);
    servoY.write(angleY);
  }

  // Any other character
  else 
  {
    Serial.println("Unknown command.");
  }

  // clean buffer
  clean();

  }



}

// ---------- Helper: empties the serial input buffer ----------
// cleans buffer
void clean()
{
  while (Serial.read() != -1)
  {
    Serial.println("cleaning...");
  }

  return 0;
}
