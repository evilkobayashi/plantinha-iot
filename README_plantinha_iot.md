# 🌱 Plantinha IoT

Monitor de saúde de plantas com interface de rosto animado que reage em tempo real aos sensores.

---

## Sobre o projeto

A Plantinha IoT é um dispositivo embarcado que monitora as condições de uma planta e exibe seu "humor" num display TFT 1.8" através de um rosto animado expressivo. Em vez de exibir números brutos, o sistema interpreta os dados dos sensores e comunica o estado da planta de forma intuitiva — ideal para quem não quer ficar verificando valores técnicos.

---

## Demonstração

| Estado | Condição | Expressão |
|--------|----------|-----------|
| 😄 Happy | Solo 40–70%, temp < 30°C, luz OK | Rosto sorridente |
| 😐 Ok | Condições aceitáveis | Rosto neutro |
| 😟 Thirsty | Umidade do solo < 20% | Rosto triste + gotinha |
| 😱 Flooded | Umidade do solo > 85% | Rosto assustado |
| 🥵 Hot | Temperatura > 35°C | Olhos cansados + chamas |
| 😴 Sleeping | Luminosidade < 50 lux | Olhos fechados + ZZZ |

---

## Hardware

| Componente | Função | Pino |
|---|---|---|
| NodeMCU ESP8266 | Microcontrolador | — |
| TFT 1.8" ST7735 (8 pinos) | Display | CS=D8, DC=D3, RST=D4, LED=D0 |
| BMP280 | Temperatura e pressão | I2C: SDA=D2, SCL=D1 |
| BH1750 | Sensor de luminosidade | I2C: SDA=D2, SCL=D1 |
| Sensor capacitivo de solo | Umidade do solo | A0 |

---

## Esquema de ligação

```
NodeMCU         TFT 1.8"
D8      ──────  CS
D3      ──────  DC (A0)
D4      ──────  RST
D0      ──────  LED (backlight)
D7      ──────  SDA (MOSI)
D5      ──────  SCK
3.3V    ──────  VCC
GND     ──────  GND

NodeMCU         BMP280 / BH1750 (I2C compartilhado)
D2      ──────  SDA
D1      ──────  SCL
3.3V    ──────  VCC
GND     ──────  GND

NodeMCU         Sensor de solo
A0      ──────  AOUT
3.3V    ──────  VCC
GND     ──────  GND
```

---

## Bibliotecas necessárias

Instale pela Arduino IDE (Sketch → Include Library → Manage Libraries):

```
Adafruit ST7735 and ST7789 Library
Adafruit GFX Library
Adafruit BMP280 Library
BH1750 (by Christopher Laws)
```

---

## Lógica de estados

```cpp
SLEEPING  →  lux < 50
HOT       →  temperatura > 35°C
FLOODED   →  solo > 85%
THIRSTY   →  solo < 20%
HAPPY     →  solo 40–70% e temp < 30°C
OK        →  demais casos
```

O display só é redesenhado quando o estado muda, evitando flickering.

---

## Detalhes técnicos importantes

```cpp
// A biblioteca ST7735 não possui ST77XX_DARKGREY — defina manualmente:
#define DARKGREY 0x4208

// O backlight precisa ser ativado antes do initR():
pinMode(TFT_LED, OUTPUT);
digitalWrite(TFT_LED, HIGH);

// Orientação landscape (160x128px):
tft.setRotation(1);

// Endereço I2C do BMP280:
bmp.begin(0x76);
```

---

## Como usar

1. Clone este repositório
2. Abra `plantinha_iot.ino` na Arduino IDE
3. Instale as bibliotecas listadas acima
4. Selecione a placa **NodeMCU 1.0 (ESP-12E Module)**
5. Faça as ligações conforme o esquema
6. Faça o upload e abra o Serial Monitor (115200 baud) para acompanhar as leituras

---

## Stack

![skills](https://skillicons.dev/icons?i=cpp,arduino&theme=dark)

`ESP8266` `C++` `I2C` `SPI` `Embedded Systems`

---

## Autor

**André Veiga** — [@evilkobayashi](https://github.com/evilkobayashi)  
Professor de robótica e desenvolvimento em Queimados, RJ.  
📧 andreveiga.eng@gmail.com
