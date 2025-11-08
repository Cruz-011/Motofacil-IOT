# 📡 MotoFácil IoT — Sistema de Localização Indoor com ESP32

Este guia apresenta **todo o processo de configuração, execução e integração** do **MotoFácil IoT**, um sistema de localização indoor baseado em **ESP32**, **RSSI**, **backend Java/Spring Boot** e **app React Native (Expo)**.

---

## 🚀 1️⃣ Visão Geral

O **MotoFácil IoT** tem como objetivo **localizar motos ou objetos dentro de pátios, galpões ou estacionamentos** utilizando a força do sinal Wi-Fi (**RSSI**) entre múltiplos ESP32.

A arquitetura combina:

* Sensores ESP32 que captam o sinal RSSI da moto.
* Um ESP32 central que recebe os dados dos sensores e calcula a posição via multilateração.
* Um backend Java para persistência e integração com o app.
* Um app React Native/Expo para visualização e rastreamento em mapa.

---

## ⚙️ 2️⃣ Pré-requisitos

* ✅ 5x ESP32 DevKit (4 sensores + 1 central)
* ✅ Rede Wi-Fi local (pode ser hotspot do celular)
* ✅ Arduino IDE ou VSCode com PlatformIO
* ✅ Backend configurado → [motofacil-java](https://github.com/Cruz-011/motofacil-java)
* ✅ Frontend configurado → [MOTOFACIL-APP](https://github.com/Cruz-011/MOTOFACIL-APP)

---

## 🧩 3️⃣ Clonando o Projeto

```bash
git clone https://github.com/Cruz-011/Motofacil-IOT.git
cd Motofacil-IOT
```

---

## 🏗️ 4️⃣ Arquitetura do Sistema

O sistema é composto por **4 ESPs sensores** e **1 ESP central**:

* Os **sensores** medem a intensidade do sinal (**RSSI**) emitido pela moto (BLE ou Wi-Fi).
* O **ESP central** recebe essas medições via **UDP**, calcula a posição da moto usando multilateração e envia os dados para o **backend Java**.
* O **app mobile** exibe em mapa a posição em tempo real.

```plaintext
       [ESP1] -------- [ESP2]
          |               |
          |     (MOTO)    | → RSSI capturado
          |               |
       [ESP3] -------- [ESP4]

                ↓
            [ESP CENTRAL]
```

---

## 🔧 5️⃣ Configuração dos ESPs

### 🌐 Wi-Fi

Em todos os códigos (central e sensores), configure a rede Wi-Fi:

```cpp
const char* ssid = "NOME_DA_REDE";
const char* password = "SENHA_DA_REDE";
```

### 🆔 Identificação dos Sensores

Em cada ESP sensor, altere o ID:

```cpp
const char* sensorID = "ESP1"; // Trocar para ESP2, ESP3, ESP4...
```

### 🌍 IP da Central

No código dos sensores, defina o IP do ESP central:

```cpp
const char* centralIP = "10.3.52.4"; // IP da central
```

---

## 🔌 6️⃣ Upload dos Códigos

| Tipo de ESP | Código a usar | Função                                                        |
| ----------- | ------------- | ------------------------------------------------------------- |
| 4 sensores  | `sensor.ino`  | Captura RSSI e envia para central via UDP                     |
| 1 central   | `central.ino` | Recebe dados dos sensores, calcula posição e envia ao backend |

Use o **Arduino IDE** → selecione **ESP32 Dev Module** → **Upload** em cada dispositivo.

---

## ☁️ 7️⃣ Integração com o Backend

A central envia os dados para o backend:

```http
POST http://192.168.0.100:8080/api/location
```

**Exemplo de payload:**

```json
{
  "motoId": "MOTO-001",
  "patioId": "PATIO-1",
  "x": 2.45,
  "y": 3.12,
  "tag": "patio"
}
```

> ⚠️ Ajuste o IP conforme o da sua máquina ou servidor local.

---

## 📲 8️⃣ Como Usar

1. Conecte todos os ESPs à **mesma rede Wi-Fi** (ex: hotspot do celular).
2. Ligue os 4 sensores → cada um coleta o RSSI da moto.
3. Ligue o ESP central → ele recebe os dados e calcula a posição.
4. Veja no **Monitor Serial** a localização estimada:

   ```plaintext
   📍 Localização estimada da moto: X=2.45 | Y=3.12
   ```
5. A posição é enviada ao backend e exibida no **app MotoFácil**.

---

## 🗂️ 9️⃣ Estrutura do Projeto

```plaintext
Motofacil-IOT/
├── central.ino       # Código do ESP Central (cálculo de localização)
├── sensor.ino        # Código dos ESP Sensores (coleta RSSI)
├── README.md          # Este guia
```

---

## 🚧 🔟 Roadmap (Melhorias Futuras)

* 🗺️ Dashboard em tempo real (React ou Python) para visualização 2D do pátio
* ☁️ Envio dos dados para banco de dados na nuvem (Firebase / AWS IoT Core)
* 🔋 Monitoramento do consumo e bateria dos ESPs
* 🎯 Algoritmos de precisão (Filtro de Kalman, ML para suavização de RSSI)
* 🔔 Notificações push no app quando a moto se mover fora da zona permitida

---

## 👨‍💻 Créditos

Desenvolvido por **Inovex Group**
Projeto acadêmico FIAP — *MOTOFÁCIL*
Equipe:

* **Cauan da Cruz Ferreira** — RM 558238
* **Igor Dias Barrocal** — RM 555217

---

> 📎 Repositórios relacionados:
>
> * [⚙️ Backend Java/Spring Boot](https://github.com/Cruz-011/motofacil-java)
> * [📱 Frontend React Native/Expo](https://github.com/Cruz-011/MOTOFACIL-APP)
> * [💻 Simulador ESPs (Python)](https://github.com/Cruz-011/simuladorESPS)
