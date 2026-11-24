// SmartPark - Sensores ultrassónicos + LCD 16x2 (I2C)
// Projeto I
// LCD Keyestudio LCM1602 com módulo I2C (só 4 fios: GND, VCC, SDA, SCL)
// Biblioteca: "LiquidCrystal I2C" (Frank de Brabander) - instalar no Library Manager

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define NUM_LUGARES 4
#define DIST_OCUPADO 5   // abaixo disto há carro (ajustar na maquete)

// pinos I2C do LCD (no ESP32-C6 podemos escolher quaisquer GPIO livres)
#define LCD_SDA 6
#define LCD_SCL 7

// endereço I2C do LCD (normalmente 0x27; se não aparecer nada, experimentar 0x3F)
#define LCD_ENDERECO 0x27

LiquidCrystal_I2C lcd(LCD_ENDERECO, 16, 2);   // endereço, colunas, linhas

// pinos de cada sensor (posição 0 = lugar 1, posição 1 = lugar 2, ...)
int trigPins[NUM_LUGARES] = {18, 20, 22, 2};
int echoPins[NUM_LUGARES] = {19, 21, 23, 3};

long distancias[NUM_LUGARES];   // última distância de cada lugar
bool ocupado[NUM_LUGARES];      // true = ocupado, false = livre

int sensorAtual = 0;            // qual o sensor que vamos medir a seguir
unsigned long ultimaLeitura = 0;
int ultimosLivres = -1;         // último valor mostrado no LCD

long medirDistancia(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duracao = pulseIn(echo, HIGH, 30000);
  if (duracao == 0) {
    return -1;  // sem eco
  }
  return duracao / 58;
}

// atualiza o LCD só quando o número de lugares livres muda
void atualizarLCD(int livres) {
  if (livres == ultimosLivres) return;
  ultimosLivres = livres;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SmartPark");
  lcd.setCursor(0, 1);
  if (livres <= 0) {
    lcd.print("PARQUE CHEIO");
  } else {
    lcd.print("Livres: ");
    lcd.print(livres);
    lcd.print("/");
    lcd.print(NUM_LUGARES);
  }
}

void mostrarEstado() {
  int livres = 0;
  for (int i = 0; i < NUM_LUGARES; i++) {
    Serial.print("L");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(distancias[i]);
    Serial.print(" cm ");
    if (ocupado[i]) {
      Serial.print("OCUPADO | ");
    } else {
      Serial.print("LIVRE | ");
      livres++;
    }
  }
  Serial.print("Livres: ");
  Serial.print(livres);
  Serial.print("/");
  Serial.println(NUM_LUGARES);

  atualizarLCD(livres);   // mostra no LCD
}

void setup() {
  Serial.begin(9600);

  // primeiro dizemos ao ESP32 em que pinos está o I2C, só depois iniciamos o LCD
  Wire.begin(LCD_SDA, LCD_SCL);
  lcd.init();
  lcd.backlight();        // liga a luz de fundo

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SmartPark");
  lcd.setCursor(0, 1);
  lcd.print("A iniciar");

  for (int i = 0; i < NUM_LUGARES; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }

  Serial.println("SmartPark - sensores + LCD I2C");
}

void loop() {
  // mede um sensor de cada vez, de 70 em 70 ms
  if (millis() - ultimaLeitura >= 70) {
    ultimaLeitura = millis();

    long d = medirDistancia(trigPins[sensorAtual], echoPins[sensorAtual]);
    distancias[sensorAtual] = d;

    // -1 (sem eco) quer dizer que não há nada perto, logo está livre
    if (d != -1 && d < DIST_OCUPADO) {
      ocupado[sensorAtual] = true;
    } else {
      ocupado[sensorAtual] = false;
    }

    sensorAtual++;

    // depois de medir os 4, mostra tudo e volta ao primeiro
    if (sensorAtual == NUM_LUGARES) {
      sensorAtual = 0;
      mostrarEstado();
    }
  }
}
