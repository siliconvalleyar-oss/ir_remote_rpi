# CHANGELOG

Todos los cambios relevantes del proyecto IR-LG-Remote.

El formato se basa en [Keep a Changelog](https://keepachangelog.com/) y el
versionado sigue semver estricto con el ciclo patch 0-9 definido en
`docs/LEARNINGS.md` (tag = VERSION).

## [1.0.0] — 2026-09-13

### Añadido

- Emulador de control remoto LG por IR (NEC, 38 kHz) sobre Raspberry Pi con
  `bcm2835`.
- Motor NEC por software: start frame, bits LSB-first, paridad y stop bit.
- Driver emisor IR (`IR_Transmitter`) en GPIO 22 (LED + transistor NPN).
- Driver receptor IR (`IR_Receiver`, VS1838B en GPIO 17) con modo `--learn`.
- Modos CLI: `--send`, `--interactive`, `--macro`, `--learn`, `--list`,
  `--version`, `--help`.
- Tabla de códigos LG en `config/ir_codes.cfg` (36 teclas), editable sin
  recompilar.
- Config de hardware (`config/hardware.cfg`) y general/macros
  (`config/config.cfg`).
- Pantalla OLED SSD1306 por I2C (ioctl `/dev/i2c-N`), opcional.
- Versión en tiempo de compilación mediante `-DVERSION="$(cat VERSION)"`.
- Makefile con objetivos `all`, `clean`, `distclean`, `fetch`, `crossover*`,
  `deploy`, `remote`, `run`, `info`.
- Scripts: `install_deps.sh`, `setup_git.sh`, `fetch_sysroot.sh`,
  `deploy.sh`, `build_remote.sh`, `generate_basic_src.sh`.
- Documentación completa en `docs/` y `README.md`.
- Ejemplos de uso en `examples/` (bash y python).

### Notas

- Compatible con Raspberry Pi OS de 32 y 64 bits.
- La compilación cruzada desde x86 requiere `scripts/fetch_sysroot.sh`.