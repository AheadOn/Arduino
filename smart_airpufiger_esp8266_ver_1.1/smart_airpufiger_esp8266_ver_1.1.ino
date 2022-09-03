#include <ESP8266WiFi.h>
#include <DHT.h>                    
#define DHTPIN 4                  
#define DHTTYPE DHT11                
DHT dht(4, DHT11);

// ** 동방/기기 상태 변수 선언 **

int fanStatus;   // 공기청정기 on off 상태 변수 ** 아직 적용안함
char Airinform;     // 동방 공기질 상태 변수
int Dust = 0;

char DUST[10];
char TEM[10];
char HUMI[10];

int Vo = A0 ;
int V_LED = 2 ;
int FAN = 5 ;
int R_LED = 12 ;
int Y_LED = 14 ;
int G_LED = 16 ;

//미세먼지값 측정을 위한 데이터값 변수 선언
float Vo_value = 0;
float Voltage = 0;
float a = 0;

const char* ssid = "SSCCWIFI"; 
const char* password = "2022sscc";
 
WiFiServer server(80);
 
void setup() {
  pinMode(V_LED,OUTPUT);
  pinMode(Vo,INPUT);
  pinMode(FAN,OUTPUT);
  pinMode(R_LED,OUTPUT);
  pinMode(Y_LED,OUTPUT);
  pinMode(G_LED,OUTPUT);
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.begin();
  Serial.println("Server started");
}
 
void loop() {
  unsigned char t = dht.readTemperature();
  unsigned char h = dht.readHumidity();
  digitalWrite(V_LED,LOW);
  delayMicroseconds(280);
  Vo_value = analogRead(Vo);
  delayMicroseconds(40);
  digitalWrite(V_LED,HIGH);
  delayMicroseconds(9680);

  Voltage = Vo_value * 5.0 / 1024.0;
  if( Voltage < 0.1 ){
    Dust = 0 ;
  } 
  else if ( Voltage < 0.8){
    Dust =10*(5*(Voltage - 0.1));   
  }
 
  else if ( Voltage < 1 ){
    Dust = 10 + 15*((Voltage - 0.8)*10);
  }
 
  else if( Voltage > 1){
    Dust = 40 + 16*((Voltage - 1)*10);
  }
  delay(500);
 
 if (  Dust < 50 ){   //미세먼지 농도 좋음, 초록불, 팬 정지
        digitalWrite(FAN,LOW);
        digitalWrite(R_LED,LOW);
        digitalWrite(Y_LED,LOW);
        digitalWrite(G_LED,HIGH);
        sprintf(DUST,"%03d",Dust);
        sprintf(TEM,"%02d",t);
        sprintf(HUMI,"%02d",h);
        /*
        Serial.print(DUST);
        Serial.print("Good Air! No need to work on");
        Serial.print(TEM);
        Serial.print(HUMI);
        */
  } 
  
  else if ( Dust < 80 ){   //미세먼지 농도 보통, 노란불, 팬 동작
        digitalWrite(FAN,HIGH);
        digitalWrite(R_LED,LOW);
        digitalWrite(Y_LED,HIGH);
        digitalWrite(G_LED,LOW);
        sprintf(DUST,"%03d",Dust);
        sprintf(TEM,"%02d",t);
        sprintf(HUMI,"%02d",h);
        /*
        Serial.print(DUST);
        Serial.print("Not good Air...I am working on");
        Serial.print(TEM);
        Serial.print(HUMI);
        */
  }
  
  else if ( Dust > 80) {   //미세먼지 농도 나쁨, 빨간불, 팬 동작
        digitalWrite(FAN,HIGH);
        digitalWrite(R_LED,HIGH);
        digitalWrite(Y_LED,LOW);
        digitalWrite(G_LED,LOW);
        sprintf(TEM,"%02d",t);
        sprintf(DUST,"%03d",Dust);
        sprintf(HUMI,"%02d",h);
        /*
        Serial.print(DUST);
        Serial.print("Bad Air...I'm working hard!");
        Serial.print(TEM);
        Serial.print(HUMI);
        */
  }


  WiFiClient client = server.available();
  if(!client) return;
  
  Serial.println("새로운 클라이언트");
  client.setTimeout(5000);
  
  String request = client.readStringUntil('\r');
  Serial.println("request: ");
  Serial.println(request);
 
  if(request.indexOf("/fanOff") != -1) {
    //digitalWrite(PIN_LED, LOW);
  }
  else if(request.indexOf("/fanOn") != -1) {
    //digitalWrite(PIN_LED, HIGH);
  }
  else {
    Serial.println("invalid request");
  }
 
  while(client.available()) {
    client.read();
  }
 
  client.print("HTTP/1.1 200 OK");
  client.print("Content-Type: text/html\r\n\r\n");
  client.print("<!DOCTYPE HTML>");
  client.print("<html>");
  client.print("<head>"); 
  client.print("<meta&nbsp;charset=\"UTF-8\">");
  client.print("<title>SSCC AIR PURIFIER</title>");
  client.print("</head>");
  client.print("<body>");
  client.print("<h2>SSCC AIR PURIFER Control Page</h2>");
  client.print("Fan Status : ");
  client.print(fanStatus ? "ON" : "OFF");
  client.print("<br>");
  client.print("Dust : ");
  client.print(DUST);
  client.print("%");
  client.print("<br>");
  client.print("Temperature :");
  client.print(TEM);
  client.print("'C");
  client.print("<br>");
  client.print("Humidity : ");
  client.print(HUMI);
  client.print("%");
  client.print("</body>");
  client.print("</html>");
 
  Serial.println("클라이언트 연결 해제");
}
