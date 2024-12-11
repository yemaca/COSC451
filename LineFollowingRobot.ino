// black high white low
#define LW1 2
#define LW2 3  // wheel positions
#define RW1 4  // LW2 and RW1 high to turn forward
#define RW2 5

#define enaLW 9  // enable motor control for wheels
#define enaRW 10

#define RIGHT 6
#define FRONT 7  // sensor positions
#define LEFT 8

int R = 0;
int F = 0;  // variables to store sensor readings
int L = 0;
int last = 0;
int count = 0;

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
    } else if (path[currPath] == 'L' && path[currPath + 1] == 'B' && path[currPath + 2] == 'F') {  // LBF --> R
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
  digitalWrite(LW2, HIGH);
  digitalWrite(RW1, HIGH);

  delay(100);

  digitalWrite(LW2, LOW);
  digitalWrite(RW1, LOW);
}

// short turns to find line based on last movement
void findLine() {
  delay(500);
  
  // if turned right
  if (last == 0){
    while (digitalRead(FRONT) == LOW || digitalRead(LEFT) == HIGH || digitalRead(RIGHT) == HIGH) {
      digitalWrite(LW2, HIGH);
      digitalWrite(RW2, HIGH);
      delay(50);
      digitalWrite(LW2, LOW);
      digitalWrite(RW2, LOW);

      delay(200);
    }
    
    for (int i = 0; i < 1; i++) {
      digitalWrite(LW2, HIGH);
      digitalWrite(RW2, HIGH);
      delay(50);
      digitalWrite(LW2, LOW);
      digitalWrite(RW2, LOW);
    }
  }

  // if turned left
  else {
    while (digitalRead(FRONT) == LOW || digitalRead(LEFT) == HIGH || digitalRead(RIGHT) == HIGH) {
      digitalWrite(LW1, HIGH);
      digitalWrite(RW1, HIGH);
      delay(50);
      digitalWrite(LW1, LOW);
      digitalWrite(RW1, LOW);

      delay(200);
    }
  }
}

void moveForward() {
  while (digitalRead(FRONT) == HIGH && digitalRead(LEFT) == LOW) {
    count++;
    digitalWrite(RW1, HIGH);
    digitalWrite(LW2, HIGH);
    delay(25);
    digitalWrite(RW1, LOW);
    digitalWrite(LW2, LOW);

    delay(50);

    if (count > 20) {
      last = 0; 
      count = 0;
    }
  }

  if (digitalRead(FRONT) == LOW && digitalRead(LEFT) == LOW && digitalRead(RIGHT) == LOW)
    findLine();

  count = 0;  
  updatePath('F');
}

void turnLeft() {
  last = 1;
  digitalWrite(LW2, HIGH);
  digitalWrite(RW1, HIGH);
  delay(50);
  digitalWrite(LW2, LOW);
  digitalWrite(RW1, LOW);

  while (digitalRead(FRONT) == LOW) {
    digitalWrite(LW1, HIGH);
    digitalWrite(RW1, HIGH);

    delay(25);

    digitalWrite(RW1, LOW);
    digitalWrite(LW1, LOW);

    delay(200);
  }

  for (int i = 0; i < 2; i++) {
    digitalWrite(LW1, HIGH);
    digitalWrite(RW1, HIGH);

    delay(25);

    digitalWrite(RW1, LOW);
    digitalWrite(LW1, LOW);
  }

  updatePath('L');
}

void turnRight() {
  last = 0;
  digitalWrite(LW2, HIGH);
  digitalWrite(RW1, HIGH);
  delay(50);
  digitalWrite(LW2, LOW);
  digitalWrite(RW1, LOW);

  // turn right
  while (digitalRead(FRONT) == LOW) {
    digitalWrite(LW2, HIGH);
    digitalWrite(RW2, HIGH);

    delay(25);

    digitalWrite(RW2, LOW);
    digitalWrite(LW2, LOW);

    delay(200);
  }

  // turn slightly right two more times to get further on line
  for (int i = 0; i < 2; i++) {
    digitalWrite(LW2, HIGH);
    digitalWrite(RW2, HIGH);

    delay(25);

    digitalWrite(RW2, LOW);
    digitalWrite(LW2, LOW);
  }

  updatePath('R');
}

void loop() {
  delay(1000);
  // enable motor control
  digitalWrite(enaLW, HIGH);
  digitalWrite(enaRW, HIGH);

  // read from sensors
  L = digitalRead(LEFT);
  R = digitalRead(RIGHT);
  F = digitalRead(FRONT);

  if (L == HIGH)
    turnLeft();
  else if (F == HIGH)
    moveForward();
  else
    turnRight();

  if (L == LOW && R == LOW && F == LOW) {           // spin if at dead end (000)
    findLine();
  } else if (L == LOW && R == HIGH && F == LOW) {   // turn right if only option & move forward (001)
    turnRight();
  } else if (L == LOW && R == LOW && F == HIGH) {   // move forward if only option (010)
    moveForward();
  } else if (L == LOW && R == HIGH && F == HIGH) {  // move forward if can go F/R (011)
    moveForward();
  } else if (L == HIGH && R == LOW && F == LOW) {   // turn left if only option & move forward (100)
    turnLeft();
  } else if (L == HIGH && R == HIGH && F == LOW) {  // turn left if can go L/R (101)
    turnLeft();
  } else if (L == HIGH && R == LOW && F == HIGH) {  // turn left if can go L/F (110)
    turnLeft();
  } else {                                          // either turn left or at end of maze (111)

    checkEnd();                                     // checkEnd to move slightly forward and check if sensors still all black
    L = digitalRead(LEFT);
    R = digitalRead(RIGHT);                         // if L/R sensor are low then there was a turn that was missed by moving forward
    F = digitalRead(FRONT);

    if (L == HIGH && R == HIGH && F == HIGH) {      // end of maze
      updatePath('E');
      minimizePath();
    } else {                                        // turn left at + intersection
      turnLeft();
    }
  }
}

/*
// black high white low
#define LW1 2
#define LW2 3  // wheel positions
#define RW1 4  // LW2 and RW1 high to turn forward
#define RW2 5

#define enaLW 9  // enable motor control for wheels
#define enaRW 10

#define RIGHT 6
#define FRONT 7  // sensor positions
#define LEFT 8

int R = 0;
int F = 0;  // variables to store sensor readings
int L = 0;
int last = 0;
int count = 0;

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
    } else if (path[currPath] == 'L' && path[currPath + 1] == 'B' && path[currPath + 2] == 'F') {  // LBF --> R
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
  digitalWrite(LW2, HIGH);
  digitalWrite(RW1, HIGH);

  delay(100);

  digitalWrite(LW2, LOW);
  digitalWrite(RW1, LOW);
}

// short turns to find line based on last movement
void findLine() {
  delay(500);
  
  // if turned right
  // if (last == 0){
    while (digitalRead(FRONT) == LOW || digitalRead(LEFT) == HIGH || digitalRead(RIGHT) == HIGH) {
      digitalWrite(LW2, HIGH);
      digitalWrite(RW2, HIGH);
      delay(50);
      digitalWrite(LW2, LOW);
      digitalWrite(RW2, LOW);

      delay(200);
    }
    
    // for (int i = 0; i < 1; i++) {
    //   digitalWrite(LW2, HIGH);
    //   digitalWrite(RW2, HIGH);
    //   delay(50);
    //   digitalWrite(LW2, LOW);
    //   digitalWrite(RW2, LOW);
    // }
  // }

  // if turned left
  // else {
  //   while (digitalRead(FRONT) == LOW || digitalRead(LEFT) == HIGH || digitalRead(RIGHT) == HIGH) {
  //     digitalWrite(LW1, HIGH);
  //     digitalWrite(RW1, HIGH);
  //     delay(50);
  //     digitalWrite(LW1, LOW);
  //     digitalWrite(RW1, LOW);

  //     delay(200);
  //   }
  // }
}

void moveForward() {
  while (digitalRead(FRONT) == HIGH && digitalRead(LEFT) == LOW) {
    count++;
    digitalWrite(RW1, HIGH);
    digitalWrite(LW2, HIGH);
    delay(25);
    digitalWrite(RW1, LOW);
    digitalWrite(LW2, LOW);

    delay(50);

    if (count > 20) {
      last = 0; 
      count = 0;
    }
  }

  if (digitalRead(FRONT) == LOW && digitalRead(LEFT) == LOW && digitalRead(RIGHT) == LOW)
    findLine();

  count = 0;  
  updatePath('F');
}

void turnLeft() {
  last = 1;
  digitalWrite(LW2, HIGH);
  digitalWrite(RW1, HIGH);
  delay(100);
  digitalWrite(LW2, LOW);
  digitalWrite(RW1, LOW);

  while (digitalRead(FRONT) == LOW) {
    digitalWrite(LW1, HIGH);
    digitalWrite(RW1, HIGH);

    delay(25);

    digitalWrite(RW1, LOW);
    digitalWrite(LW1, LOW);

    delay(200);
  }

  for (int i = 0; i < 2; i++) {
    digitalWrite(LW1, HIGH);
    digitalWrite(RW1, HIGH);

    delay(25);

    digitalWrite(RW1, LOW);
    digitalWrite(LW1, LOW);
  }

  updatePath('L');
}

void turnRight() {
  last = 0;
  digitalWrite(LW2, HIGH);
  digitalWrite(RW1, HIGH);
  delay(50);
  digitalWrite(LW2, LOW);
  digitalWrite(RW1, LOW);

  // turn right
  while (digitalRead(FRONT) == LOW) {
    digitalWrite(LW2, HIGH);
    digitalWrite(RW2, HIGH);

    delay(25);

    digitalWrite(RW2, LOW);
    digitalWrite(LW2, LOW);

    delay(200);
  }

  // turn slightly right two more times to get further on line
  for (int i = 0; i < 2; i++) {
    digitalWrite(LW2, HIGH);
    digitalWrite(RW2, HIGH);

    delay(25);

    digitalWrite(RW2, LOW);
    digitalWrite(LW2, LOW);
  }

  updatePath('R');
}

void loop() {
  delay(1000);
  // enable motor control
  digitalWrite(enaLW, HIGH);
  digitalWrite(enaRW, HIGH);

  // read from sensors
  L = digitalRead(LEFT);
  R = digitalRead(RIGHT);
  F = digitalRead(FRONT);

  if (L == HIGH)
    turnLeft();
  else if (F == HIGH)
    moveForward();
  else
    turnRight();

  if (L == LOW && R == LOW && F == LOW) {           // spin if at dead end (000)
    findLine();
  } else if (L == LOW && R == HIGH && F == LOW) {   // turn right if only option & move forward (001)
    turnRight();
  } else if (L == LOW && R == LOW && F == HIGH) {   // move forward if only option (010)
    moveForward();
  } else if (L == LOW && R == HIGH && F == HIGH) {  // move forward if can go F/R (011)
    moveForward();
  } else if (L == HIGH && R == LOW && F == LOW) {   // turn left if only option & move forward (100)
    turnLeft();
  } else if (L == HIGH && R == HIGH && F == LOW) {  // turn left if can go L/R (101)
    turnLeft();
  } else if (L == HIGH && R == LOW && F == HIGH) {  // turn left if can go L/F (110)
    turnLeft();
  } else {                                          // either turn left or at end of maze (111)

    checkEnd();                                     // checkEnd to move slightly forward and check if sensors still all black
    L = digitalRead(LEFT);
    R = digitalRead(RIGHT);                         // if L/R sensor are low then there was a turn that was missed by moving forward
    F = digitalRead(FRONT);

    if (L == HIGH && R == HIGH && F == HIGH) {      // end of maze
      updatePath('E');
      minimizePath();
    } else {                                        // turn left at + intersection
      turnLeft();
    }
  }
}

/*
  for end of maze idea - https://www.hackster.io/Varun2905/maze-solving-robot-3-ir-sensors-9ada3b
  path minimization - P
*/
