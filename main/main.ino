#include <Servo.h>

Servo ESC;
// Define Input Connections
#define CH1 A1
#define CH2 A2
#define CH3 A3
//#define CH4 ...
//#define CH5 ...
//#define CH6 ...
#define CH7 A4
#define CH8 A5
#define CH9 2  // Use digital pin for Channel 9
#define CH10 3 // Use digital pin for Channel 10

// Integers to represent values from sticks and pots
int ch1Value; //Left/Right Value
int ch2Value; // UP/Down Value
int ch3Value; //Weapon Control
//int ch4Value; // Weapon L/R
//int ch5Value; // L/R Inverse
//int ch6Value; // Unbound
int ch7Value; // SwA Main On/Off
int ch8Value; // SwB Weapon Saftey
int ch9Value; // SwC (Bot Orientation)
int ch10Value; // SwD 

// Read the number of a specified channel and convert to the range provided.
// If the channel is off, return the default value
int readChannel(int channelInput, int minLimit, int maxLimit, int defaultValue) {
  int ch = pulseIn(channelInput, HIGH, 30000); // 30,000 microsecond width
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

bool upright() {
  // In the futute will check light level and determine orientation
  // Currently we just assume it's upright
  return true;
}

int adjustForOrientation(int inputVal, int switchVal) {
  // Most of this code is unneeded, the normalize function will always have the switch value at a round number
  // I simply just trust my code though so I'm using thresholds, it doesn't hurt
  if(switchVal >= 0 && switchVal <= 100) { // 100 is kinda arbitrary it just needs a bound
    // Switch is DOWN, invert value
    return inputVal * -1;
  } else if(switchVal >= 100 && switchVal <= 200) { // Again, numbers are arbitrary, just somewhere in the middle
    return upright() ? inputVal : inputVal * -1;
  } else {
    return inputVal;
  }
}

int normalize(int value) { // Used to normalize SW3 to a three-phase output only
    int outval;
    if (value < 64) outval = 0;          // Values near 0 map to 0
    else if (value < 191) outval = 127;       // Values in the middle map to 127
    else outval = 255;                        // Values near 255 map to 255

    return 255-outval; //Basically flips the switch
}

void setup() {
  // Set up serial communication
  Serial.begin(9600);

  ESC.attach(4, 1000, 2000);
  // Set all pins as inputs
  pinMode(CH1, INPUT);
  pinMode(CH2, INPUT);
  pinMode(CH3, INPUT);
  //pinMode(CH4, INPUT);
  //pinMode(CH5, INPUT);
  //pinMode(CH6, INPUT);
  pinMode(CH7, INPUT);
  pinMode(CH8, INPUT);
  pinMode(CH9, INPUT);  // Set Channel 9 to input
  pinMode(CH10, INPUT); // Set Channel 10 to input
}

void writeToMotors(int ch1, int ch2, int ch3) {
  /*
  We don't have drive motors rn so yeah
  */
  int speed = map(ch3Value, 0, 255, 0, 180);
  ESC.write(speed);
}

void loop() {
  // Get values for each channel
  ch7Value = readChannel(CH7, 0, 255, 0); //Sw1 (Main On/Off)
  ch8Value = readChannel(CH8, 0, 255, 0); //Sw2 (Weapon On/Off)
  ch9Value = readChannel(CH9, 0, 255, 0); //Sw3 (Orientation)
  ch10Value = readChannel(CH10, 0, 255, 0); //Sw4 (Not quite sure ab this one yet, weapon direction maybe?)
  int ch9Normalized = normalize(ch9Value);

  // Print switch values
  Serial.print(ch7Value); Serial.print(" ");
  Serial.print(ch8Value); Serial.print(" ");
  Serial.print(ch9Normalized); Serial.print(" ");
  Serial.print(ch10Value); Serial.print(" ");

  if(ch7Value < 100) { // Only while main power is on
    ch1Value = readChannel(CH1, -255, 255, 0); // Left/Right
    ch2Value = readChannel(CH2, -255, 255, 0); // Forward/Backward
    if(ch8Value > 10) { //Only read motor if motor power is on
      ch3Value = readChannel(CH3, 0, 255, 0); // Weapon Magnatude
    } else { //"comment that" - some nerd
      ch3Value = 0;
    }
  } else { //Set joysticks to 0 if power is off
    ch1Value = 0; // Left/Right
    ch2Value = 0; // Forward/Backward
    ch3Value = 0; // Weapon Magnatude
  }
  
  ch1Value = adjustForOrientation(ch1Value, ch9Normalized); // Adjust L/R for orientation
  // Print out joystick values
  Serial.print(ch1Value); Serial.print(" "); 
  Serial.print(ch2Value); Serial.print(" ");
  Serial.println(ch3Value);

  // Write to motors
  writeToMotors(ch1Value, ch2Value, ch3Value);
  `
  // Add a delay to allow for smooth plotting
  delay(20);
}
