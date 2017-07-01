// Loading the ESP8266WiFi library and the PubSubClient library
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


// 
int SoffitR;
int SoffitG;
int SoffitB;
const int RlightPin = 12;
const int GlightPin = 14;
const int BlightPin = 13;



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

//主要 ：连接上MQTT，从服务端传输数据时的响应，并把收到的数据进行处理
void callback(String topic, byte* payload, unsigned int length) {
    String topicStr = topic; 
   String value = String((char*)payload);

  // 将收到的字符串数据（如'1023,1023,1023'）,分割开并转为整型处理，付给RGB
    SoffitR = value.substring(0,value.indexOf(',')).toInt();
    SoffitG = value.substring(value.indexOf(',')+1,value.lastIndexOf(',')).toInt();
    SoffitB = value.substring(value.lastIndexOf(',')+1).toInt();

 //将对应的颜色的GPIO输出模拟量电压值
  analogWrite(GlightPin, SoffitG);
  analogWrite(RlightPin, SoffitR);
  analogWrite(BlightPin, SoffitB);
    
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect

    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
     // 订阅话题
       client.subscribe("hut/rgb");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

//初始化
void setup() {
  Serial.begin(115200);
  pinMode(RlightPin, OUTPUT);
  pinMode(GlightPin, OUTPUT);
  pinMode(BlightPin, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

主循环
void loop() {
  if (!client.connected() && WiFi.status() == 3) {reconnect();}

  //maintain MQTT connection
  client.loop();

  //MUST delay to allow ESP8266 WIFI functions to run
  delay(10); 
}
