# ACTIVITY — Registro de actividad del proyecto

Bitácora de hitos y cambios relevantes del proyecto IR-LG-Remote.

## 2026-09-13 — v1.0.0 (commit inicial)

- Clonada la base remota `siliconvalleyar-oss/ir_remote_rpi` (LICENSE MIT +
  VERSION 1.0.0) y reconstruido el proyecto completo sobre ella.
- Generado el emulador IR-LG-Remote:
  - Motor NEC 38 kHz por software (`IR_Engine`).
  - Drivers emisor (GPIO 22) y receptor (GPIO 17, VS1838B).
  - `Device_t` con CLI (`--send`, `--interactive`, `--macro`, `--learn`,
    `--list`, `--version`).
  - Pantalla OLED SSD1306 por I2C (ioctl).
  - Config `config/*.cfg` y 36 teclas LG.
- Scripts: instalación de dependencias, setup Git, sysroot cruzado, despliegue
  y build remoto.
- Documentación completa (`docs/*.md`, `README.md`).
- Verificación: compilación de todos los `.cpp` sin errores (`-Wall -Wextra`),
  smoke tests de `--version`, `--list` y ayuda.

## Pendiente

- Validación sobre TV LG real (hardware).
- Ajuste de códigos según modelo concreto (ver `docs/ROADMAP.md`).