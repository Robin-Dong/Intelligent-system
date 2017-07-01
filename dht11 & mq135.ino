//导入ESP8266WiFi,MQTT客户端
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// 定义所使用的温度传感器信号
#define DHTTYPE DHT11   // DHT 11

// 设置ESP8266WiFi芯片所要连接网络的名称和密码，以及MQTT服务器的IP
const char* ssid = "1111";
const char* password = "11111112";
const char* mqtt_server = "192.168.191.20";

// 初始化ESP客户端
WiFiClient espClient;
PubSubClient client(espClient);

#定义引脚
const int DHTPin = 5; //GPIO5
const int Aout = A0;  //ADC脚                          

// 初始化引脚
DHT dht(DHTPin, DHTTYPE);


long now = millis();
long lastMeasure = 0;


//ESP8266芯片WiFi功能的设置
void setup_wifi() {
  delay(10);
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



//订阅响应时的信息处理，（话题，数据，长度）
void callback(String topic, byte* payload, unsigned int length) {
  
}


//重新连接MQTT服务器,订阅话题
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("DHT")) {
      Serial.println("connected");  
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
初始化主程序（波特率设置，MQTT客户端初始化设置）
void setup() {
  dht.begin();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

// For this project, you don't need to change anything in the loop function. 
// Basically it ensures that you ESP is connected to your broker
主循环程序
void loop() {
 //确保连接到MQTT服务器
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("DHT");
    
  now = millis();
  // 每五秒发布新的温湿度，和空气质量
  if (now - lastMeasure > 5000) {
    lastMeasure = now;
    //读取湿度
    float h = dht.readHumidity();
    // 读取温度
    float t = dht.readTemperature();


    //读取失败的处理
    if (isnan(h) || isnan(t) ) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    //计算温度,湿度,转化为字符串格式
    float hic = dht.computeHeatIndex(t, h, false);
    static char temperatureTemp[7];
    dtostrf(hic, 6, 1, temperatureTemp);

    static char humidityTemp[7];
    dtostrf(h, 6, 1, humidityTemp);
 
    // 读取mq135空气质量读数，并转为字符串
      int val = analogRead(Aout);
      String cs= String(val);
      static char message[58];
      cs.toCharArray(message,58);
      Serial.println(message);
    
    
    //发布传感器数据到MQTT服务器处理
    client.publish("hut/mq135", message);
    client.publish("hut/temperature", temperatureTemp);
    client.publish("hut/humidity", humidityTemp);

    //温度和空气质量超标，警报信息的发布
    if (hic < 35)
       {
        client.publish("hut/alarm3","0");
        }
    else{
        client.publish("hut/alarm3","1");
         }
    if(val<600){
      client.publish("hut/alarm2","0");      
    }
    else{
      client.publish("hut/alarm2","1");
    }
  }
}
