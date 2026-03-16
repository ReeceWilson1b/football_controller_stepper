#include "PCA9685.h"
#include <Wire.h>

#include <AccelStepper.h>

#define STEPPER_COUNT  3
#define ENABLE_PIN  8

AccelStepper stepperX(AccelStepper::DRIVER, 2, 5);
AccelStepper stepperY(AccelStepper::DRIVER, 3, 6);
AccelStepper stepperZ(AccelStepper::DRIVER, 4, 7);
//AccelStepper stepperA(AccelStepper::DRIVER, 12, 13);

AccelStepper *steppers[STEPPER_COUNT] = { &stepperX, &stepperY, &stepperZ };     //, &stepperA };

int kickState[2];

void setup() {
  // Begin writing/reading serial at 9600 baud
  Serial.begin(9600);

  // Enable the CNC shield
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH);

  // Set speeds and accelerations for all motors
  for (int i = 0; i < STEPPER_COUNT; i++) {
    steppers[i]->setMaxSpeed(1000);
    steppers[i]->setAcceleration(1000);
    steppers[i]->moveTo(0);
  }

  //steppers[2]->disableOutputs();
  //steppers[3]->disableOutputs();

  steppers[0]->disableOutputs();
  steppers[1]->disableOutputs();

  // Init kick states
  kickState[0] = 0;
  kickState[1] = 0;
  
  digitalWrite(ENABLE_PIN, LOW);
}

void loop() {
  // Get the serial input
  parse_serial();

  // Step the motors
  if (steppers[0]->distanceToGo() != 0)
  {
    //steppers[0]->enableOutputs();
    steppers[0]->run();
    //steppers[0]->disableOutputs();
  }
  if (steppers[1]->distanceToGo() != 0)
  {
    //steppers[1]->enableOutputs();
    steppers[1]->run();
    //steppers[1]->disableOutputs();
  }

  //process_kick_states();
}

// Kick the ball
void kick_ball(int motorID, int level, int dir) {
  // Set the kick speed according to the level
  steppers[motorID]->setMaxSpeed(500*level);

  // Set up the state with respect to the direction
  kickState[motorID] = 2 * dir;
  steppers[motorID]->moveTo(-5 * dir);
}

// Process a kick state
/*void process_kick_states() {
  for (int i = 0; i < 2; i++) {

    // Check if we are in the process of kicking
    if (kickSteppers[i].distanceToGo() != 0)
      kickSteppers[i].run();

    // Process next states
    else if (kickSteppers[i].distanceToGo() == 0) {

      switch (kickState[i]) {
        // Swinging back
        case 2:
          kickSteppers[i].moveTo(-5);
          kickState[i] = 1;
          break;
        case -2:
          kickSteppers[i].moveTo(5);
          kickState[i] = -1;
          break;
        // Swinging forward
        case 1:
          kickSteppers[i].moveTo(25);
          kickState[i] = 0;
          break;
        case -1:
          kickSteppers[i].moveTo(-25);
          kickState[i] = 0;
          break;
        // Resetting
        case 0:
          kickSteppers[i].moveTo(0);
          break;
      }
    }
  }
}*/

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
