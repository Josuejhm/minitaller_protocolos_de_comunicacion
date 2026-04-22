#include <Wire.h>

// ── Configuración ──────────────────────────────────────
#define SLAVE_ADDRESS 0x08   // Dirección I2C de este slave
#define TMP36_PIN     A0     // Pin analógico del TMP36

// ── Variables globales ─────────────────────────────────
float temperatura = 0.0;
unsigned long ultimaLectura = 0;
const unsigned long INTERVALO = 5000; // 5 segundos

// ── Setup ──────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  Wire.begin(SLAVE_ADDRESS);         // Inicia I2C como Slave
  Wire.onRequest(enviarTemperatura); // Callback cuando Master pide dato

  Serial.println("Slave 1 listo");
}

// ── Loop ───────────────────────────────────────────────
void loop() {
  unsigned long ahora = millis();

  if (ahora - ultimaLectura >= INTERVALO) {
    ultimaLectura = ahora;

    // Leemos el valor analógico (0-1023) y lo convertimos a voltaje
    int lecturaCruda = analogRead(TMP36_PIN);
    float voltaje = lecturaCruda * (5.0 / 1023.0); // Convertir a voltios
    float lectura = (voltaje * 1000 - 500) / 10.0; // Convertir a °C

    temperatura = lectura;
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" °C");
  }
}

// ── Callback I2C ───────────────────────────────────────
void enviarTemperatura() {
  // Convertimos el float a 4 bytes y los enviamos
  byte datos[4];
  memcpy(datos, &temperatura, 4);
  Wire.write(datos, 4);
}