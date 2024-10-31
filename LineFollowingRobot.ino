//black high white low
#define LW1 2
#define LW2 3     //wheel positions
#define RW1 4     //LW2 and RW1 high to turn forward
#define RW2 5

#define enaLW 11  //enable motor control for wheels
#define enaRW 12

#define RIGHT 6
#define FRONT 7   //sensor positions
#define LEFT 8

int R = 0;
int F = 0;        //variables to store sensor readings
int L = 0;

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

// turn right until line is found
void findLine() {
  while ((F = digitalRead(FRONT)) == LOW) {
    digitalWrite(LW2, HIGH);
    digitalWrite(RW2, HIGH);

    delay(100);

    digitalWrite(RW2, LOW);
    digitalWrite(LW2, LOW);
  }
}

//move left wheels forward and right wheels backwards
void turnRight() {
  Serial.write("turn right\n");
  
  digitalWrite(LW2, HIGH);
  digitalWrite(RW2, HIGH);

  delay(430);

  digitalWrite(RW2, LOW);
  digitalWrite(LW2, LOW);
}

// //move right wheels forward and left wheels backwards
void turnLeft() {
  Serial.write("turn left\n");

  digitalWrite(RW1, HIGH);
  digitalWrite(LW1, HIGH);

  delay(430);

  digitalWrite(RW1, LOW);
  digitalWrite(LW1, LOW);
}

// //turn left twice to spin
void spin() {
  Serial.write("spin\n");
  
  digitalWrite(RW1, HIGH);
  digitalWrite(LW1, HIGH);
  
  delay(860);

  digitalWrite(RW1, LOW);
  digitalWrite(LW1, LOW);
}

// //move all wheels forward
void moveForward() {
  Serial.write("move forward\n");
  
  digitalWrite(LW2, HIGH);
  digitalWrite(RW1, HIGH);

  delay(3000);

  digitalWrite(LW2, LOW);
  digitalWrite(RW1, LOW);
}

void halt() {
  Serial.write("stopping\n");

  digitalWrite(LW1, LOW);
  digitalWrite(LW2, LOW);
  digitalWrite(RW2, LOW);
  digitalWrite(RW1, LOW);
}

void loop() {
  //read from sensors
  digitalWrite(enaLW, HIGH);
  digitalWrite(enaRW, HIGH);
  
  L = digitalRead(LEFT);
  R = digitalRead(RIGHT);
  F = digitalRead(FRONT);

  if (L == LOW && R == LOW && F == LOW) {             //spin if at dead end (000)
    spin();
    moveForward();
  } else if (L == LOW && R == HIGH && F == LOW) {     //turn right if only option & move forward (001)
    turnRight();
    moveForward();
  } else if (L == LOW && R == LOW && F == HIGH) {     //move forward if only option (010)
    moveForward();
  } else if (L == LOW && R == HIGH && F == HIGH) {    //move forward if can go F/R (011)
    moveForward();
  } else if (L == HIGH && R == LOW && F == LOW) {     //turn left if only option & move forward (100)
    turnLeft();
    moveForward();
  } else if (L == HIGH && R == HIGH && F == LOW) {    //turn left if can go L/R (101)
    turnLeft();
    moveForward();
  } else if (L == HIGH && R == LOW && F == HIGH) {    //turn left if can go L/F (110)
    turnLeft();
    moveForward();
  } else {                                            //either turn left or at end of maze (111)
    Serial.write("done/turn left\n");
  }

  delay(3000);
}
