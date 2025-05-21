
# 📡 MotoFácil IoT - Sistema de Localização com ESP32

## 🔥 Visão Geral
Este projeto utiliza **ESP32** para simular um sistema de localização indoor baseado na força do sinal Wi-Fi (**RSSI**) entre múltiplos sensores. Ele permite estimar a posição de uma moto ou objeto dentro de um pátio, galpão ou estacionamento.

## 🧠 Tecnologias e Componentes
- 📶 **ESP32 DevKit** (mínimo 5 unidades)
- 🔌 **Rede Wi-Fi Local** (pode ser hotspot do celular)
- 🖥️ Computadores para monitoramento via Serial Monitor ou interface web
- 🚀 Linguagem: **C++ (Arduino Framework)**

## 🏗️ Arquitetura
- **4 ESP32 sensores nas extremidades do pátio** — medem a intensidade do sinal (RSSI) do objeto.
- **1 ESP32 central** — coleta os dados dos sensores e calcula a posição.
- 🚀 O celular atua como roteador Wi-Fi (hotspot).

```plaintext
       [ESP1] -------- [ESP2]
          |               |
          |     (MOTO)    | → RSSI capturado
          |               |
       [ESP3] -------- [ESP4]

                ↓
            [ESP CENTRAL]
```

## ⚙️ Funcionalidades
- Conexão Wi-Fi automática.
- Cada sensor possui um endpoint HTTP que responde ao ping com o valor do RSSI.
- O ESP central faz requisições HTTP para todos os sensores, recebe os RSSI e calcula a posição da moto usando ponderação inversa pela distância estimada.
- Visualização dos dados no monitor serial do Arduino.

## 🚀 Setup

- Bibliotecas:
  - `WiFi.h` (nativa do ESP32)
  - `HTTPClient.h` (nativa do ESP32)

### 2️⃣ Configuração de Rede
- Defina seu **SSID** e **senha do Wi-Fi** no código:
```cpp
const char* ssid = "iPhone Cauan";
const char* password = "Cauan1010";
```

### 3️⃣ Endereçamento IP
- Configure IPs fixos no roteador (recomendado) ou no código.
Exemplo no código central:
```cpp
const char* sensorIPs[4] = {
  "172.20.10.9", // ESP1
  "172.20.10.10", // ESP2
  "172.20.10.11", // ESP3
  "172.20.10.12"  // ESP4
};
```

### 4️⃣ Upload dos Códigos
- ✅ Carregue o código dos **sensores** (servidores HTTP).
- ✅ Carregue o código do **ESP central** (cliente HTTP).

## 🖥️ Como Usar
1. ✅ Ligue todos os ESP32.
2. ✅ Todos se conectam à rede Wi-Fi do celular.
3. ✅ O ESP central consulta os sensores.
4. ✅ Veja no Monitor Serial do central algo como:
```plaintext
📍 Localização estimada da moto: (2.45, 3.12)
```

## 📜 Estrutura do Projeto
```plaintext
├── central.ino       # Código para o ESP central (cliente HTTP)
├── sensor.ino        # Código para os 4 ESP sensores (servidores HTTP)
├── README.md         # Este arquivo
```

## 📈 Futuras Melhorias (Roadmap)
- 🗺️ Visualização gráfica da posição (Dashboard em React ou Python)
- ☁️ Envio dos dados para um servidor ou banco de dados na nuvem
- 🔋 Monitoramento de bateria dos ESPs
- 🎯 Algoritmo mais preciso com Kalman Filter ou Machine Learning

## 🏆 Créditos
Desenvolvido por **Inovex Group**