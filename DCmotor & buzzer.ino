// 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>



// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "1111";
const char* password = "11111112";



// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "192.168.191.20";

// Initializes the espClient
WiFiClient espClient;
PubSubClient client(espClient);


int c = 0;
int b = 0;
// DHT Sensor
const int fanPin = 5;                             
const int buzzerPin = 14;
const int buzzer1Pin = 16;
// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;

// Don't change the function below. This functions connects your ESP8266 to your router
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

// 主要：  接收到MQTT服务端的信息更新，并执行相应动作，开关风扇，传感器异常警报
void callback(String topic, byte* payload, unsigned int length) {
    String topicStr = topic; 
    
    //话题为风扇是，执行相应指令
    if(topicStr == "hut/fan"){
      if(payload[0] == '1'){
      //开
      digitalWrite(fanPin, LOW);
    }
  //关
     else {
     digitalWrite(fanPin, HIGH);
    }
  }
  
    //烟雾和温度异常话题警报 
   if(topicStr == "hut/alarm2" || "hut/alarm3"){
    if(topicStr== "hut/alarm2")
     {  b = int(payload[0]);} 
    if(topicStr== "hut/alarm3")
     {  c = int(payload[0]);} 
     
     if(( b-48) || (c-48) == 1 ){
      digitalWrite(buzzerPin, LOW);
   }
     else if(( b-48) || (c-48) == 0)
     {
      digitalWrite(buzzerPin, HIGH);
      Serial.println(topicStr);}
   }

   //人体红外警报
  if(topicStr == "hut/alarm1")
  {
    if (payload[0]=='1'){
     digitalWrite(buzzer1Pin,HIGH);
     Serial.println("in here");
    }
    else{
      digitalWrite(buzzer1Pin, LOW);
    }
   
   }

    
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
     /*
     YOU  NEED TO CHANGE THIS NEXT LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
     To change the ESP device ID, you will have to give a unique name to the ESP8266.
     Here's how it looks like now:
       if (client.connect("ESP8266Client")) {
     If you want more devices connected to the MQTT broker, you can do it like this:
       if (client.connect("ESPOffice")) {
     Then, for the other ESP:
       if (client.connect("ESPGarage")) {
      That should solve your MQTT multiple connections problem

     THE SECTION IN loop() function should match your device name
    */
    if (client.connect("ESP8266fan")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
       client.subscribe("hut/fan");
       client.subscribe("hut/alarm1");
       client.subscribe("hut/alarm2");
       client.subscribe("hut/alarm3");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// The setup function sets your ESP GPIOs to Outputs, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually controls the LEDs
void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin,HIGH);
  pinMode(buzzer1Pin, OUTPUT);
  digitalWrite(buzzer1Pin,LOW);
  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin,HIGH);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

// For this project, you don't need to change anything in the loop function. 
// Basically it ensures that you ESP is connected to your broker
void loop() {
  if (!client.connected() && WiFi.status() == 3) {reconnect();}

  //maintain MQTT connection
  client.loop();

  //MUST delay to allow ESP8266 WIFI functions to run
  delay(10); 
}
