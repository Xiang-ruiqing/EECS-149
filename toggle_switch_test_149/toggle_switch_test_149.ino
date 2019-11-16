int left_state = 0; //variable for left toggle
int left_state_prev = 0;
int right_state = 0; //v-ariable for right toggle
int right_state_prev = 0;
int LEFT_SWITCH = 19;
int RIGHT_SWITCH = 22;
int LEFT_LED = 13;
int RIGHT_LED = 12;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LEFT_LED, OUTPUT);
  pinMode(RIGHT_LED, OUTPUT);
  pinMode(LEFT_SWITCH, INPUT);
  pinMode(RIGHT_SWITCH, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  //left toggling
  if(digitalRead(LEFT_SWITCH) == HIGH) {
    left_state = 1 - left_state;
    Serial.print("Left state: ");
    Serial.println(left_state);
  }
  //toggle on
  if(left_state == 1 && left_state_prev == 0) {
    digitalWrite(LEFT_LED, HIGH);
    delay(500);
    Serial.println("left on");
  }
  //toggle off
  if (left_state == 0 && left_state_prev == 1) {
    digitalWrite(LEFT_LED, LOW);
    delay(500);
    Serial.println("left off");
  }
  left_state_prev = left_state;

  //right toggling
  if(digitalRead(RIGHT_SWITCH) == HIGH) {
    right_state = 1 - right_state;
    Serial.print("Right state: ");
    Serial.println(right_state);
  }
  //toggle on
  if(right_state == 1 && right_state_prev == 0) {
    digitalWrite(RIGHT_LED, HIGH);
    delay(500);
    Serial.println("right on");
  }
  //toggle off
  if (right_state == 0 && right_state_prev == 1) {
    digitalWrite(RIGHT_LED, LOW);
    delay(500);
    Serial.println("right off");
  }
  right_state_prev = right_state;
}
