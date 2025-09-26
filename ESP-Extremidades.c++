#include <WiFi.h>
#include <WiFiUdp.h>
#include <BLEDevice.h>
#include <BLEScan.h>

// --- Configurações Wi-Fi ---
const char* ssid = "FIAP-IOT";
const char* password = "F!@p25.IOT";

// --- Central ---
const char* centralIP = "10.3.52.4";
const int centralPort = 4210;
WiFiUDP udp;

// --- Sensor ---
const char* sensorID = "ESP1";
BLEScan* pBLEScan;
const int scanTime = 2; // segundos

// --- Função que pega RSSI ---
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

// --- Envia RSSI (JSON manual) ---
void sendToCentral(int bleRssi) {
  char buffer[128];
  snprintf(buffer, sizeof(buffer),
           "{\"sensorID\":\"%s\",\"bleRssi\":%d,\"x\":0,\"y\":0,\"espRssi\":[-100,-100,-100]}",
           sensorID, bleRssi);

  udp.beginPacket(centralIP, centralPort);
  udp.write((uint8_t*)buffer, strlen(buffer));
  udp.endPacket();
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\n✅ Conectado ao Wi-Fi.");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(false); // menor tamanho

  udp.begin(4211);
}

// --- Loop ---
void loop() {
  int bleRssi = getBleRssi();
  sendToCentral(bleRssi);
  delay(3000);
}
