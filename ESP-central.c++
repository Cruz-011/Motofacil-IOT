#include <WiFi.h>
#include <HTTPClient.h>
#include <math.h>

// --- Config Wi-Fi
const char* ssid = "iPhone Cauan";
const char* password = "Cauan1010";

// --- IPs reais dos 4 sensores laterais
const char* sensorIPs[4] = {
  "172.20.10.9",   // ESP1
  "172.20.10.10",  // ESP2
  "172.20.10.11",  // ESP3
  "172.20.10.12"   // ESP4
};

struct Node {
  String id;
  float x;
  float y;
};

Node nodes[4];

// --- Backend Java
const char* backend_url = "http://192.168.0.100:8080/api/location";

// --- Função: requisita RSSI de um ESP lateral
int getRssiFromSensor(String ip) {
  HTTPClient http;
  http.begin("http://" + ip + "/ping");
  int httpCode = http.GET();
  int rssi = -100;
  if (httpCode == 200) {
    String payload = http.getString();
    int index = payload.indexOf("RSSI: ");
    int endIndex = payload.indexOf(" dBm", index);
    if (index != -1 && endIndex != -1) {
      rssi = payload.substring(index + 6, endIndex).toInt();
    }
  }
  http.end();
  return rssi;
}

// --- Função: pedir distância relativa entre ESPs (para calibração)
float getDistance(String ip, String target) {
  HTTPClient http;
  http.begin("http://" + ip + "/distance?target=" + target);
  int httpCode = http.GET();
  float dist = 5.0; // valor padrão
  if (httpCode == 200) {
    dist = http.getString().toFloat();
  }
  http.end();
  return dist;
}

// --- Calibração automática do pátio
void calibratePatio() {
  Serial.println("Iniciando calibração...");

  float d12 = getDistance(sensorIPs[0], "ESP2");
  float d13 = getDistance(sensorIPs[0], "ESP3");
  float d14 = getDistance(sensorIPs[0], "ESP4");

  // aqui você pode usar triangulação; para simplificar vou assumir retângulo
  nodes[0] = {"ESP1", 0, 0};
  nodes[1] = {"ESP2", d12, 0};
  nodes[2] = {"ESP3", 0, d13};
  nodes[3] = {"ESP4", d12, d13};

  Serial.println("✅ Calibração concluída!");
}

// --- Localização da moto/celular
void locateMoto() {
  int rssi[4];
  for (int i = 0; i < 4; i++) {
    rssi[i] = getRssiFromSensor(sensorIPs[i]);
    Serial.printf("RSSI do %s: %d dBm\n", nodes[i].id.c_str(), rssi[i]);
  }

  float estX = 0, estY = 0, totalWeight = 0;

  for (int i = 0; i < 4; i++) {
    float distance = pow(10, (abs(rssi[i]) - 45) / 20.0);
    float weight = 1.0 / fmax(distance, 0.1);
    estX += nodes[i].x * weight;
    estY += nodes[i].y * weight;
    totalWeight += weight;
  }

  estX /= totalWeight;
  estY /= totalWeight;

  Serial.printf("📍 Localização estimada: (%.2f, %.2f)\n", estX, estY);

  // Envia para backend
  HTTPClient http;
  http.begin(backend_url);
  http.addHeader("Content-Type", "application/json");
  String json = "{\"x\":" + String(estX, 2) + ",\"y\":" + String(estY, 2) + "}";
  int httpCode = http.POST(json);
  if (httpCode > 0) {
    Serial.println("✅ Localização enviada ao backend!");
  } else {
    Serial.println("❌ Falha ao enviar localização.");
  }
  http.end();
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Central conectada ao Wi-Fi!");

  calibratePatio(); // roda uma vez no início
}

void loop() {
  locateMoto();
  delay(5000); // atualiza a cada 5 segundos
}
