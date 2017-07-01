#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define Sensor 2 // gpio2

// Update these with values suitable for your network.
const char* ssid = "1111";
const char* password = "11111112";
const char* mqtt_server = "192.168.191.20";
//const char* mqtt_server = "iot.eclipse.org";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

void setup_wifi() {
   delay(100);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
} 

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} //end reconnect()

void setup() {
  Serial.begin(115200);
  pinMode(Sensor, INPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}


主循环程序
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
 // 每一秒钟读取一次传感器读数
  if (now - lastMsg > 1000) {
     lastMsg = now;
     int val = digitalRead(Sensor);
     String msg="";
     //将读数，储存到字符串，并发布响应的警报消息
     if (val ==0){
       msg= msg+"0";
        client.publish("hut/alarm1","0");
      }
     else{
      msg = msg+"1";
      client.publish("hut/alarm1","1");
     }
     char message[58];
     msg.toCharArray(message,58);
     Serial.println(message);
     //将消息发布到MQTT服务端
    client.publish("hut/pir", message);       
  }
}
