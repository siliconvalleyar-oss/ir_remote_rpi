# PROMPT — Requisitos base del proyecto

Este documento es la referencia de especificación del proyecto IR-LG-Remote,
según el prompt original (`prompt.md` en la raíz del repositorio).

## Resumen del proyecto

Emulador de control remoto **LG** por infrarrojos sobre **Raspberry Pi**,
escrito en **C++** con **bcm2835**. Compatible con Raspberry Pi OS de 32 y 64
bits. Compilable con `make`.

## Requisitos funcionales

1. Emitir comandos IR **NEC** (portadora 38 kHz por software con
   `nanosleep`/`bcm2835_delayMicroseconds`).
2. Reproducir el teclado LG: `POWER`, `VOLUME_UP/DOWN`, `MUTE`,
   `CHANNEL_UP/DOWN`, `INPUT`, `MENU`, `OK`, flechas, dígitos 0-9.
3. Cargar códigos desde `config/ir_codes.cfg` (editar sin recompilar).
4. Modos: `--send <TECLA>`, `--interactive`, `--macro <nombre>`.
5. (Opcional) receptor VS1838B en GPIO 17 para `--learn`.
6. (Opcional) OLED SSD1306 por I2C.

## Formato obligatorio de `main.cpp`

```cpp
#include <memory>
#include "core/Device_t.hpp"

int main(int argc, char** argv) {
    auto device = std::make_unique<Device::Device_t>(argc, argv);
    device->run();
    return 0;
}
```

## Versión en tiempo de compilación

- No se lee `VERSION` en ejecución: el Makefile inyecta `-DVERSION="$(VERSION)"`.
- Se muestra en el banner y en `--version`.

## Protocolo NEC (referencia)

- Portadora 38 kHz (periodo ~26.3 µs: 13 µs ON + 13 µs OFF).
- Inicio: 9000 µs + 4500 µs.
- Bit 0: 560 µs + 560 µs. Bit 1: 560 µs + 1690 µs.
- Parada: 560 µs.
- 32 bits LSB-first: `addr | ~addr | cmd | ~cmd`.
- LG: dirección 0x04; POWER=0x08, VOLUME_UP=0x02, etc.

## Estructura obligatoria

Ver el árbol completo en `prompt.md` (raíz). Resumen: `bin/`, `config/`,
`docs/`, `examples/`, `include/`, `obj/`, `scripts/`, `src/`, `Makefile`,
`VERSION`, `LICENSE`, `README.md`, `generate_basic_src.sh`.

## Entregable

- Código fuente + cabeceras + scripts + documentación completos.
- Compila con `make` sin errores.
- Configuración de repositorio Git interactiva (`scripts/setup_git.sh`).