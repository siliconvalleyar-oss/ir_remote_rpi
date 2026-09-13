# ROADMAP — Plan de desarrollo

## Versión 1.x — Base (estado actual)

- [x] Emisor IR NEC 38 kHz por software (`bcm2835_delayMicroseconds`).
- [x] CLI: `--send`, `--interactive`, `--macro`, `--learn`, `--list`, `--version`.
- [x] Config editable (`config/*.cfg`).
- [x] OLED SSD1306 por I2C (opcional).
- [x] Compilación nativa, cruzada (32/64 bits) y remota.
- [x] Setup de repositorio Git y despliegue.

## 1.1 — Robustez del receptor

- [ ] Medición precisa con interrupciones (edge trigger) en lugar de polling.
- [ ] Soportar repetición y tramas de 13 bit LG (extended) en `--learn`.
- [ ] Guardar dirección aprendida por tecla (no solo comando).

## 1.2 — Motor

- [ ] PWM hardware (GPIO 18) como alternativa a la portadora por software.
- [ ] Envío de tramas con `carrier` definido en `ir_codes.cfg` (0x04/0x08).

## 2.0 — Producto

- [ ] Interfaz web (dashboard) sobre los scripts existentes.
- [ ] Programación de macros por horario (cron).
- [ ] Soporte multiprotocolo (Sony SIRC, RC5, RC6) detectado al aprender.

## En curso

- Validación sobre hardware real LG (POWER, volumen, input).
- Refinar tabla de códigos LG (colores, apps).