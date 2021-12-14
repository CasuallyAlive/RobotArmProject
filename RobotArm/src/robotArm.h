#include <Arduino.h>
#include <Servo.h>
#include <Stepper.h>

Servo servoChannels[4];// array of servo segments
Servo horizontalRot1, horizontalRot2, horizontalRot3, gripper;
const int stepsPerRev = 360;
Stepper verticalRot = Stepper(stepsPerRev,2,4,3,7);
int joyStickX {0}, joyStickY {0}, pinX {A0}, pinY {A1}; //Variables associated with the potentiometers that control the arm
int segmentInControl; //portion of arm that is in control: bottom segment, middle segment, top segment and grip (claw).
const int analogMax {1023}; //max value of analog in.
int defaultAngle[3] {30,0,90}; //default position of arm on startup
int currentAngle[3]; // The current angle of either the top or bottom segment
int angularVelocity; // Velocity of rotation
int pushbuttonSegmentShifter{13};
int changeSegment;
// int pushbuttonDeactivateClaw{12};
// int clawMovement;
const int closedAngle {180}, openAngle {145};
// boolean clawClosed;

void rotateHorizontalSegment();
void setServoArray();
int getAngle(int readSignal); 
void armRotation(int indexOfArm);
void updateAngularVelocity(); 
void initiateArmSegments(); 
void activateGripper();
void deactivateGripper();
int magnitude(int angle);
void updateCurrentAngle(int indexOfArm);
int getRPM();
void updateStep();
int getStep();
void updateSegment();
void clawActivation();