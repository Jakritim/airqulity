#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include "PMS.h"

PMS pms(Serial);
PMS::DATA data;

const char* username = ".................";
const char* Topic_PM2_5 = "BIOPM25"; // noty
const char* Topic_PM10 = "BIOPM10";  // note
const unsigned int writeInterval = 30000; // Delay
//const char* mqtt_server = "192.168.0.150";
const char* mqtt_server = "broker.hivemq.com";
//const char* mqtt_user = "service_mqtt";
//const char* mqtt_passwd ="admin@1234";
//unsigned int mqtt_port = 1883;
byte mac[] = { 0x2C, 0xF7, 0xF1, 0x08, 0x37, 0x07 }; // note 

IPAddress ip(192, 168, 0, 12); // TODO: Add the IP address 192.168.60.254,192.168.60.113 note
EthernetClient askClient;
PubSubClient client(askClient);
void setup() {
  Serial.begin(9600);
  Serial.println("*****************************************************");
  Serial.println("********** Program Start : Arduino Ethernet publishes data to AskSensors over MQTT");
  Serial.print("********** connecting to00000000 Ethernet : ");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }
  delay(1000);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected())
    reconnect();
  client.loop();
  if (pms.read(data)) {

    Serial.print("PM 2.5 (ug/m3): ");
    Serial.println(data.PM_AE_UG_2_5);

    Serial.print("PM 10.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_10_0);
    Serial.println();
    
    client.publish(Topic_PM2_5, String(data.PM_AE_UG_2_5).c_str());
    client.publish(Topic_PM10, String(data.PM_AE_UG_10_0).c_str());

    delay(writeInterval);// delay
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("********** Attempting MQTT connection...");
    String clientId = "ethClient-";      // ถ้าใช้ arduino ต้องเปลี่ยน
    //if (client.connect(clientId.c_str(),mqtt_user, mqtt_passwd)) {       //if (client.connect("ethClient", username, ""))
    if (client.connect("ethClient", username, "")) {    
      Serial.println("-> MQTT client connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("-> try again in 5 seconds");
      delay(5000);
    }
  }
}
