// lidar project
#include <Servo.h>
#include "LIDARLite_v4LED.h"
#include <Wire.h>

char commandString[5];

Servo servoX;
Servo servoY;
float angleX = 0;
float angleY = 0;
int command = 0;
LIDARLite_v4LED lidar;
float distance = 0;

float offsetx = 0;
float offsety = 0;
float offsetz = 0;
float offsettheta = 0;
float offsetphi = 0;
float finalx = 0;
float finaly = 0;
float finalz = 0;

void setup() {
  // put your setup code here, to run once:
  servoX.attach(2); // <------------------------
  servoY.attach(3); // <------------------------
  Wire.begin();
  Serial.begin(9600);
  Serial.setTimeout(3000);

  // Serial.println("Moving to initial position.");
  servoX.write(angleX);
  servoY.write(angleY);

  // if (lidar.begin() == false) {
  //   Serial.println("Device did not acknowledge! Freezing.");
  //   while(1);
  // }
  // Serial.println("LIDAR acknowledged!");

}

void loop() {

  // put your main code here, to run repeatedly
  if (Serial.available() > 0) 
  {

    Serial.println("Enter command:");
    command = Serial.read();

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

  else if (command == 'r')
  {
    Serial.println("Getting distance...");
    lidar.takeRange();

    lidar.waitForBusy();

    distance = lidar.readDistance();
    finalx = offsetx+distance*cos((angleY+offsetphi)/180*PI)*sin((angleX+offsettheta)/180*PI);
    finaly = offsety+distance*sin((angleY+offsetphi)/180*PI);
    finalz = offsetz+distance*cos((angleY+offsetphi)/180*PI)*cos((angleX+offsettheta)/180*PI);
    Serial.print("Distance is "); Serial.println(distance);
    Serial.print("xyz coordinate is "); Serial.print(finalx); Serial.print("cm,");
    Serial.print(finaly); Serial.print("cm,");
    Serial.print(finalz); Serial.println("cm");

  }

  else if (command == 'o')
  {
    clean();

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

  else if (command == 't')
  {
    clean();

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

    servoX.write(angleX);
    servoY.write(angleY);
  }

  else if (command == 'i')
  {
    clean();

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

    servoX.write(angleX);
    servoY.write(angleY);
  }

  else if (command == 'l')
  {
    servoX.write(0);
    servoY.write(0);

    Serial.println("Starting to scan...");

    for (int j = 0; j < 110;)
    {
      j += 10;
      servoY.write(j);
      for (int i = 0; i < 180; i++)
      {
        servoX.write(i);
        delay(100);
      }
    }

    Serial.println("Going back to initial position...");
    servoX.write(angleX);
    servoY.write(angleY);
  }

  else 
  {
    Serial.println("Unknown command.");
  }

  // clean buffer
  clean();

  }



}

// cleans buffer
void clean()
{
  while (Serial.read() != -1)
  {
    Serial.println("cleaning...");
  }

  return 0;
}