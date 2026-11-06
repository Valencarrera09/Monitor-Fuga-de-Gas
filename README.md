# 🛡️ GasGuard Pro v2.0 
### *Sistema Inteligente de Monitoreo Ambiental y Detección de Fugas de Gas* 🚀

<p align="center">
  <img src="https://img.shields.io/badge/STATUS-OPERATIVO_%F0%9F%94%A5-success?style=for-the-badge&logo=none" alt="Status">
  <img src="https://img.shields.io/badge/HARDWARE-ARDUINO-00979D?style=for-the-badge&logo=arduino&logoColor=white" alt="Arduino">
  <img src="https://img.shields.io/badge/FRONTEND-HTML5_%7C_CSS3_%7C_JS-orange?style=for-the-badge" alt="Tech">
  <img src="https://img.shields.io/badge/CONEXI%C3%93N-WEB_SERIAL_API-purple?style=for-the-badge" alt="WebSerial">
  <img src="https://img.shields.io/badge/ALERTAS-TELEGRAM-0088cc?style=for-the-badge&logo=telegram&logoColor=white" alt="Telegram">
  <img src="https://img.shields.io/badge/MONITOREO-100%25_OFFLINE-blue?style=for-the-badge" alt="Offline">
</p>

---

## 📖 Acerca del Proyecto

**GasGuard Pro** es una solución integral de seguridad ambiental desarrollada para la detección temprana de gases nocivos en espacios cerrados. Combina hardware de bajo costo (**Arduino** y sensor **MQ-135**) con una interfaz gráfica web moderna de alto rendimiento impulsada por la **Web Serial API**. 

Diseñado específicamente para operar de forma autónoma (ideal para entornos educativos como las netbooks del gobierno *Juana Manso*), el monitoreo funciona 100% offline: solo las notificaciones al celular usan conexión a internet. Ante una fuga, el sistema procesa alertas en tiempo real, activa alarmas sonoras/lumínicas, pone en marcha la purificación automática del ambiente (coolers + ventana con servo) y registra un historial completo con gráficos dinámicos.

---

## ✨ Características Principales

* **🔌 Conexión USB Directa (Nativa):** Comunicación bidireccional instantánea entre la placa y el navegador mediante la *Web Serial API* (compatible con Google Chrome y Microsoft Edge).
* **🚦 Sistema Tri-Estado de Alerta Inteligente:**
  * 🛡️ **Seguro:** Monitoreo activo con aire limpio y ambiente optimizado.
  * ⚠️ **Precaución:** Elevación moderada de parámetros; activación de protocolos preventivos y ventilación.
  * 🚨 **Peligro Crítico:** Disparador de sirena estilo ambulancia ("I-O"), luces intermitentes en placa y panel interactivo de evacuación.
* **🌀 Purificación Automática:** Ante detección de gas, un módulo relé enciende dos coolers de PC (AMD) que renuevan el aire de la habitación y un servo motor abre una ventana para expulsar el gas al exterior; al volver los valores a la normalidad, los coolers se apagan y la ventana se cierra solos.
* **📲 Alertas Remotas por Telegram:** Notificaciones push al celular mediante un bot propio: aviso inmediato de fuga, recordatorio cada 5 minutos mientras continúa y aviso cuando el aire se normaliza.
* **📈 Gráfico de Tendencias en Tiempo Real:** Renderizado dinámico de datos mediante HTML5 `<canvas>` sin librerías externas pesadas.
* **📜 Ventana Modal de Historial Completo:** Registro estructurado por fecha, hora exacta, valor en PPM y nivel de riesgo con opción de borrado seguro.
* **🏫 100% Offline-First:** Arquitectura basada en Emojis y estilos nativos para garantizar una carga instantánea y funcionamiento total sin red Wi-Fi (el monitoreo completo opera sin internet; únicamente las alertas por Telegram utilizan conexión).

---

## 🛠️ Stack Tecnológico y Hardware

| Capa | Componente / Tecnología | Descripción |
| :--- | :--- | :--- |
| **Microcontrolador** | Arduino (Uno / Nano / Mega) | Procesamiento de señales analógicas del sensor y control de actuadores. |
| **Sensor** | MQ-135 | Detección analógica de gases contaminantes y calidad del aire ($A0$). |
| **Actuadores** | Buzzer (3 pines) + LED Amarillo | Emisión de alerta acústica modulada y destellos lumínicos. |
| **Purificación** | 2× Coolers de PC (AMD) + Módulo Relé | Renuevan el aire de la habitación; el relé los enciende al detectar gas y los apaga al normalizarse. |
| **Extracción** | Servo Motor (SG90) | Abre una ventana para expulsar el gas al exterior y la cierra cuando el sensor deja de detectarlo. |
| **Interfaz Web** | HTML5, CSS3 (Variables, Grid/Flexbox) | Estructura responsiva y diseño futurista en modo oscuro. |
| **Lógica Frontend** | JavaScript (Vanilla / ES6+) | Gestión de streams seriales, gráficos y manejo de modales. |
| **Firmware** | `GasGuard/GasGuard.ino` (IDE de Arduino) | Lectura del MQ-135, sirena "I-O", relé de coolers y servo de la ventana. |

---

## ⚡ Esquema de Conexiones Físicas

```text
[Arduino UNO / Nano]
  ├── 5V  ─────────────── VCC (Sensor MQ-135, Buzzer & Servo)
  ├── GND ─────────────── GND (Sensor, Buzzer, Cátodo LED, Relé & Servo)
  ├── A0  ─────────────── AO  (Salida Analógica MQ-135)
  ├── D8  ─────────────── S   (Señal Buzzer)
  ├── D7  ─────────────── Resistencia 220Ω ── Ánodo LED Amarillo
  ├── D6  ─────────────── IN  (Módulo Relé → conmuta los 2 Coolers AMD a 12V)
  └── D9  ─────────────── Señal Servo SG90 (abre / cierra la ventana)
```

---

## 📲 Alertas Remotas por Telegram

El sistema envía notificaciones push al celular mediante un bot de Telegram (gratis y sin límite de mensajes):

* 🚨 **Fuga detectada:** alerta inmediata con la hora y el nivel del sensor.
* ⏰ **Recordatorio:** si la fuga continúa, reavisa cada 5 minutos.
* 🟢 **Normalización:** aviso cuando el aire vuelve a valores seguros.

**Configuración (una sola vez):**

1. Hablá con [@BotFather](https://t.me/BotFather) en Telegram, mandá `/newbot` y seguí los pasos → te devuelve el **token** del bot.
2. Abrí tu bot, mandale cualquier mensaje y entrá a `https://api.telegram.org/bot<TU_TOKEN>/getUpdates` → ahí aparece tu **chat_id**.
3. Pegá ambos valores en `ALERTA_TELEGRAM` al inicio de `java.js`.
4. Probá con el botón **"📲 Probar alerta Telegram"** del simulador de la página.
