#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <MPU9250_asukiaaa.h>
#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21
#define SCL_PIN 22
#endif

//switch pins
const int LEFT_SWITCH = 34;
const int RIGHT_SWITCH = 35;
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
  OFF_b,
  ON,
} brake_state_t;

brake_state_t brake_state = OFF_b;
turn_state_t turn_state = OFF;


MPU9250_asukiaaa mySensor;
float pitch, roll,yaw;
float acc_x;

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 48;
char buf[4];

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};


void setup() {
  while(!Serial);
   
  //pinMode(LEFT_SWITCH, INPUT);
  //pinMode(RIGHT_SWITCH, INPUT);
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
   // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");

}

bool just_left = false;
bool just_right = false;
char lights[3];

void loop() {
  //Serial.println(analogRead(RIGHT_SWITCH));
  //delay(50);
  mySensor.accelUpdate();
  mySensor.magUpdate();
  acc_x = mySensor.accelX();
  char* acc_string = gcvt(acc_x,4,buf);
  getAngle(mySensor.accelX(),mySensor.accelY(),mySensor.accelZ(),mySensor.magX(),mySensor.magY(),mySensor.magZ());
  Serial.print("Pitch:\t"); 
  Serial.println(pitch, 6);
  Serial.print("Roll:\t"); 
  Serial.println(roll, 6);
  Serial.print("Yaw:\t"); 
  Serial.println(yaw, 6);

  switch(turn_state){
    case OFF: {
      Serial.println("OFF");
      lights[0] = '0';
      lights[2] = '0';
      if (analogRead(LEFT_SWITCH) == 4095){
        delay(500);
        if (!just_left){
          turn_state = PRE_LEFT;
        }
        just_left = true;
        just_right = false;
      }else if(analogRead(RIGHT_SWITCH) == 4095){
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
      lights[0] = '1';
      lights[2] = '0';
      if (analogRead(LEFT_SWITCH) == 4095){
        delay(500);
        if (!just_left){
          turn_state = OFF;
        }
        just_left = true;
        just_right = false;
        
      }else if(analogRead(RIGHT_SWITCH) == 4095){
        delay(500);
        if (!just_right){
          turn_state = PRE_RIGHT;
        }
        just_left = false;
        just_right = true;
      } else if (roll >= LEFT_THRESH) {
        //placeholder
        turn_state = LEFT;
        just_left = false;
        just_right = false;
      } else {
        just_left = false;
        just_right = false;
      }
      break;
    }
    case PRE_RIGHT: {
      Serial.println("PRE_RIGHT");
      lights[0] = '0';
      lights[2] = '1';
      if (analogRead(LEFT_SWITCH) == 4095){
        delay(500);
        if (!just_left){
          turn_state = PRE_LEFT;
        }
        just_left = true;
        just_right = false;
      }else if(analogRead(RIGHT_SWITCH) == 4095){
        delay(500);
        if (!just_right){
          turn_state = OFF;
        }
        just_left = false;
        just_right = true;
      } else if (roll <= RIGHT_THRESH) {
        //placeholder
        turn_state = RIGHT;
        just_left = false;
        just_right = false;
      } else {
        just_left = false;
        just_right = false;
      }
      break;
    }
    case LEFT: {
      Serial.println("LEFT");
      lights[0] = '1';
      lights[2] = '0'; 
      if (roll < LEFT_THRESH / 2) {
        turn_state = OFF;
      }
      just_left = false;
      just_right = false;
      break;
    }
    case RIGHT: {
      Serial.println("RIGHT");
      lights[0] = '0';
      lights[2] = '1';
      if (roll > RIGHT_THRESH / 2) {
        turn_state = OFF;
      }
      just_left = false;
      just_right = false;
      break;
    }
    
  };

  switch (brake_state){
    case OFF_b: {
      lights[1] = '0';
      if(acc_x <= 0){ // || vel == approx 0 (really need velocity?)
        brake_state = ON;
      }
      break;
    }
    case ON: {
      lights[1] = '1';
      if(acc_x > 0){
        brake_state = OFF_b;
      }
      break;
    }
  };
  if (deviceConnected) {
    pCharacteristic->setValue(lights);
    pCharacteristic->notify();
    delay(30); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
  }
 

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

