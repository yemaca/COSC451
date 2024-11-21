// black high white low
#define LW1 2
#define LW2 3  // wheel positions
#define RW1 4  // LW2 and RW1 high to turn forward
#define RW2 5

#define enaLW 11  // enable motor control for wheels
#define enaRW 12

#define RIGHT 6
#define FRONT 7  // sensor positions
#define LEFT 8

int R = 0;
int F = 0;  // variables to store sensor readings
int L = 0;

char path[128];
char optimized[32];
int index = 0;

void setup() {
  Serial.begin(9600);
  pinMode(FRONT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(LEFT, INPUT);
  pinMode(LW1, OUTPUT);
  pinMode(LW2, OUTPUT);
  pinMode(RW1, OUTPUT);
  pinMode(RW2, OUTPUT);
  pinMode(enaLW, OUTPUT);
  pinMode(enaRW, OUTPUT);
}

void updatePath(char c) {
  path[index] = c;
  index++;
}

// shorten robot path using pre-defined rules
void minimizePath() {
  int currPath = 0;  // current index of path array
  int currOpt = 0;   // current index of optimized array

  while (path[currPath] != 'E') {
    if (path[currPath] == 'L' && path[currPath + 1] == 'B' && path[currPath + 2] == 'R') {  // LBR --> B
      optimized[currOpt] = 'B';
    } else if (path[currPath] == 'L' && path[currPath + 1] == 'B' && path[currPath + 2] == 'S') {  // LBS --> R
      optimized[currOpt] = 'R';
    } else if (path[currPath] == 'R' && path[currPath + 1] == 'B' && path[currPath + 2] == 'L') {  // RBL --> B
      optimized[currOpt] = 'B';
    } else if (path[currPath] == 'F' && path[currPath + 1] == 'B' && path[currPath + 2] == 'L') {  // FBL --> R
      optimized[currOpt] = 'R';
    } else if (path[currPath] == 'L' && path[currPath + 1] == 'B' && path[currPath + 2] == 'R') {  // FBF --> B
      optimized[currOpt] = 'B';
    } else if (path[currPath] == 'L' && path[currPath + 1] == 'B' && path[currPath + 2] == 'R') {  // LBL --> F
      optimized[currOpt] = 'F';
    } else {
      optimized[currOpt] = path[currPath];
    }

    currPath++;
    currOpt++;
  }
  optimized[currOpt] = 'E';
}

// move left wheels forward and right wheels backwards
void turnRight() {
  Serial.write("turn right\n");

  // move forward slightly to turn closer to line
  digitalWrite(LW2, HIGH);
  digitalWrite(RW1, HIGH);
  delay(150);
  digitalWrite(LW2, LOW);
  digitalWrite(RW1, LOW);

  // turn right
  digitalWrite(LW2, HIGH);
  digitalWrite(RW2, HIGH);
  delay(125);
  digitalWrite(RW2, LOW);
  digitalWrite(LW2, LOW);

  updatePath('R');
}

// move right wheels forward and left wheels backwards
void turnLeft() {
  Serial.write("turn left\n");

  // move forward slightly to turn closer to line
  digitalWrite(LW2, HIGH);
  digitalWrite(RW1, HIGH);
  delay(150);
  digitalWrite(LW2, LOW);
  digitalWrite(RW1, LOW);

  // turn left
  while ((F = digitalRead(FRONT)) == LOW) {
    digitalWrite(LW1, HIGH);
    digitalWrite(RW1, HIGH);

    delay(25);

    digitalWrite(RW1, LOW);
    digitalWrite(LW1, LOW);

    delay(200);
  }

  findLine('L');

  updatePath('L');
}

// turn left twice to spin
void spin() {
  Serial.write("spin\n");

  digitalWrite(RW1, HIGH);
  digitalWrite(LW1, HIGH);

  delay(860);

  digitalWrite(RW1, LOW);
  digitalWrite(LW1, LOW);

  updatePath('B');
}

// turn right until line is found
void findLine(char c) {
  delay(500);
  // char b = path[index];

  // if (c == 'L' || b == 'L') {
  //   while ((F = digitalRead(FRONT)) == LOW) {
  //     digitalWrite(LW1, HIGH);
  //     digitalWrite(RW1, HIGH);

  //     delay(25);

  //     digitalWrite(RW1, LOW);
  //     digitalWrite(LW1, LOW);

  //     delay(200);
  //   }
  // } else {
  //   while ((F = digitalRead(FRONT)) == LOW) {
  //     digitalWrite(LW2, HIGH);
  //     digitalWrite(RW2, HIGH);

  //     delay(25);

  //     digitalWrite(RW2, LOW);
  //     digitalWrite(LW2, LOW);

  //     delay(200);
  //   }
  // }

  if ((L = digitalRead(LEFT)) == HIGH) {
    turnLeft();
    return;
  }

  if (path[index-1] == 'L') {
    digitalWrite(LW1, HIGH);
    digitalWrite(RW1, HIGH);

    delay(25);

    digitalWrite(RW1, LOW);
    digitalWrite(LW1, LOW);
  }

  while ((F = digitalRead(FRONT)) == LOW) {
    digitalWrite(LW2, HIGH);
    digitalWrite(RW2, HIGH);

    delay(25);

    digitalWrite(RW2, LOW);
    digitalWrite(LW2, LOW);

    delay(200);
  }
}

// move all wheels forward
void moveForward() {
  updatePath('F');

  while ((F = digitalRead(FRONT)) == HIGH) {
    digitalWrite(LW2, HIGH);
    digitalWrite(RW1, HIGH);
    delay(50);

    digitalWrite(LW2, LOW);
    digitalWrite(RW1, LOW);

    findLine('F');

    if ((L = digitalRead(LEFT)) == HIGH) {
      break;
    }
  }
}

// void moveForward() {
//   Serial.write("move forward\n");

//   digitalWrite(LW2, HIGH);
//   digitalWrite(RW1, HIGH);

//   delay(200);

//   digitalWrite(LW2, LOW);
//   digitalWrite(RW1, LOW);

//   updatePath('F');
// }

// stop all movement
void halt() {
  Serial.write("stopping\n");

  digitalWrite(LW1, LOW);
  digitalWrite(LW2, LOW);
  digitalWrite(RW2, LOW);
  digitalWrite(RW1, LOW);
}

// move slightly forward to check if end of maze
void checkEnd() {
  Serial.write("check end\n");

  digitalWrite(LW2, HIGH);
  digitalWrite(RW1, HIGH);

  delay(300);

  digitalWrite(LW2, LOW);
  digitalWrite(RW1, LOW);
}

void loop() {
  delay(5000);
  // enable motor control
  digitalWrite(enaLW, HIGH);
  digitalWrite(enaRW, HIGH);

  // read from sensors
  L = digitalRead(LEFT);
  R = digitalRead(RIGHT);
  F = digitalRead(FRONT);

  if (L == HIGH)
    turnLeft();
  else
    moveForward();

  // if (L == HIGH) {
  //   turnLeft();
  //   delay(500);
  //   F = digitalRead(FRONT);
  //   if (F == LOW)
  //     findLine();
  // }
  // else if (F == HIGH)
  //   moveForward();

  // if (L == LOW && R == LOW && F == LOW) {           // spin if at dead end (000)
  //   spin();
  //   moveForward();
  // } else if (L == LOW && R == HIGH && F == LOW) {   // turn right if only option & move forward (001)
  //   turnRight();
  //   moveForward();
  // } else if (L == LOW && R == LOW && F == HIGH) {   // move forward if only option (010)
  //   moveForward();
  // } else if (L == LOW && R == HIGH && F == HIGH) {  // move forward if can go F/R (011)
  //   moveForward();
  // } else if (L == HIGH && R == LOW && F == LOW) {   // turn left if only option & move forward (100)
  //   turnLeft();
  //   moveForward();
  // } else if (L == HIGH && R == HIGH && F == LOW) {  // turn left if can go L/R (101)
  //   turnLeft();
  //   moveForward();
  // } else if (L == HIGH && R == LOW && F == HIGH) {  // turn left if can go L/F (110)
  //   turnLeft();
  //   moveForward();
  // } else {                                          // either turn left or at end of maze (111)
  //   Serial.write("done/turn left\n");

  //   checkEnd();                                     // checkEnd to move slightly forward and check if sensors still all black
  //   L = digitalRead(LEFT);
  //   R = digitalRead(RIGHT);                         // if L/R sensor are low then there was a turn that was missed by moving forward
  //   F = digitalRead(FRONT);

  //   if (L == HIGH && R == HIGH && F == HIGH) {      // end of maze
  //     updatePath('E');
  //     minimizePath();
  //   } else {                                        // turn left at + intersection
  //     turnLeft();
  //     moveForward();
  //   }
  // }

  // // ensure robot is back on line
  // if ((F = digitalRead(FRONT)) == LOW)
  //   findLine();

  // delay(3000);

  // moveForward();
  // delay(2000);
  // turnLeft();
  // delay(2000);
  // turnRight();
  // delay(2000);
  // spin();
  // delay(5000);
}

/*
  for end of maze idea - https://www.hackster.io/Varun2905/maze-solving-robot-3-ir-sensors-9ada3b
  path minimization - https://patrickmccabemakes.com/tutorials/Maze_Solving/
*/
