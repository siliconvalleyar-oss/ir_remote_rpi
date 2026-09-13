# DESIGN (DESING) — Decisiones de diseño del emulador IR

Nota: el archivo conserva el nombre `DESING.md` por compatibilidad con la
especificación del proyecto.

## Objetivos

1. Emular un mando LG (NEC) con un LED IR en un GPIO.
2. Códigos editables sin recompilar.
3. Compilable en Raspberry Pi de 32 y 64 bits, y en cruz desde x86.
4. Código sencillo, RAII y comentado.

## Decisiones clave

### Portadora por software vs hardware

**Decisión:** portadora de 38 kHz por software (`bcm2835_gpio_write` +
`bcm2835_delayMicroseconds`, ciclo 13+13 µs).

**Razón:** sin hardware adicional (PWM 18 o SPI), funciona en cualquier GPIO.
La tolerancia ±20 % del receptor hace innecesario el PWM hardware en esta
versión.

### Ciclo de vida de bcm2835

**Decisión:** el `IR_Transmitter` es propietario de `bcm2835_init/close`.
En `--learn`, el transmisor inicia bcm2835 y el receptor lo asume.

**Razón:** centralizar la inicialización evita dobes `close()` y simplifica el
.receptor (que solo lee pines).

### Configuración por archivos de texto plano

**Decisión:** `HARDWARE=valor` / `TECLA=0xNN` en `config/*.cfg`.

**Razón:** sencillez; sin dependencia externa de parseo. Los valores por
defecto del código se reutilizan si el archivo falta (modo degradado).

### Versión en tiempo de compilación

**Decisión:** `VERSION` → `-DVERSION="..."` en el Makefile; nunca se lee el
archivo en ejecución.

**Razón:** coherencia con `--version` sin depender del cwd.

### OLED desacoplada

**Decisión:** OLED opcional vía `unique_ptr`; si `OLEDinit()` falla, la app
continúa sin pantalla.

**Razón:** el fallo de un periférico no debe bloquear el mando IR.

## Alternativas evaluadas

| Alternativa | Veredicto |
|---|---|
| PWM 18 por hardware para portadora | Futuro (`ROADMAP`) |
| Receptor por interrupción (edge) | Pendiente de mejorar |
| JSON para config (nlohmann disponible en `include/`) | No necesario |
| ROC/otros protocolos | Fuera de alcance inicial |