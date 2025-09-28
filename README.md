# 📡 MotoFácil IoT - Sistema de Localização Indoor com ESP32

Este guia cobre tudo para rodar o MotoFácil IoT: sensores ESP32 para localização indoor baseada em RSSI, integração com backend Java/Spring Boot e frontend React/Expo. Inclui configuração, execução e roadmap de melhorias.

---

## 1️⃣ Visão Geral

O MotoFácil IoT utiliza **ESP32** para simular a localização de motos ou objetos dentro de pátios, galpões ou estacionamentos, usando a força do sinal Wi-Fi (**RSSI**) entre múltiplos sensores.

---

## 2️⃣ Pré-requisitos

- 5x ESP32 DevKit
- Rede Wi-Fi local (pode ser hotspot do celular)
- Computador com Arduino IDE ou VSCode + PlatformIO
- Backend configurado ([motofacil-java](https://github.com/Cruz-011/motofacil-java))
- Frontend configurado ([MOTOFACIL-APP](https://github.com/Cruz-011/MOTOFACIL-APP))

---

## 3️⃣ Clonando o Projeto

```bash
git clone https://github.com/Cruz-011/Motofacil-IOT.git
cd Motofacil-IOT
```

---

## 4️⃣ Arquitetura do Sistema

- **4 ESP32 sensores** posicionados nas extremidades do pátio medem o RSSI do objeto/moto.
- **1 ESP32 central** coleta os dados dos sensores via HTTP e calcula a posição.
- O celular atua como roteador Wi-Fi (hotspot).

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

## 5️⃣ Configuração dos ESPs

### Wi-Fi

No código dos ESPs, defina SSID e senha da rede Wi-Fi:
```cpp
const char* ssid = "NOME_DA_REDE";
const char* password = "SENHA_DA_REDE";
```

### Endereçamento IP

Configure IPs fixos para os sensores:
```cpp
const char* sensorIPs[4] = {
  "172.20.10.9", // ESP1
  "172.20.10.10", // ESP2
  "172.20.10.11", // ESP3
  "172.20.10.12"  // ESP4
};
```

### Upload dos Códigos

- Faça upload do código de **sensor** (servidor HTTP) para os 4 ESPs sensores.
- Faça upload do código de **central** (cliente HTTP) para o ESP central.

---

## 6️⃣ Integração com Backend

Os dados de localização podem ser enviados do ESP central para o backend Java/Spring Boot:
```
POST http://localhost:8080/api/esps/data
```
> Ajuste a URL conforme o IP/porta do backend configurado.

---

## 7️⃣ Como Usar

1. Ligue todos os ESP32 e conecte-os à rede Wi-Fi do hotspot.
2. O ESP central consulta os sensores via HTTP.
3. Veja no Monitor Serial do central a localização estimada:
```plaintext
📍 Localização estimada da moto: (2.45, 3.12)
```

---

## 8️⃣ Estrutura do Projeto

```plaintext
Motofacil-IOT/
├── central.ino       # Código para o ESP central (cliente HTTP)
├── sensor.ino        # Código para os 4 ESP sensores (servidores HTTP)
├── README.md         # Este arquivo
```

---

## 9️⃣ Roadmap (Futuras Melhorias)

- 🗺️ Visualização gráfica da posição (Dashboard em React ou Python)
- ☁️ Envio dos dados para servidor ou banco de dados na nuvem
- 🔋 Monitoramento de bateria dos ESPs
- 🎯 Algoritmo mais preciso (Kalman Filter, Machine Learning)

---

## 🔟 Créditos

Desenvolvido por **Inovex Group**

---

> Para dúvidas,integração, confira também:
> - [Backend Java/Spring Boot](https://github.com/Cruz-011/motofacil-java)
> - [Frontend React Native/Expo](https://github.com/Cruz-011/MOTOFACIL-APP)
> - [Simulador ESPs (Python)](https://github.com/Cruz-011/simuladorESPS)
