#include <Arduino.h>
#include "pla_logic.h"

// UNO R3 / ATmega328P. D0 y D1 reservados para USB serie.
constexpr uint8_t DATA = 11, CLOCK = 13, LATCH = 10, ENABLE = 12;
constexpr uint8_t OUTPUTS[] = {2, 3, 4, 5, 6, 7, 8, 9};
constexpr unsigned int SETTLE_US = 50;
constexpr uint8_t MAX_DETAILS = 100;

void apply(uint16_t vector) {
  digitalWrite(LATCH, LOW);
  // U1 recibe SER del UNO; U1 QH' alimenta SER de U2.
  shiftOut(DATA, CLOCK, MSBFIRST, uint8_t(vector >> 8));
  shiftOut(DATA, CLOCK, MSBFIRST, uint8_t(vector));
  digitalWrite(LATCH, HIGH);
}

uint8_t sample() {
  uint8_t value = 0;
  for (uint8_t bit = 0; bit < 8; ++bit)
    value |= uint8_t(digitalRead(OUTPUTS[bit]) == HIGH) << bit;
  return value;
}

void printHex(uint16_t value, uint8_t digits) {
  for (int8_t shift = (digits - 1) * 4; shift >= 0; shift -= 4)
    Serial.print("0123456789ABCDEF"[(value >> shift) & 15]);
}

void runTest() {
  uint32_t tested = 0, failed = 0, unstable = 0;
  uint32_t errors[8] = {}, highs[8] = {};
  uint8_t details = 0;
  bool stopped = false;
  const uint32_t started = millis();
  Serial.println(F("C64 PLA: dos barridos, dos lecturas/vector; enviar x para detener."));
  Serial.println(F("Entradas: A13 A14 A15 /VA14 CHAREN HIRAM LORAM /CAS VA12 VA13 /GAME /EXROM RW /AEC BA A12"));
  Serial.println(F("Salidas: /CASRAM /BASIC /KERNAL /CHAROM /GRW /IO /ROML /ROMH"));
  digitalWrite(ENABLE, LOW);
  for (uint8_t pass = 0; pass < 2 && !stopped; ++pass) {
    // 32 bits: un contador uint16_t desbordaria y no terminaria.
    for (uint32_t step = 0; step < 65536UL; ++step) {
      while (Serial.available()) {
        if (Serial.read() == 'x') stopped = true;
      }
      if (stopped) break;
      const uint16_t vector = pass == 0 ? uint16_t(step) : uint16_t(65535UL - step);
      apply(vector);
      delayMicroseconds(SETTLE_US);
      const uint8_t first = sample();
      delayMicroseconds(SETTLE_US);
      const uint8_t second = sample(), wanted = expected(vector);
      const uint8_t diff = (first ^ wanted) | (second ^ wanted);
      ++tested;
      unstable += first != second;
      for (uint8_t bit = 0; bit < 8; ++bit) {
        errors[bit] += (diff >> bit) & 1;
        highs[bit] += (second >> bit) & 1;
      }
      if (diff) {
        ++failed;
        if (details < MAX_DETAILS) {
          ++details;
          Serial.print(F("FALLO pasada=")); Serial.print(pass + 1);
          Serial.print(F(" entrada=")); printHex(vector, 4);
          Serial.print(F(" esperado=")); printHex(wanted, 2);
          Serial.print(F(" lectura1=")); printHex(first, 2);
          Serial.print(F(" lectura2=")); printHex(second, 2);
          Serial.print(F(" xor=")); printHex(diff, 2); Serial.println();
        }
      }
      if ((tested % 8192UL) == 0) {
        Serial.print(F("Progreso ")); Serial.print(tested);
        Serial.print(F("/131072 fallidos=")); Serial.println(failed);
      }
    }
  }
  apply(0); // Mantener entradas definidas hasta la siguiente prueba.
  Serial.print(F("RESULTADO: "));
  if (stopped) Serial.println(F("INTERRUMPIDO"));
  else if (failed) Serial.println(F("FAIL"));
  else Serial.println(F("PASS_ESTATICO"));
  Serial.print(F("Vectores=")); Serial.print(tested);
  Serial.print(F(" fallidos=")); Serial.print(failed);
  Serial.print(F(" inestables=")); Serial.print(unstable);
  Serial.print(F(" tiempo_ms=")); Serial.println(millis() - started);
  for (uint8_t bit = 0; bit < 8; ++bit) {
    Serial.print(F("Salida bit=")); Serial.print(bit);
    Serial.print(F(" errores=")); Serial.print(errors[bit]);
    Serial.print(F(" bajas=")); Serial.print(tested - highs[bit]);
    Serial.print(F(" altas=")); Serial.println(highs[bit]);
  }
  Serial.println(F("Listo. Enviar t para otra prueba."));
}

void setup() {
  // Pull-up externo en /OE mantiene los 595 deshabilitados durante reset.
  digitalWrite(ENABLE, HIGH); pinMode(ENABLE, OUTPUT);
  digitalWrite(DATA, LOW); pinMode(DATA, OUTPUT);
  digitalWrite(CLOCK, LOW); pinMode(CLOCK, OUTPUT);
  digitalWrite(LATCH, LOW); pinMode(LATCH, OUTPUT);
  for (uint8_t pin : OUTPUTS) pinMode(pin, INPUT);
  apply(0);
  digitalWrite(ENABLE, LOW);
  Serial.begin(115200);
  Serial.println(F("C64 PLA listo. Enviar t para iniciar; x para detener."));
}

void loop() {
  if (Serial.available() && Serial.read() == 't') runTest();
}
