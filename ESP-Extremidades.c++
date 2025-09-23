#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <BLEDevice.h>
#include <BLEScan.h>

const char* ssid = "iPhone Cauan";      // Trocar pela sua rede real
const char* password = "Cauan1010";

String sensorID = "ESP1"; // >>> Altere para ESP1, ESP2, ESP3 ou ESP4

WebServer server(80);
BLEScan* pBLEScan;
int scanTime = 2; // segundos

// --- Função: escaneia sinal BLE do celular/moto
int getBleRssi() {
  int bleRssi = -100; 
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  for (int i = 0; i < foundDevices.getCount(); i++) {
    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    if (device.getName() == "MotoFacil") { // nome BLE do celular/moto
      bleRssi = device.getRSSI();
      break;
    }
  }
  return bleRssi;
}

// --- Endpoint: /ping (retorna RSSI do celular/moto)
void handlePing() {
  int bleRssi = getBleRssi();
  String response = "Sensor: " + sensorID + "\nRSSI: " + String(bleRssi) + " dBm";
  Serial.println(response);
  server.send(200, "text/plain", response);
}

// --- Endpoint: /distance?target=ESP2 (mede distância relativa via RSSI)
void handleDistance() {
  String target = server.arg("target");
  // Aqui você pode simular ou implementar comunicação ESP-ESP via WiFi Direct/MQTT.
  // Para simplificar, vou devolver um valor fixo que o central usará para calibrar.
  // Depois você pode ajustar para pegar sinal real entre ESPs.
  float fakeDistance = 5.0; // metros simulados
  server.send(200, "text/plain", String(fakeDistance));
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
  server.on("/distance", handleDistance);
  server.begin();
  Serial.println("Servidor HTTP iniciado.");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);
}

void loop() {
  server.handleClient();
}
