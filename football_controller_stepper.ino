#include "PCA9685.h"
#include <Wire.h>

#include <AccelStepper.h>

#define SLIDE_STEPPER_COUNT	2
#define KICK_STEPPER_COUNT	2

#define ENABLE_PIN		8

AccelStepper slideSteppers[2];
AccelStepper kickSteppers[2];

int kickState[2];

void setup() {
  // Begin writing/reading serial at 9600 baud
  Serial.begin(9600);

  // Enable the CNC shield
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW);

  // Set up pins
  slideSteppers[0] = new AccelStepper(1, 2, 5);
  slideSteppers[1] = new AccelStepper(1, 3, 6);
  kickSteppers[0] = new AccelStepper(1, 4, 7);
  kickSteppers[1] = new AccelStepper(1, 12, 13);

  // Set speeds and accelerations for all motors
  for (int i = 0; i < 2; i++) {
    slideSteppers[i].setMaxSpeed(200);
    slideSteppers[i].setAcceleration(100);

    kickSteppers[i].setMaxSpeed(200);
    kickSteppers[i].setAcceleration(100);
  }

  // Init kick states
  kickState[0] = 0;
  kickState[1] = 0;
}

void loop() {
  // Get the serial input
  parse_serial();

  // Step the motors
  slideSteppers[0].run();
  slideSteppers[1].run();

  process_kick_states();
}

// Kick the ball
void kick_ball(int rod, int level, int dir) {
  // Set the kick speed according to the level
  kickSteppers[rod].setMaxSpeed(500*level);

  // Set up the state with respect to the direction
  kickState[rod] = 2 * dir;
  kickSteppers[rod].moveTo(-5 * dir);
}

// Process a kick state
void process_kick_states() {
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
