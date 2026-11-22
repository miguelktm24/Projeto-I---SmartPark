// Teste de 1 sensor ultrassónico HC-SR04 no ESP32
// SmartPark - Projeto I

#define TRIG_PIN 18
#define ECHO_PIN 19

#define DIST_OCUPADO 5   // abaixo de 5 cm consideramos que há carro (ajustar na maquete)

unsigned long ultimaLeitura = 0;

long medirDistancia() {
  // garantir que o trig começa em LOW
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // pulso de 10 us para o sensor disparar o som
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // mede quanto tempo o echo fica em HIGH (em microssegundos)
  // o 30000 é um limite: se não vier eco em 30 ms, desiste e devolve 0
  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duracao == 0) {
    return -1;  // sem eco (nada à frente ou erro)
  }

  // o som vai e volta, por isso divide-se por 58 para ter cm
  return duracao / 58;
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.println("Teste do sensor ultrassonico");
}

void loop() {
  // medir de 300 em 300 ms, com millis() em vez de delay()
  if (millis() - ultimaLeitura >= 300) {
    ultimaLeitura = millis();

    long dist = medirDistancia();

    if (dist == -1) {
      Serial.println("Sem leitura");
    } else {
      Serial.print("Distancia: ");
      Serial.print(dist);
      Serial.print(" cm -> ");

      if (dist < DIST_OCUPADO) {
        Serial.println("OCUPADO");
      } else {
        Serial.println("LIVRE");
      }
    }
  }
}
