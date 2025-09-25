#include <WiFi.h>
#include <WiFiUdp.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "iPhone Cauan";
const char* password = "Cauan1010";

String sensorID = "ESP1"; // ESP1, ESP2, ESP3, ESP4
const char* centralIP = "192.168.0.200";
const int centralPort = 4210;
WiFiUDP udp;

BLEScan* pBLEScan;
int scanTime = 2;

// Lista dos outros ESPs
String outrosESP[3] = {"ESP2","ESP3","ESP4"};
String espIPs[4] = {"192.168.0.201","192.168.0.202","192.168.0.203","192.168.0.204"};

// Pega RSSI do celular/moto
int getBleRssi() {
  int bleRssi = -100; 
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  for (int i = 0; i < foundDevices.getCount(); i++) {
    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    if (device.getName() == "MotoFacil") { 
      bleRssi = device.getRSSI();
      break;
    }
  }
  return bleRssi;
}

// Envia posição ou RSSI para central
void sendToCentral(float x=0, float y=0, int bleRssi=-100, JsonArray espRssi={}) {
  StaticJsonDocument<256> doc;
  doc["sensorID"] = sensorID;
  doc["x"] = x;
  doc["y"] = y;
  doc["bleRssi"] = bleRssi;

  JsonArray espArr = doc.createNestedArray("espRssi");
  for(int i=0;i<3;i++){
    int rssi=-100; // Pode adicionar scan entre ESPs
    espArr.add(rssi);
  }

  char buffer[256];
  size_t n = serializeJson(doc, buffer);
  udp.beginPacket(centralIP, centralPort);
  udp.write((uint8_t*)buffer,n);
  udp.endPacket();
}

void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  Serial.print("Conectando");
  while(WiFi.status()!=WL_CONNECTED){delay(300); Serial.print(".");}
  Serial.println("\n✅ " + sensorID + " conectado, IP: " + WiFi.localIP());

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);

  udp.begin(4211);
}

void loop(){
  String motoId = "MOTO-001"; 
  int bleRssi = getBleRssi();
  sendToCentral(0,0,bleRssi); // As coordenadas x,y serão calculadas pelo central
  delay(3000);
}
