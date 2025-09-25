#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <math.h>

const char* ssid = "iPhone Cauan";
const char* password = "Cauan1010";
const int listenPort = 4210; 
WiFiUDP udp;

// Backend Java
const char* backend_url = "http://192.168.0.100:8080/api/location";

// Configura os 4 nós do pátio (ESP1 a ESP4)
struct Node { String id; float x; float y; };
Node nodes[4] = {{"ESP1",0,0},{"ESP2",5,0},{"ESP3",0,5},{"ESP4",5,5}};

// Converte RSSI em distância real
float rssiToDistance(int rssi,int rssiRef=-45,float n=2.0){ return pow(10,(rssiRef-rssi)/(10*n)); }

// Multilateração
void calcularLocalizacao(JsonObject moto){
  int bleRssi = moto["bleRssi"];
  JsonArray espRssi = moto["espRssi"];
  
  float estX=0, estY=0, totalWeight=0;
  for(int i=0;i<4;i++){
    float dist = (i==0)? rssiToDistance(bleRssi) : rssiToDistance(espRssi[i-1]);
    float weight = 1.0/fmax(dist,0.1);
    estX += nodes[i].x*weight;
    estY += nodes[i].y*weight;
    totalWeight += weight;
  }
  estX/=totalWeight;
  estY/=totalWeight;

  String motoId = moto["motoId"] | "MOTO-001";
  String patioId = "PATIO-1";
  String tag = "patio";

  String json = "{\"motoId\":\""+motoId+"\",\"patioId\":\""+patioId+"\",\"x\":"+String(estX,2)+",\"y\":"+String(estY,2)+",\"tag\":\""+tag+"\"}";
  HTTPClient http;
  http.begin(backend_url);
  http.addHeader("Content-Type","application/json");
  http.POST(json);
  http.end();

  Serial.printf("Moto %s → X:%.2f Y:%.2f\n", motoId.c_str(), estX, estY);
}

void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  Serial.print("Conectando");
  while(WiFi.status()!=WL_CONNECTED){delay(500); Serial.print(".");}
  Serial.println("\n✅ Central conectada IP: " + WiFi.localIP());
  udp.begin(listenPort);
}

void loop(){
  int packetSize = udp.parsePacket();
  if(packetSize){
    char packet[512];
    int len = udp.read(packet,512);
    packet[len]=0;

    StaticJsonDocument<512> doc;
    DeserializationError err = deserializeJson(doc, packet);
    if(!err){
      calcularLocalizacao(doc.as<JsonObject>());
    }
  }
  delay(10);
}
