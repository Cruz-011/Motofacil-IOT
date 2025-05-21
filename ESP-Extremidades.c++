#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
 
const char* ssid = "iPhone Cauan";
const char* password = "Cauan1010";
 
String sensorID = "ESP1"; // Altere para ESP1, ESP2, etc.
WebServer server(80);
 
void handlePing() {
  int rssi = WiFi.RSSI();
  String response = "Ping recebido!\nSensor: " + sensorID + "\nRSSI: " + String(rssi) + " dBm";
 
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
 
  // Rota para teste via navegador
  server.on("/ping", handlePing);
  server.begin();
  Serial.println("Servidor HTTP iniciado.");
}
 
void loop() {
  server.handleClient();
}