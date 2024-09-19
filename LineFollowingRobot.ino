//black high white low
#define RIGHT 2
#define FRONT 3
#define LEFT 4

int R = 0;
int L = 0;
int F = 0;

void setup() {
  Serial.begin(9600);
  pinMode(FRONT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(LEFT, INPUT);
}

void turnRight() {
  Serial.write("turn right\n");
}

void turnLeft() {
  Serial.write("turn left\n");
}

void spin() {
  Serial.write("spin\n");
}

void moveForward() {
  Serial.write("move forward\n");
}

void loop() {
  L = digitalRead(LEFT);
  R = digitalRead(RIGHT);
  F = digitalRead(FRONT);

  if (L == LOW && R == LOW && F == LOW) {
    spin();
    moveForward();
  } else if (L == LOW && R == HIGH && F == LOW) {
    turnRight();
    moveForward();
  } else if (L == LOW && R == LOW && F == HIGH) {
    moveForward();
  } else if (L == LOW && R == HIGH && F == HIGH) {
    moveForward();
  } else if (L == HIGH && R == LOW && F == LOW) {
    turnLeft();
    moveForward();
  } else if (L == HIGH && R == HIGH && F == LOW) {
    turnLeft();
    moveForward();
  } else if (L == HIGH && R == LOW && F == HIGH) {
    turnLeft();
    moveForward();
  } else {
    Serial.write("done/turn left\n");
  }

  delay(2000);
}
