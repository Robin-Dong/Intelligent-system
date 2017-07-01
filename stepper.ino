#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define IN1 12 //GPIO 12
#define IN2 13 //GPIO 13
#define IN3 14 //GPIO 14
#define IN4 16 //GPIO 16

//连接wifi,mqtt
const char* ssid = "1111";
const char* password = "11111112";
 const char* mqtt_server = "192.168.191.20";
//const char* mqtt_server = "iot.eclipse.org";

const int NBSTEPS = 4095;
const int STEPTIME = 900;
int Step = 0;
boolean Clockwise = true;

int arrayDefault[4] = {LOW, LOW, LOW, LOW};

//四相八拍
int stepsMatrix[8][4] = {
  {LOW, LOW, LOW, HIGH},
  {LOW, LOW, HIGH, HIGH},
  {LOW, LOW, HIGH, LOW},
  {LOW, HIGH, HIGH, LOW},
  {LOW, HIGH, LOW, LOW},
  {HIGH, HIGH, LOW, LOW},
  {HIGH, LOW, LOW, LOW},
  {HIGH, LOW, LOW, HIGH},
};

unsigned long lastTime = 0L;
unsigned long changetime = 0L;

void writeStep(int outArray[4]);
void stepper();
void setDirection();

WiFiClient espClient;
PubSubClient client(espClient);

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


// 主要  ：收到MQTT服务器传输数据后，对消息进行处理，控制电机正反转
void callback(char* topic, byte* payload, unsigned int length) 
{
  int p =(char)payload[0]-'0';

//正转
  if(p==1) 
  {
    Clockwise = true;
    go(4);
    delay(1000);
    }
  //反转
  else if(p==0)
  {
    Clockwise = !Clockwise;
    go(4);
   }
}
 
//  Serial.println();
 //end callback

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
      client.subscribe("hut/step");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} //end reconnect()


主程序初始化
void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}

//引脚对应数组位置
void writeStep(int outArray[4]) {
  digitalWrite(IN1, outArray[0]);
  digitalWrite(IN2, outArray[1]);
  digitalWrite(IN3, outArray[2]);
  digitalWrite(IN4, outArray[3]);
}

void stepper() {
  if ((Step >= 0) && (Step < 8)) {
    writeStep(stepsMatrix[Step]);
  } else {
    writeStep(arrayDefault);
  }
  setDirection();
}

void setDirection() {
  (Clockwise == true) ? (Step++) : (Step--);

  if (Step > 7) {
    Step = 0;
  } else if (Step < 0) {
    Step = 7;
  }
}

//转动（圈数）
void go(int n){
  unsigned long currentMicros;
  int stepsLeft =n* NBSTEPS;
  changetime = 0;
  lastTime = micros();
  while (stepsLeft > 0) {
    currentMicros = micros();
    if (currentMicros - lastTime >= STEPTIME) {
      stepper();
      changetime += micros() - lastTime;
      lastTime = micros();
      stepsLeft--;
    }
    delay(1);
  }
  Serial.println(changetime);
  Serial.println("Wait...!");
  delay(1000);
}
