#include <WiFi.h>
#include <WiFiUdp.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Wi-Fi
const char* ssid = "iPhone Cauan";
const char* password = "Cauan1010";

// Identificação do ESP
String sensorID = "ESP1"; // ESP1, ESP2, ESP3 ou ESP4

// UDP Central
const char* centralIP = "192.168.0.200";
const int centralPort = 4210;
WiFiUDP udp;

// BLE
BLEScan* pBLEScan;
int scanTime = 2;

// Lista dos outros ESPs para medir RSSI entre eles
String outrosESP[3] = {"ESP2","ESP3","ESP4"};
String espIPs[4] = {"172.20.10.9","172.20.10.10","172.20.10.11","172.20.10.12"};

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

// Pega RSSI de outro ESP via HTTP GET /ping
int getEspRssi(String targetID){
  String targetIP="";
  for(int i=0;i<4;i++) if(sensorID!=targetID && espIPs[i].endsWith(targetID.substring(3))) targetIP=espIPs[i];

  WiFiClient client;
  if(!client.connect(targetIP.c_str(),80)) return -100;
  client.print(String("GET /ping HTTP/1.1\r\nHost: ") + targetIP + "\r\nConnection: close\r\n\r\n");
  long timeout = millis()+500;
  while(!client.available() && millis()<timeout) delay(10);
  String response="";
  while(client.available()) response+=(char)client.read();
  int index=response.indexOf("RSSI:");
  int endIndex=response.indexOf(" dBm",index);
  if(index!=-1 && endIndex!=-1) return response.substring(index+5,endIndex).toInt();
  return -100;
}

// Envia RSSI para o central
void sendToCentral(String motoId) {
  int bleRssi = getBleRssi();
  StaticJsonDocument<256> doc;
  doc["sensorID"] = sensorID;
  doc["bleRssi"] = bleRssi;
  doc["motoId"] = motoId;

  JsonArray espDistances = doc.createNestedArray("espRssi");
  for(int i=0;i<3;i++){
    int rssi=getEspRssi(outrosESP[i]);
    espDistances.add(rssi);
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

  udp.begin(4211); // Porta local para receber ACK do central se necessário
}

void loop(){
  String motoId = "MOTO-001"; // Receber do app via backend seria ideal
  sendToCentral(motoId);
  delay(3000); // envia a cada 3s
}
