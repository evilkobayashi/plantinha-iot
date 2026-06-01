#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <BH1750.h>
#include <SPI.h>

// --- Pinos TFT ---
#define TFT_CS   D8
#define TFT_DC   D3
#define TFT_RST  D4
#define TFT_LED  D0

// --- Pino sensor de umidade do solo ---
#define SOIL_PIN A0

// --- Cor customizada ---
#define DARKGREY 0x4208

// --- Objetos ---
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
Adafruit_BMP280 bmp;
BH1750 lightMeter;

// --- Estados da planta ---
enum PlantState {
  HAPPY,
  OK,
  THIRSTY,
  FLOODED,
  HOT,
  SLEEPING
};

PlantState lastState = (PlantState)-1;

// --- Leituras ---
float temperature = 0;
float lux = 0;
int soilPercent = 0;

// -----------------------------------------------
// Desenha o rosto animado conforme o estado
// -----------------------------------------------
void drawFace(PlantState state) {
  tft.fillScreen(ST77XX_BLACK);

  // Fundo colorido por estado
  uint16_t bgColor;
  switch (state) {
    case HAPPY:   bgColor = ST77XX_BLACK; break;
    case OK:      bgColor = ST77XX_BLACK; break;
    case THIRSTY: bgColor = ST77XX_BLACK; break;
    case FLOODED: bgColor = ST77XX_BLACK; break;
    case HOT:     bgColor = ST77XX_BLACK; break;
    case SLEEPING:bgColor = ST77XX_BLACK; break;
  }

  int cx = 64;  // centro X (display 128px largura em landscape)
  int cy = 52;  // centro Y

  // Cabeça (círculo da planta)
  tft.fillCircle(cx, cy, 38, ST77XX_GREEN);
  tft.drawCircle(cx, cy, 38, ST77XX_WHITE);

  // Olhos — posição base
  int eyeLX = cx - 13;
  int eyeRX = cx + 13;
  int eyeY  = cy - 8;

  // Boca — posição base
  int mouthY = cy + 14;

  switch (state) {
    case HAPPY:
      // Olhos felizes (semicírculo)
      tft.fillCircle(eyeLX, eyeY, 6, ST77XX_BLACK);
      tft.fillCircle(eyeRX, eyeY, 6, ST77XX_BLACK);
      tft.fillCircle(eyeLX, eyeY + 3, 5, ST77XX_GREEN);
      tft.fillCircle(eyeRX, eyeY + 3, 5, ST77XX_GREEN);
      // Boca sorriso
      for (int i = -10; i <= 10; i++) {
        tft.drawPixel(cx + i, mouthY + (i * i) / 20, ST77XX_BLACK);
        tft.drawPixel(cx + i, mouthY + (i * i) / 20 + 1, ST77XX_BLACK);
      }
      break;

    case OK:
      // Olhos normais
      tft.fillCircle(eyeLX, eyeY, 5, ST77XX_BLACK);
      tft.fillCircle(eyeRX, eyeY, 5, ST77XX_BLACK);
      tft.fillCircle(eyeLX - 1, eyeY - 1, 2, ST77XX_WHITE);
      tft.fillCircle(eyeRX - 1, eyeY - 1, 2, ST77XX_WHITE);
      // Boca reta
      tft.drawFastHLine(cx - 10, mouthY, 20, ST77XX_BLACK);
      tft.drawFastHLine(cx - 10, mouthY + 1, 20, ST77XX_BLACK);
      break;

    case THIRSTY:
      // Olhos semicerrados
      tft.fillCircle(eyeLX, eyeY, 5, ST77XX_BLACK);
      tft.fillCircle(eyeRX, eyeY, 5, ST77XX_BLACK);
      tft.fillRect(eyeLX - 6, eyeY - 6, 12, 6, ST77XX_GREEN);
      tft.fillRect(eyeRX - 6, eyeY - 6, 12, 6, ST77XX_GREEN);
      // Boca triste
      for (int i = -10; i <= 10; i++) {
        tft.drawPixel(cx + i, mouthY - (i * i) / 20 + 4, ST77XX_BLACK);
        tft.drawPixel(cx + i, mouthY - (i * i) / 20 + 5, ST77XX_BLACK);
      }
      // Gotinha de suor
      tft.fillCircle(cx + 28, cy - 15, 4, ST77XX_CYAN);
      tft.fillTriangle(cx + 28, cy - 22, cx + 24, cy - 15, cx + 32, cy - 15, ST77XX_CYAN);
      break;

    case FLOODED:
      // Olhos assustados (grandes)
      tft.fillCircle(eyeLX, eyeY, 7, ST77XX_WHITE);
      tft.fillCircle(eyeRX, eyeY, 7, ST77XX_WHITE);
      tft.fillCircle(eyeLX, eyeY, 4, ST77XX_BLACK);
      tft.fillCircle(eyeRX, eyeY, 4, ST77XX_BLACK);
      // Boca aberta
      tft.fillCircle(cx, mouthY, 8, ST77XX_BLACK);
      tft.fillCircle(cx, mouthY + 2, 6, ST77XX_RED);
      break;

    case HOT:
      // Olhos cansados
      tft.fillCircle(eyeLX, eyeY, 5, ST77XX_BLACK);
      tft.fillCircle(eyeRX, eyeY, 5, ST77XX_BLACK);
      tft.drawFastHLine(eyeLX - 6, eyeY - 2, 12, ST77XX_RED);
      tft.drawFastHLine(eyeRX - 6, eyeY - 2, 12, ST77XX_RED);
      // Boca aberta cansada
      tft.fillRoundRect(cx - 8, mouthY - 4, 16, 10, 4, ST77XX_BLACK);
      // Chamas no topo
      tft.fillCircle(cx - 10, cy - 42, 5, ST77XX_RED);
      tft.fillCircle(cx,      cy - 44, 5, ST77XX_ORANGE);
      tft.fillCircle(cx + 10, cy - 42, 5, ST77XX_RED);
      break;

    case SLEEPING:
      // Olhos fechados (linhas)
      tft.drawFastHLine(eyeLX - 5, eyeY, 10, ST77XX_BLACK);
      tft.drawFastHLine(eyeLX - 5, eyeY + 1, 10, ST77XX_BLACK);
      tft.drawFastHLine(eyeRX - 5, eyeY, 10, ST77XX_BLACK);
      tft.drawFastHLine(eyeRX - 5, eyeY + 1, 10, ST77XX_BLACK);
      // Boca reta suave
      tft.drawFastHLine(cx - 7, mouthY, 14, ST77XX_BLACK);
      // ZZZ
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(1);
      tft.setCursor(cx + 22, cy - 25);
      tft.print("z");
      tft.setCursor(cx + 26, cy - 33);
      tft.print("Z");
      tft.setCursor(cx + 30, cy - 42);
      tft.print("Z");
      break;
  }

  // --- Mensagem de status (parte inferior) ---
  tft.setTextSize(1);
  tft.setCursor(0, 105);

  switch (state) {
    case HAPPY:
      tft.setTextColor(ST77XX_GREEN);
      tft.print("  Estou otima! :)");
      break;
    case OK:
      tft.setTextColor(ST77XX_YELLOW);
      tft.print("  Tudo bem por aqui");
      break;
    case THIRSTY:
      tft.setTextColor(ST77XX_CYAN);
      tft.print("  Me rega, por favor!");
      break;
    case FLOODED:
      tft.setTextColor(ST77XX_BLUE);
      tft.print("  Agua demais! Socorro");
      break;
    case HOT:
      tft.setTextColor(ST77XX_RED);
      tft.print("  Muito calor aqui...");
      break;
    case SLEEPING:
      tft.setTextColor(DARKGREY);
      tft.print("  Boa noite... zzz");
      break;
  }

  // Linha separadora
  tft.drawFastHLine(0, 100, 160, DARKGREY);

  // Mini indicadores no rodapé
  tft.setTextSize(1);
  tft.setTextColor(DARKGREY);
  tft.setCursor(0, 116);
  tft.print("S:");
  tft.setTextColor(ST77XX_WHITE);
  tft.print(soilPercent);
  tft.print("%");

  tft.setTextColor(DARKGREY);
  tft.setCursor(50, 116);
  tft.print("T:");
  tft.setTextColor(ST77XX_WHITE);
  tft.print((int)temperature);
  tft.print("C");

  tft.setTextColor(DARKGREY);
  tft.setCursor(95, 116);
  tft.print("L:");
  tft.setTextColor(ST77XX_WHITE);
  tft.print((int)lux);
  tft.print("lx");
}

// -----------------------------------------------
// Determina estado baseado nos sensores
// -----------------------------------------------
PlantState getState() {
  if (lux < 50)          return SLEEPING;
  if (temperature > 35)  return HOT;
  if (soilPercent > 85)  return FLOODED;
  if (soilPercent < 20)  return THIRSTY;
  if (soilPercent >= 40 && soilPercent <= 70 && temperature < 30) return HAPPY;
  return OK;
}

// -----------------------------------------------
void setup() {
  Serial.begin(115200);

  // Liga backlight do TFT
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);

  // Inicia TFT
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);  // Landscape: 160x128
  tft.fillScreen(ST77XX_BLACK);

  // Inicia I2C
  Wire.begin(D2, D1);  // SDA, SCL

  // BMP280
  if (!bmp.begin(0x76)) {
    tft.setTextColor(ST77XX_RED);
    tft.setCursor(10, 50);
    tft.print("BMP280 nao encontrado");
    while (1);
  }

  // BH1750
  lightMeter.begin();

  Serial.println("Plantinha IoT iniciada!");
}

// -----------------------------------------------
void loop() {
  // Leituras dos sensores
  temperature  = bmp.readTemperature();
  lux          = lightMeter.readLightLevel();
  int soilRaw  = analogRead(SOIL_PIN);
  soilPercent  = map(soilRaw, 1023, 0, 0, 100);
  soilPercent  = constrain(soilPercent, 0, 100);

  Serial.printf("Temp: %.1f C | Luz: %.0f lx | Solo: %d%%\n",
                temperature, lux, soilPercent);

  PlantState currentState = getState();

  // Só redesenha se o estado mudou
  if (currentState != lastState) {
    drawFace(currentState);
    lastState = currentState;
  }

  delay(2000);
}
