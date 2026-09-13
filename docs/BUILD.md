# BUILD — Compilación del proyecto

## Requisitos

- `g++` (C++17), `make`, librería `bcm2835` (`/usr/local/include/bcm2835.h` y
  `/usr/local/lib/libbcm2835.a`).
- Instalación automática: `sudo bash scripts/install_deps.sh`.

## Compilación nativa (en la Raspberry Pi)

```bash
make clean && make -j4
```

Resultado: `bin/App`. La versión se lee de `VERSION` y se inyecta con
`-DVERSION="..."`.

## Compilación cruzada (desde un PC x86)

Se necesita el sysroot con `bcm2835` de la Pi:

```bash
bash scripts/fetch_sysroot.sh             # descarga bcm2835 de pi@raspi.local
make crossover64                          # ARM 64-bit (aarch64-linux-gnu)
make crossover32                          # ARM 32-bit (arm-linux-gnueabihf)
make crossover                            # detecta arquitectura automáticamente
```

El Makefile detecta la arquitectura del host y cruza al tamaño opuesto
(`x86_64` → ARM64; `aarch64` → ARM32).

## Objetivos del Makefile

| Objetivo | Descripción |
|---|---|
| `all` | Compila `bin/App` |
| `clean` | Elimina `obj/` y `bin/App` |
| `distclean` | `clean` + elimina `sysroot/` |
| `fetch` / `sysroot` | Descarga sysroot con bcm2835 de la Pi |
| `crossover` / `crossover32` / `crossover64` | Compilación cruzada |
| `run` / `runnosudo` | Ejecuta la app (con/sin sudo), `ARGS="..."` |
| `deploy` | `scp` del binario a la Pi |
| `remote` | git pull + make + run en la Pi |
| `info` | Muestra VERSION, ARCH, compilador y objetos |

## Verificación

- `make run ARGS="--version"` → muestra `IR-LG-Remote v1.0.0`.
- `make run ARGS="--list"` → listado de las teclas cargadas.