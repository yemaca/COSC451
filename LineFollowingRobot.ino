//black high white low
#define RIGHT 2
#define FRONT 3 //sensor positions
#define LEFT 4
#define RW 5
#define LW 6    //wheel positions

int R = 0;
int L = 0;      //variables to store sensor readings
int F = 0;

void setup() {
  Serial.begin(9600);
  pinMode(FRONT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(LEFT, INPUT);
  pinMode(RW, OUTPUT);
  pinMode(LW, OUTPUT);
}

/*
  in moving functions, will need to slow motors down when done with turning/moving
  could ramp up/down if using analog instead of digital (https://docs.arduino.cc/learn/electronics/transistor-motor-control/)
  https://forum.arduino.cc/t/line-follower-robot-not-moving-forward/582416/5
  https://circuitdigest.com/microcontroller-projects/arduino-uno-line-follower-robot
*/

/*
  for moving functions, need to add a delay between increasing & decreasing speeds so robot actually moves
*/

//move left wheels forward and right wheels backwards
void turnRight() {
  Serial.write("turn right\n");
  
  digitalWrite(LW, HIGH);
  digitalWrite(RW, LOW);

  // for (int x = 1; x <= 255; x++) {
  //   digitalWrite(LW, x);
  //   digitalWrite(RW, -x);
  //   delay(50);
  // }

  // for (int x = 255; x <= 0; x--) {
  //   digitalWrite(LW, x);
  //   digitalWrite(RW, -x);
  //   delay(50);
  // }
}

//move right wheels forward and left wheels backwards
void turnLeft() {
  Serial.write("turn left\n");

  digitalWrite(LW, LOW);
  digitalWrite(RW, HIGH);
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
  
  digitalWrite(LW, HIGH);
  digitalWrite(RW, HIGH);
}

void halt() {
  Serial.write("stopping\n");

  digitalWrite(LW, 0);
  digitalWrite(RW, 0);
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
