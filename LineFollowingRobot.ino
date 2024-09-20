//black high white low
#define RIGHT 2
#define FRONT 3 //sensor positions
#define LEFT 4
#define LF 5
#define LB 6    //motor positions
#define RF 7    //(left front, left back, etc)
#define RB 8

int R = 0;
int L = 0;      //variables to store sensor readings
int F = 0;

void setup() {
  Serial.begin(9600);
  pinMode(FRONT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(LEFT, INPUT);
  pinMode(LF, OUTPUT);
  pinMode(LB, OUTPUT);
  pinMode(RF, OUTPUT);
  pinMode(RB, OUTPUT);
}

/*
  in moving functions, will need to slow motors down when done with turning/moving
  could ramp up/down if using analog instead of digital (https://docs.arduino.cc/learn/electronics/transistor-motor-control/)
*/

//move left wheels forward and right wheels backwards
void turnRight() {
  Serial.write("turn right\n");
  
  digitalWrite(LF, HIGH);
  digitalWrite(LB, HIGH);
  digitalWrite(RF, LOW);
  digitalWrite(RB, LOW);

  // for (int x = 1; x <= 255; x++) {
  //   digitalWrite(LF, x);
  //   digitalWrite(LB, x);
  //   digitalWrite(RF, -x);
  //   digitalWrite(RB, -x);
  //   delay(50);
  // }

  // for (int x = 255; x <= 0; x--) {
  //   digitalWrite(LF, x);
  //   digitalWrite(LB, x);
  //   digitalWrite(RF, -x);
  //   digitalWrite(RB, -x);
  //   delay(50);
  // }
}

//move right wheels forward and left wheels backwards
void turnLeft() {
  Serial.write("turn left\n");

  digitalWrite(LF, LOW);
  digitalWrite(LB, LOW);
  digitalWrite(RF, HIGH);
  digitalWrite(RB, HIGH);

  // for (int x = 1; x <= 255; x++) {
  //   digitalWrite(LF, -x);
  //   digitalWrite(LB, -x);
  //   digitalWrite(RF, x);
  //   digitalWrite(RB, x);
  //   delay(50);
  // }

  // for (int x = 255; x <= 0; x--) {
  //   digitalWrite(LF, -x);
  //   digitalWrite(LB, -x);
  //   digitalWrite(RF, x);
  //   digitalWrite(RB, x);
  //   delay(50);
  // }
}

//turn left twice to spin
void spin() {
  Serial.write("spin\n");
  
  turnLeft();
  turnLeft();
}

//move all wheels forward
void moveForward() {
  Serial.write("move forward\n");
  
  digitalWrite(LF, HIGH);
  digitalWrite(LB, HIGH);
  digitalWrite(RF, HIGH);
  digitalWrite(RB, HIGH);

  // for (int x = 1; x <= 255; x++) {
  //   digitalWrite(LF, x);
  //   digitalWrite(LB, x);
  //   digitalWrite(RF, x);
  //   digitalWrite(RB, x);
  //   delay(50);
  // }

  // for (int x = 255; x <= 0; x--) {
  //   digitalWrite(LF, x);
  //   digitalWrite(LB, x);
  //   digitalWrite(RF, x);
  //   digitalWrite(RB, x);
  //   delay(50);
  // }
}

void loop() {
  //read from sensors
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

  delay(2000);
}
