#include <WiFi.h>
#include <WiFiUdp.h>
#include <BLEDevice.h>
#include <BLEScan.h>

const char* ssid = "FIAP-IOT";
const char* password = "F!@p25.IOT";

const char* centralIP = "10.3.52.4"; // IP da CENTRAL
const int centralPort = 4210;
WiFiUDP udp;

const char* sensorID = "ESP1"; // Troque para ESP2, ESP3, ESP4
BLEScan* pBLEScan;
const int scanTime = 2; 

int getBleRssi() {
  int rssi = -100;
  BLEScanResults* results = pBLEScan->start(scanTime, false);
  for (int i = 0; i < results->getCount(); i++) {
    BLEAdvertisedDevice device = results->getDevice(i);
    if (device.getName() == "MotoFacil") {
      rssi = device.getRSSI();
      break;
    }
  }
  return rssi;
}

void sendToCentral(int bleRssi) {
  char buffer[128];
  snprintf(buffer, sizeof(buffer),
           "{\"sensorID\":\"%s\",\"bleRssi\":%d}", sensorID, bleRssi);
  udp.beginPacket(centralIP, centralPort);
  udp.write((uint8_t*)buffer, strlen(buffer));
  udp.endPacket();
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  Serial.println("\n✅ Conectado. IP: " + WiFi.localIP().toString());

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(false);
  udp.begin(4211);
}

void loop() {
  int bleRssi = getBleRssi();
  sendToCentral(bleRssi);
  delay(3000);
}
