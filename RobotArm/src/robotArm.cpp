#include <Arduino.h>
#include <Servo.h>
#include <Stepper.h>
#include "robotArm.h"

void setup() {
  // put your setup code here, to run once:
  setServoArray(); 

  segmentInControl = 1; // bottom segment is in control by default

  pinMode(pushbuttonSegmentShifter,INPUT);
  // pinMode(pushbuttonDeactivateClaw,INPUT);

  angularVelocity = 0; // the velocity of a given segment's rotation is zero by default. 
  //Serial.begin(9600);
  initiateArmSegments(); // rotates every arm segment to its default position
  changeSegment = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  //reads analog data from potentiometer
  joyStickX = analogRead(pinX);
  delay(100);
  joyStickY = analogRead(pinY);
  delay(100); 
  //Serial.println(getAngle(joyStickX));
  changeSegment = digitalRead(pushbuttonSegmentShifter); 
  // clawMovement = digitalRead(pushbuttonDeactivateClaw);

  if(changeSegment == HIGH)
    updateSegment();
  // else if(clawMovement == HIGH)
  //   clawActivation();
  else{
    updateAngularVelocity(); // velocity is updated based on predefined steps.
    rotateHorizontalSegment();
    updateStep();
    // rotates an arm segment as defined by segmentInControl. 
  }
}
//rotates a given segment based on whichever has control.
void rotateHorizontalSegment() {
  switch (segmentInControl) {
    case 1: armRotation(0); break;
    case 2: armRotation(1); break;
    case 3: armRotation(2); break;
  }
}
//Makes an array for the servo segments
void setServoArray() {
  Servo servoChannelsArr[4] = {horizontalRot1, horizontalRot2, horizontalRot3, gripper};
   for(int index{0}; index < 3; index++) {
    servoChannels[index] = servoChannelsArr[index];
    servoChannels[index].attach(index + 9);
  }
  servoChannels[3].attach(5);
}
int getAngle(int readSignal) {
  int angle = ((readSignal*180l) / analogMax);
  return angle;
}
//handles the rotation of either the bottom or top segment based on whichever has control
void armRotation(int indexOfArm) {
  updateCurrentAngle(indexOfArm);
  int val = currentAngle[indexOfArm];
  servoChannels[indexOfArm].write(val);
  //Serial.println(val);
  delay(50);
}
//angular velocity is increased to set values based on the position of the joystick (pot)
void updateAngularVelocity() {
  int increaseJoy = getAngle(joyStickY);
  //Serial.println(increaseJoy);
  if(increaseJoy >= 72 && increaseJoy <= 92) {
 //Serial.println("noIncrease");
    angularVelocity = 0;
  } 
  else if(increaseJoy > 92){
   // Serial.println("increaseV");
    angularVelocity = (getAngle(joyStickY) < 135) ? 5 : 10; 
  } 
  else if(increaseJoy < 72){
  //  Serial.println("decreaseV");
    angularVelocity = (getAngle(joyStickY) > 40) ? -5: -10;
  }
}
/*
   Places the arm segments into their default position
*/
void initiateArmSegments() {
  for (int index = 0; index < 3; index ++) {
    servoChannels[index].write(defaultAngle[index]);
    currentAngle[index] = defaultAngle[index];
    delay(100);
  }
  servoChannels[3].write(openAngle); 
  // clawClosed = true;
  // deactivateGripper();
}

// void clawActivation(){
//   if(clawClosed)
//     deactivateGripper();
//   else
//     activateGripper();
// }

// void activateGripper() {
//     servoChannels[3].write(closedAngle);
//     delay(100);
//     clawClosed = !clawClosed;
// }

// void deactivateGripper() {
//     servoChannels[3].write(openAngle);
//     delay(100);
//     clawClosed = !clawClosed;
// }

int magnitude(int angle){
  return (angle < 90) ? (angle + 2*(90 - angle)): angle;
}
// Allows the angle of an arm segment to be incremented given that the direction of rotation and the limits of rotation are opposite.
void updateCurrentAngle(int indexOfArm){
  //if(currentAngle[indexOfArm] > 0 && currentAngle[indexOfArm] < 180) 
    //currentAngle[indexOfArm] += angularVelocity;
  if(angularVelocity < 0 && currentAngle[indexOfArm] > 0)
    currentAngle[indexOfArm] += angularVelocity;
  else if(angularVelocity > 0 && currentAngle[indexOfArm] < 180)
    currentAngle[indexOfArm] += angularVelocity;
  else
    return;
  //Serial.println(currentAngle[indexOfArm]);
}
// returns the angular velocity of the stepper motor. A max yaw on the joystick yields an rpm of 55 and a min yaw yields an rpm of 30
int getRPM(){
  //Serial.println(getAngle(joyStickX));
  //Serial.println(125 - (4500l/joyAngle));
   int motorSpeed = map(magnitude(getAngle(joyStickX)),90,180,0,100);
  return motorSpeed;
}
//the motor's step is incremented or decremented if the pot is not in the center
void updateStep(){
  if(getAngle(joyStickX) >= 70 && getAngle(joyStickX) <= 95)
    return;
 
  verticalRot.setSpeed(getRPM());
  verticalRot.step(getStep());
  delay(5);
}
//returns a positive or negative step size depending on whether the joystick is in the positive or negative position
int getStep(){
  if(getAngle(joyStickX) > 92)
    return -(stepsPerRev/64) - 20;
  if(getAngle(joyStickX) < 75);
    return stepsPerRev/64 + 20;
  return 0;
}
//shifts control to next arm segment
void updateSegment(){

  segmentInControl = (segmentInControl != 3) ? segmentInControl + 1: 1;
}