#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <math.h>

// --- Config Wi-Fi ---
const char* ssid = "FIAP-IOT";
const char* password = "F!@p25.IOT";
const int listenPort = 4210; 
WiFiUDP udp;

// --- Backend Java ---
const char* backend_url = "http://192.168.0.100:8080/api/location";

// --- Estrutura dos sensores ---
struct Node { String id; float x; float y; float distancia; };
Node nodes[4] = {
  {"ESP1", 0, 0, 0},
  {"ESP2", 4, 0, 0},
  {"ESP3", 0, 4, 0},
  {"ESP4", 4, 4, 0}
};

// --- Converte RSSI em distância ---
float rssiToDistance(int rssi, int rssiRef=-45, float n=2.0) {
  return pow(10, (rssiRef - rssi) / (10 * n));
}

// --- Calcula localização com média ponderada ---
void calcularLocalizacao() {
  float estX = 0, estY = 0, totalWeight = 0;

  for (int i = 0; i < 4; i++) {
    float dist = nodes[i].distancia;
    if (dist <= 0) continue;
    float weight = 1.0 / fmax(dist, 0.1);
    estX += nodes[i].x * weight;
    estY += nodes[i].y * weight;
    totalWeight += weight;
  }

  if (totalWeight == 0) return;
  estX /= totalWeight;
  estY /= totalWeight;

  // --- Envia resultado ao backend ---
  String json = "{\"motoId\":\"MOTO-001\",\"patioId\":\"PATIO-1\",\"x\":" + 
                String(estX, 2) + ",\"y\":" + String(estY, 2) + ",\"tag\":\"patio\"}";
  HTTPClient http;
  http.begin(backend_url);
  http.addHeader("Content-Type", "application/json");
  http.POST(json);
  http.end();

  Serial.printf("📍 Localização moto → X: %.2f | Y: %.2f\n", estX, estY);
}

// --- Recebe dados dos sensores ---
void receberDados() {
  int packetSize = udp.parsePacket();
  if (!packetSize) return;

  char packet[256];
  int len = udp.read(packet, 256);
  packet[len] = 0;

  StaticJsonDocument<256> doc;
  if (deserializeJson(doc, packet)) return;

  String id = doc["sensorID"];
  int rssi = doc["bleRssi"];
  float dist = rssiToDistance(rssi);

  for (int i = 0; i < 4; i++) {
    if (nodes[i].id == id) {
      nodes[i].distancia = dist;
      Serial.printf("📡 %s → RSSI:%d → %.2fm\n", id.c_str(), rssi, dist);
    }
  }
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\n✅ Central conectada. IP: " + WiFi.localIP().toString());
  udp.begin(listenPort);
}

// --- Loop principal ---
void loop() {
  receberDados();
  calcularLocalizacao();
  delay(2000); // recalcula a cada 2s
}
