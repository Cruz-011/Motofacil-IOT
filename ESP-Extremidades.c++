#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <BLEDevice.h>
#include <BLEScan.h>

const char* ssid = "iPhone Cauan";
const char* password = "Cauan1010";

String sensorID = "ESP1"; // Altere para ESP1, ESP2, ESP3, ESP4 conforme cada extremidade
WebServer server(80);
BLEScan* pBLEScan;
int scanTime = 2; // segundos

// Este ESP32 de extremidade escaneia BLE do celular/moto e responde via HTTP
// para o ESP-central coletar o RSSI em tempo real.

void handlePing() {
  int bleRssi = -100; // Valor padrão caso não encontre o dispositivo BLE
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  for (int i = 0; i < foundDevices.getCount(); i++) {
    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    // Troque "MotoFacil" pelo nome real do BLE do seu celular/moto
    if (device.getName() == "MotoFacil") {
      bleRssi = device.getRSSI();
      break;
    }
  }
  String response = "Ping recebido!\nSensor: " + sensorID + "\nRSSI: " + String(bleRssi) + " dBm";
  Serial.println(response);
  server.send(200, "text/plain", response);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\n✅ Conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/ping", handlePing);
  server.begin();
  Serial.println("Servidor HTTP iniciado.");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);
}

void loop() {
  server.handleClient();
}