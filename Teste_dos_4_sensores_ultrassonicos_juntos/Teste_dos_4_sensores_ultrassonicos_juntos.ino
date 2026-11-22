// Teste dos 4 sensores ultrassónicos - SmartPark
// Projeto I

#define NUM_LUGARES 4
#define DIST_OCUPADO 5   // abaixo disto há carro (ajustar na maquete)

// pinos de cada sensor (posição 0 = lugar 1, posição 1 = lugar 2, ...)
int trigPins[NUM_LUGARES] = {18, 20, 22, 2};
int echoPins[NUM_LUGARES] = {19, 21, 23, 3};

long distancias[NUM_LUGARES];   // última distância de cada lugar
bool ocupado[NUM_LUGARES];      // true = ocupado, false = livre

int sensorAtual = 0;            // qual o sensor que vamos medir a seguir
unsigned long ultimaLeitura = 0;

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
}

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < NUM_LUGARES; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }

  Serial.println("Teste dos 4 sensores");
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