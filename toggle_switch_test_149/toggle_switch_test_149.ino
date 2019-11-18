//states
int left_state = 0; //variable for left toggle
int left_state_prev = 0;
int right_state = 0; //variable for right toggle
int right_state_prev = 0;
//pins
const int LEFT_SWITCH = 19;
const int RIGHT_SWITCH = 22;
const int LEFT_LED = 13;
const int RIGHT_LED = 12;
//timing/blinking stuff
const long interval = 1000; //blink interval in milliseconds
unsigned long prev_time = 0;
int left_led_state = LOW;
int right_led_state = LOW;

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

  unsigned long curr_time = millis();

  //left toggling
  if(digitalRead(LEFT_SWITCH) == HIGH) {
    left_state = 1 - left_state;
    Serial.print("Left state: ");
    Serial.println(left_state);
    if (right_state == 1) {
      right_state = 0;
      right_led_state = LOW;
      digitalWrite(RIGHT_LED, LOW);
    }
  }
  //toggle on
  if(left_state == 1 && left_state_prev == 0) {
    //digitalWrite(LEFT_LED, HIGH);
    //blink
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
    if (left_state == 1) {
      left_state = 0;
      left_led_state = LOW;
      digitalWrite(LEFT_LED, LOW);
    }
  }
  //toggle on
  if(right_state == 1 && right_state_prev == 0) {
    //digitalWrite(RIGHT_LED, HIGH);
    //blink
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

  blink(LEFT_LED, left_state, &left_led_state, curr_time);
  blink(RIGHT_LED, right_state, &right_led_state, curr_time);
}

void blink(int LED, int state, int* led_state, unsigned long curr_time) {
  if (state == 1) {
    if(curr_time - prev_time >= interval) {
      prev_time = curr_time;
      if (*led_state == HIGH) {
        *led_state = LOW;
        digitalWrite(LED, LOW);
      } else {
        *led_state = HIGH;
        digitalWrite(LED, HIGH);
      }
    }
  }
}
