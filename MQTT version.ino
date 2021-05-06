#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "MPU9250.h"

MPU9250 mpu;

const char* ssid = "SSID";
const char* password =  "password";
const char* mqttServer = "192.168.100.6";
const int mqttPort = 1883;
const char* mqttUser = "user";
const char* mqttPassword = "password";
 
WiFiClient espClient;
PubSubClient client(espClient);

void selectFilter(QuatFilterSel MADGWICK){}
void setup() {
 
  Serial.begin(115200);
  Serial.println();
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }

  //MPU
    Wire.begin();
    delay(2000);

     MPU9250Setting setting;
    setting.accel_fs_sel = ACCEL_FS_SEL::A16G;
    setting.gyro_fs_sel = GYRO_FS_SEL::G2000DPS;
    setting.mag_output_bits = MAG_OUTPUT_BITS::M16BITS;
    setting.fifo_sample_rate = FIFO_SAMPLE_RATE::SMPL_200HZ;
    setting.gyro_fchoice = 0x03;
    setting.gyro_dlpf_cfg = GYRO_DLPF_CFG::DLPF_250HZ;
    setting.accel_fchoice = 0x01;
    setting.accel_dlpf_cfg = ACCEL_DLPF_CFG::DLPF_218HZ_0;

    if (!mpu.setup(0x68)) {  // change to your own address
        while (1) {
            Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
            delay(5000);
        }
    }
 
}
 
void loop() {

  mpu.update();
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
 
  JSONencoder["device"] = "LilyGO";
  JSONencoder["sensorType"] = "Quaternion";
  JsonArray& values = JSONencoder.createNestedArray("values");

  
 
  values.add(mpu.getQuaternionX());
  values.add(mpu.getQuaternionY());
  values.add(mpu.getQuaternionZ());
  values.add(mpu.getQuaternionW());
 

  
  
  char JSONmessageBuffer[100];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println("Sending message to MQTT topic..");
  Serial.println(JSONmessageBuffer);
 
  if (client.publish("esp/test", JSONmessageBuffer) == true) {
    Serial.println("Success sending message");
  } else {
    Serial.println("Error sending message");
  }
 
  client.loop();
  Serial.println("-------------");
 
  delay(4);
 
}
