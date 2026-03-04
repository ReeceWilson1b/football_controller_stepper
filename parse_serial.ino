//PAIR_SELECTOR -> 0 = Left two rods, 1 = right two rods
#define PAIR_SELECTOR 0

void parse_serial() {
  // Bytes read in
  byte b1 = 0;
  byte b2 = 0;

  // For slide steppers
  float receivedPos = 0;
  int moveToPos = 0;

  // For kick steppers
  int level = 0;
  int dir = 0;

  // Wait until at least two bytes are available
  if (Serial.available() >= 2) {
    b1 = Serial.read();
    b2 = Serial.read();

    // Get the id of the current rod and the command opcode
    int currentRod = (b1 >> 4) & 0x03;
    byte opcode = (b2 >> 6) & 0x03;

    Serial.println(currentRod);

    // Only move if attached to the correct pair of rods
    if (!PAIR_SELECTOR && (currentRod == 0 || currentRod == 1)
        || PAIR_SELECTOR && (currentRod == 2 || currentRod == 3))
    {
      // Normalise currentRod to be 0 or 1
      if (currentRod > 1) {
      currentRod -= 2;
      }

      // If sliding
      if (opcode == 2) {
        // Get the position as a float between 0 and 1
        receivedPos = (float)(b2 & 0x3F) / 63.0f;

        // Convert that to a number between 0 and 826
        moveToPos = int(receivedPos * 826.0);

        // Set thye target position
        slideSteppers[currentRod].moveTo(moveToPos);
        Serial.println("moving to " + String(moveToPos));
      }
      // If kicking
      else if (opcode == 3) {
        // Get the direction
        dir = (b2 >> 5) & 0x01;

        // Change 0 ot 1 to -1 or 1
        if (dir == 0)
          dir = -1;

        // Get the strength level
        level = b2 >> 3 & 0x03;

        // Send the command to kick
        kick_ball(currentRod, level, dir);
        Serial.println("kicking with level " + String(level) + " in direction " + String(dir));
      }
    }
  }
}



// 00000000
// 01RR0000
// 10PPPPPP
// 11DLL000
