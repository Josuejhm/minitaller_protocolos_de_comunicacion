#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ── Configuración ──────────────────────────────────────
#define SLAVE_ADDRESS 0x09

// Pines de los LEDs
const int LEDS[] = {3, 4, 5, 6, 7};
const int NUM_LEDS = 5;

// Umbrales de temperatura (°C)
const float UMBRALES[] = {15.0, 20.0, 25.0, 30.0};

// LCD en dirección 0x20, 16 columnas, 2 filas
LiquidCrystal_I2C lcd(0x20, 16, 2);

// ── Variables globales ─────────────────────────────────
float temperatura = 0.0;
bool datoNuevo = false;

// ── Setup ──────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  // Iniciar I2C como Slave PRIMERO
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(recibirTemperatura); // Callback cuando Master envía dato

  // Iniciar LEDs
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LEDS[i], OUTPUT);
    digitalWrite(LEDS[i], LOW);
  }

  // Iniciar LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Esperando...");

  Serial.println("Slave 2 listo");
}

// ── Loop ───────────────────────────────────────────────
void loop() {
  if (datoNuevo) {
    datoNuevo = false;
    actualizarLCD();
    actualizarLEDs();
  }
}

// ── Callback I2C ───────────────────────────────────────
void recibirTemperatura(int bytes) {
  if (bytes == 4) {
    byte datos[4];
    for (int i = 0; i < 4; i++) {
      datos[i] = Wire.read();
    }
    memcpy(&temperatura, datos, 4); // Reconstruimos el float
    datoNuevo = true;
  }
}

// ── Actualizar LCD ─────────────────────────────────────
void actualizarLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperatura:");
  lcd.setCursor(0, 1);
  lcd.print(temperatura, 1); // 1 decimal
  lcd.print(" C");

  Serial.print("LCD actualizada: ");
  Serial.println(temperatura);
}

// ── Actualizar LEDs ────────────────────────────────────
void actualizarLEDs() {
  // Determinamos cuántos LEDs encender
  int ledsEncendidos = 1; // Siempre al menos uno

  for (int i = 0; i < NUM_LEDS - 1; i++) {
    if (temperatura >= UMBRALES[i]) {
      ledsEncendidos = i + 2;
    }
  }

  // Encendemos/apagamos según corresponda
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LEDS[i], i < ledsEncendidos ? HIGH : LOW);
  }

  Serial.print("LEDs encendidos: ");
  Serial.println(ledsEncendidos);
}