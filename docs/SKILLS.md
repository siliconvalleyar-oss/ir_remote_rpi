# SKILLS — Habilidades y competencias del proyecto

## Competencias del equipo / desarrollador

Conocimientos necesarios para trabajar en IR-LG-Remote:

- **C++17**: RAII, `unique_ptr`, const-correctness, plantillas ligeras.
- **bcm2835**: GPIO (`fsel`, `write`, `lev`), retardos de microsegundos.
- **Protocolo NEC**: temporización, paridad, tramas de 8 y 13 bits.
- **Raspberry Pi hardware**: GPIO 3.3V, transistores NPN, pull-ups, I2C.
- **Linux**: `/dev/mem`, permisos de usuario, `i2cdetect`, SSH/bcmda y key-based auth.
- **Make**: patrones, detección de arquitectura, cross-compile.
- **Git**: conventional commits, tags de versión, ramas.

## Habilidades transversales

- Documentación técnica en español.
- Pruebas sobre hardware real (osciloscopio, logic analyzer, cámara IR).
- Automatización de despliegue (SSH, scp, cron).

## Perfil recomendado

- Entusiasta de sistemas embebidos con Raspberry Pi.
- Familiarizado con electrónica básica (transistor, LED, pull-up).
- Nivel medio de C++.

## Recursos de aprendizaje

- `docs/INFRARED.md` — introducción al protocolo NEC.
- `docs/HARDWARE.md` — electrónica del emisor/receptor.
- `docs/ARCHITECTURE.md` — cómo se conectan las capas.
- Ejemplos en `examples/` y código comentado en `src/`.