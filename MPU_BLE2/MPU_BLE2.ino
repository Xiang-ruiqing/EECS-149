#include <MPU9250_asukiaaa.h>

#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21
#define SCL_PIN 22
#endif

//switch pins
const int LEFT_SWITCH = 19;
const int RIGHT_SWITCH = 18;
//const int LEFT_LED = 13;
//const int RIGHT_LED = 12;

const float LEFT_THRESH = 30.0;
const float RIGHT_THRESH = -30.0;

typedef enum {
  OFF,
  LEFT,
  RIGHT,
  PRE_LEFT,
  PRE_RIGHT,
} turn_state_t;

typedef enum {
  OFF,
  ON,
} brake_state_t;

brake_state_t brake_state = OFF;
turn_state_t turn_state = OFF;
MPU9250_asukiaaa mySensor;
float pitch, roll,yaw;
void setup() {
  while(!Serial);
   
  pinMode(LEFT_SWITCH, INPUT);
  pinMode(RIGHT_SWITCH, INPUT);
  Serial.begin(115200);
  Serial.println("started");
  
  #ifdef _ESP32_HAL_I2C_H_
  // for esp32
  Wire.begin(SDA_PIN, SCL_PIN); //sda, scl
  #else
  Wire.begin();
  #endif
  
  mySensor.setWire(&Wire);
  
  mySensor.beginAccel();
  mySensor.beginMag();
  mySensor.beginGyro();

}

void loop() {
  mySensor.accelUpdate();
  mySensor.magUpdate();
  getAngle(mySensor.accelX(),mySensor.accelY(),mySensor.accelZ(),mySensor.magX(),mySensor.magY(),mySensor.magZ());
  Serial.print("Pitch:\t"); 
  Serial.println(pitch, 6);
  Serial.print("Roll:\t"); 
  Serial.println(roll, 6);
  Serial.print("Yaw:\t"); 
  Serial.println(yaw, 6);
  bool just_left = false;
  bool just_right = false;

  switch(turn_state){
    case OFF: {
      Serial.println("OFF"); 
      if (digitalRead(LEFT_SWITCH) == HIGH){
        delay(500);
        if (!just_left){
          turn_state = PRE_LEFT;
        }
        just_left = true;
        just_right = false;
      }else if(digitalRead(RIGHT_SWITCH) == HIGH){
        delay(500);
        if (!just_right){
          turn_state = PRE_RIGHT;
        }
        just_right = true;
        just_left = false;
      } else {
        just_right = false;
        just_left = false;
      }    
      break;
      
    }
    case PRE_LEFT: {
      Serial.println("PRE_LEFT"); 
      if (digitalRead(LEFT_SWITCH) == HIGH){
        delay(500);
        if (!just_left){
          turn_state = OFF;
        }
        just_left = true;
        just_right = false;
        
      }else if(digitalRead(RIGHT_SWITCH) == HIGH){
        delay(500);
        if (!just_right){
          turn_state = PRE_RIGHT;
        }
        just_left = false;
        just_right = true;
      } else if (roll >= LEFT_THRESH) {
        //placeholder
        just_left = false;
        just_right = false;
        turn_state = LEFT;
      } else {
        just_left = false;
        just_right = false;
      }
      break;
    }
    case PRE_RIGHT: {
      Serial.println("PRE_RIGHT"); 
      if (digitalRead(LEFT_SWITCH) == HIGH){
        delay(500);
        if (!just_left){
          turn_state = PRE_LEFT;
        }
        just_left = true;
        just_right = false;
      }else if(digitalRead(RIGHT_SWITCH) == HIGH){
        delay(500);
        if (!just_right){
          turn_state = OFF;
        }
        just_left = false;
        just_right = true;
      } else if (roll <= RIGHT_THRESH) {
        //placeholder
        just_left = false;
        just_right = false;
        turn_state = RIGHT;
      } else {
        just_left = false;
        just_right = false;
      }
      break;
    }
    case LEFT: {
      Serial.println("LEFT"); 
      if (roll < LEFT_THRESH / 2) {
        turn_state = OFF;
      }
      break;
    }
    case RIGHT: {
      Serial.println("RIGHT"); 
      if (roll > RIGHT_THRESH / 2) {
        turn_state = OFF;
      }
      break;
    }
    
  };

  switch (brake_state){
    case OFF: {
      if(){ //accel < 0 || vel == approx 0
        brake_state = ON;
      }
      break;
    }
    case ON: {
      if(){ //accel > 0
        brake_state = OFF;
      }
      break;
    }
  };

 
  delay(200);
}

void getAngle(float Vx, float Vy, float Vz, float magX, float magY, float magZ) {
  float x = Vx;
  float y = Vy;
  float z = Vz;
  pitch = atan(x / sqrt((y * y) + (z * z)));
  roll = atan(y / sqrt((x * x) + (z * z)));

  float Yh = (magY * cos(roll)) - (magZ * sin(roll));
  float Xh = (magX * cos(pitch))+(magY * sin(roll)*sin(pitch)) + (magZ * cos(roll) * sin(pitch));

   yaw =  atan2(-Yh, Xh);

  pitch = pitch * (180.0 / 3.14);
  roll = roll * (180.0 / 3.14) ;
  yaw = yaw * (180.0/3.14);
}

void getVelAccel() {
  
}
