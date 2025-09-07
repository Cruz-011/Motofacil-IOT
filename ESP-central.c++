#include <WiFi.h>
#include <HTTPClient.h>
#include <math.h>

const char* ssid = "iPhone Cauan";
const char* password = "Cauan1010";

// IPs reais dos 4 sensores (ajuste conforme sua rede)
const char* sensorIPs[4] = {
  "172.20.10.9",   // ESP1
  "172.20.10.10",  // ESP2
  "172.20.10.11",  // ESP3
  "172.20.10.12"   // ESP4
};

// Posições reais dos sensores (em metros, ajuste conforme instalação física)
float sensorPos[4][2] = {
  {0, 0},     // ESP1
  {0, 5},     // ESP2
  {5, 0},     // ESP3
  {5, 5}      // ESP4
};

int rssi[4];
// Coloque o IP e porta reais do seu backend Java
const char* backend_url = "http://192.168.0.100:8080/api/location";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Central conectada ao Wi-Fi!");
}

void loop() {
  for (int i = 0; i < 4; i++) {
    String url = "http://" + String(sensorIPs[i]) + "/ping";
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      Serial.println("Resposta do sensor:");
      Serial.println(payload);

      int index = payload.indexOf("RSSI: ");
      int endIndex = payload.indexOf(" dBm", index);
      if (index != -1 && endIndex != -1) {
        String rssiStr = payload.substring(index + 6, endIndex);
        rssiStr.trim();
        rssi[i] = rssiStr.toInt();
      } else {
        rssi[i] = -100;
      }
    } else {
      Serial.printf("❌ Erro ao acessar %s\n", sensorIPs[i]);
      rssi[i] = -100;
    }
    http.end();
  }

  // Estimar posição baseada nos RSSIs reais
  float estX = 0;
  float estY = 0;
  float totalWeight = 0;

  for (int i = 0; i < 4; i++) {
    float distance = pow(10, (abs(rssi[i]) - 45) / 20.0); // Ajuste conforme testes reais
    float weight = 1.0 / fmax(distance, 0.1);
    estX += sensorPos[i][0] * weight;
    estY += sensorPos[i][1] * weight;
    totalWeight += weight;
  }

  estX /= totalWeight;
  estY /= totalWeight;

  Serial.printf("📍 Localização estimada da moto: (%.2f, %.2f)\n", estX, estY);

  // Envia para backend Java
  HTTPClient http;
  http.begin(backend_url);
  http.addHeader("Content-Type", "application/json");
  String json = "{\"x\":" + String(estX, 2) + ",\"y\":" + String(estY, 2) + "}";
  int httpCode = http.POST(json);
  if (httpCode > 0) {
    Serial.println("Localização enviada ao backend!");
  } else {
    Serial.println("Falha ao enviar localização ao backend.");
  }
  http.end();

  delay(15000);
}

// Este ESP32 central consulta os 4 sensores de extremidade via HTTP,
// calcula a posição da moto e envia para o backend Java para persistência.