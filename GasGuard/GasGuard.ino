/*
  ╔══════════════════════════════════════════════════════════════════╗
  ║   GasGuard Pro v2.0 — Sketch principal (Arduino UNO / Nano)      ║
  ║   Sensor MQ-135 + Buzzer + LED + Relé (coolers) + Servo (ventana)║
  ╚══════════════════════════════════════════════════════════════════╝

  Comportamiento:
   • Cada 1 segundo mide el MQ-135 y manda el valor por USB (9600 baudios).
     La página web (java.js) lee ese número y dibuja todo el panel.
   • valor > umbralGas  →  PELIGRO: sirena "I-O", LED intermitente,
     el relé enciende los 2 coolers AMD y el servo ABRE la ventana.
   • valor entre umbralCierre y umbralGas  →  PRECAUCIÓN: LED fijo, sin sirena.
   • valor <= umbralCierre sostenido 10 segundos  →  TRANQUI: se apagan los
     coolers, el servo CIERRA la ventana y el LED se apaga.

  Importante: umbralGas debe ser IGUAL a UMBRAL en java.js (400).

  Nota de alimentación: los coolers AMD van a una fuente de 12V que conmuta
  el relé; el Arduino solo maneja la señal IN del módulo relé.
*/

#include <Servo.h>

// ===== Pines =====
const int PIN_SENSOR = A0;  // AO del MQ-135
const int PIN_BUZZER = 8;   // Señal del buzzer
const int PIN_LED    = 7;   // LED amarillo (con resistencia de 220Ω)
const int PIN_RELE   = 6;   // IN del módulo relé (coolers AMD)
const int PIN_SERVO  = 9;   // Señal del servo SG90 (ventana)

// ===== Umbrales =====
const int umbralGas    = 400;               // ← mismo valor que UMBRAL en java.js
const int umbralCierre = umbralGas * 3 / 4; // 300: recién acá se considera "purificado"

// ===== Relé =====
// La mayoría de los módulos relé se ACTIVAN con LOW.
// Si los coolers te quedan encendidos al revés, cambiá true por false.
const bool RELE_ACTIVO_BAJO = true;

// ===== Ventana (servo) =====
const int ANGULO_CERRADA = 0;
const int ANGULO_ABIERTA = 90;

// ===== Tiempos =====
const unsigned long INTERVALO_LECTURA_MS = 1000;  // medir cada 1 segundo
const unsigned long PURIFICADO_MS        = 10000; // 10 s de aire limpio antes de cerrar todo
const unsigned long SIRENA_CAMBIO_MS     = 150;   // ritmo del "I-O" de la sirena
const unsigned long LED_PARPADEO_MS      = 250;   // ritmo del LED en peligro

Servo servoVentana;

bool enPeligro         = false;  // ¿hay fuga activa?
bool coolersEncendidos = false;
bool ventanaAbierta    = false;
bool sirenaAguda       = false;  // alterna entre tono grave y agudo
int  ultimoValor       = 0;      // última lectura del sensor

unsigned long tUltimaLectura = 0;
unsigned long tSirena = 0;
unsigned long tLed    = 0;
unsigned long tLimpio = 0;  // desde cuándo el aire está limpio (para cerrar)

void setup() {
  Serial.begin(9600);                 // java.js abre el puerto a 9600 baudios
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_RELE, OUTPUT);
  releOff();                          // coolers apagados al arrancar
  servoVentana.attach(PIN_SERVO);
  servoVentana.write(ANGULO_CERRADA); // ventana cerrada al arrancar
}

void loop() {
  actualizarSirena();   // corre siempre, para que la sirena suene continua
  actualizarLed();

  if (millis() - tUltimaLectura < INTERVALO_LECTURA_MS) return;
  tUltimaLectura = millis();

  ultimoValor = analogRead(PIN_SENSOR);
  Serial.println(ultimoValor);   // la web toma el primer número de la línea

  procesarNivel(ultimoValor);
}

// ===== Lógica de estados =====
void procesarNivel(int valor) {
  bool peligroAhora = (valor > umbralGas);

  // Arranca la fuga: abrir ventana + encender coolers
  if (peligroAhora && !enPeligro) {
    enPeligro = true;
    tLimpio = 0;
    abrirVentana();
    encenderCoolers();
  }

  // La fuga cede: exigir aire limpio sostenido antes de cerrar todo
  if (!peligroAhora && enPeligro) {
    if (valor <= umbralCierre) {
      if (tLimpio == 0) tLimpio = millis();
      if (millis() - tLimpio >= PURIFICADO_MS) {
        enPeligro = false;
        tLimpio = 0;
        cerrarVentana();
        apagarCoolers();
      }
    } else {
      tLimpio = 0;  // todavía no llega a "purificado"
    }
  }
}

// ===== Actuadores =====
void abrirVentana() {
  if (ventanaAbierta) return;
  servoVentana.write(ANGULO_ABIERTA);
  ventanaAbierta = true;
}

void cerrarVentana() {
  if (!ventanaAbierta) return;
  servoVentana.write(ANGULO_CERRADA);
  ventanaAbierta = false;
}

void encenderCoolers() {
  if (coolersEncendidos) return;
  releOn();
  coolersEncendidos = true;
}

void apagarCoolers() {
  if (!coolersEncendidos) return;
  releOff();
  coolersEncendidos = false;
}

void releOn()  { digitalWrite(PIN_RELE, RELE_ACTIVO_BAJO ? LOW  : HIGH); }
void releOff() { digitalWrite(PIN_RELE, RELE_ACTIVO_BAJO ? HIGH : LOW);  }

// ===== Sirena estilo ambulancia "I-O" (no bloqueante) =====
void actualizarSirena() {
  static bool sirenaPrendida = false;

  if (enPeligro) {
    if (millis() - tSirena >= SIRENA_CAMBIO_MS) {
      tSirena = millis();
      sirenaAguda = !sirenaAguda;
      tone(PIN_BUZZER, sirenaAguda ? 1000 : 600);
      sirenaPrendida = true;
    }
  } else if (sirenaPrendida) {
    noTone(PIN_BUZZER);
    sirenaPrendida = false;
  }
}

// ===== LED: intermitente en peligro, fijo en precaución, apagado en tranqui =====
void actualizarLed() {
  if (enPeligro) {
    digitalWrite(PIN_LED, (millis() - tLed >= LED_PARPADEO_MS) ? HIGH : LOW);
    if (millis() - tLed >= LED_PARPADEO_MS) tLed = millis();
  } else if (ultimoValor >= umbralCierre) {
    digitalWrite(PIN_LED, HIGH);   // precaución: LED fijo
    tLed = millis();
  } else {
    digitalWrite(PIN_LED, LOW);
    tLed = millis();
  }
}
