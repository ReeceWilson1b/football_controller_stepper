#include "PCA9685.h"
#include <Wire.h>

#include <AccelStepper.h>
#include <SoftwareSerial.h>

//PAIR_SELECTOR -> 0 = Left two rods, 1 = right two rods
#ifdef ARDUINO_LEFT
  #define PAIR_SELECTOR 0
#elif defined(ARDUINO_RIGHT)
  #define PAIR_SELECTOR 1
#else
  #error "No Arduino selected"
#endif

#define STEPPER_COUNT  4
#define ENABLE_PIN  8

AccelStepper stepperX(AccelStepper::DRIVER, 2, 5);
AccelStepper stepperY(AccelStepper::DRIVER, 3, 6);
AccelStepper stepperZ(AccelStepper::DRIVER, 4, 7);
AccelStepper stepperA(AccelStepper::DRIVER, 12, 13);

AccelStepper *steppers[STEPPER_COUNT] = { &stepperX, &stepperY, &stepperZ, &stepperA };

SoftwareSerial rxSerial(A0, A1);

int kickState[2];

void setup() {
  // Begin writing/reading serial at 9600 baud
  Serial.begin(9600);   // USB output to Pi
  rxSerial.begin(9600); // RX input from Pi

  // Disable the CNC shield
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH);

  // Set speeds and accelerations for all motors
  for (int i = 0; i < STEPPER_COUNT; i++) {
    steppers[i]->setMaxSpeed(2500);
    steppers[i]->setAcceleration(2000);
    steppers[i]->moveTo(0);
  }
  
  steppers[0]->disableOutputs();
  steppers[1]->disableOutputs();

  steppers[2]->disableOutputs();
  steppers[3]->disableOutputs();

  // Init kick states
  kickState[0] = 0;
  kickState[1] = 0;

  // Enable shield
  digitalWrite(ENABLE_PIN, LOW);
}

void loop() {
  // Get the serial input  
  parse_serial();

  for (int i = 0; i < 4; i++) {
    if (steppers[i]->distanceToGo() != 0) {
      steppers[i]->run();
      //int playerRod = ((i + (2 * PAIR_SELECTOR)) << 6);
      //int playerPosition = (int)((float)(steppers[i]->currentPosition()) * 0.1024);  // 0.1024 = 64/1024
      //char outputByte = (char)(playerRod + playerPosition);
    }
    else
      steppers[i]->disableOutputs();
  }

  //process_kick_states();
}

// Kick the ball
void kick_ball(int motorID, int fast, int angle) {
  motorID += 2;
  steppers[motorID]->setMaxSpeed((fast + 1) * 750);
  steppers[motorID]->moveTo(angle * 10);
  steppers[motorID]->enableOutputs();
}

void process_kick_states()
{
  // 0 - Not kicking
  // 1 - Moving to position
  // 2 - Returning from position

  // Iterate through both rods' states
  for (int i = 0; i < 2; i++) {

    // If we are in the process of kicking
    if (kickState[i] != 0) {

      // If we are not yet at the target
      if (steppers[i+2]->distanceToGo() != 0) {

        // Step through to the target
        steppers[i+2]->run();
      } else {

        // If we have reached the target
        if (kickState[i] == 1) {

          // Target back and set new kick state
          steppers[i+2]->moveTo(0);
          kickState[i] = 2;

        // if we have reached back
        } else if (kickState[i] == 2) {

          // Disable the motor
          steppers[i+2]->disableOutputs();
          kickState[i] = 0;
        }
      }
    }
  }
}

// Shit's hit the fan random movement

/*


void loop() {
  for (int i = 0; i < 2; i++) {
    if (slideSteppers[i].distanceToGo() != 0) {
      slideSteppers[i].moveTo(int num = random() % 826);
    } else {
      slideSteppers[i].run();
    }
  }
}


 */


char *binary_digits[16] = {
  "0000",
  "0001",
  "0010",
  "0011",
  "0100",
  "0101",
  "0110",
  "0111",
  "1000",
  "1001",
  "1010",
  "1011",
  "1100",
  "1101",
  "1110",
  "1111"
};

void print_binary_8(char number) {
  Serial.print(binary_digits[(number & 0xF0) >> 4]);
  Serial.print(binary_digits[number & 0x0F]);
  Serial.print("\n");
}
