# IR-LG-Remote

Emulador de control remoto **LG** por infrarrojos para **Raspberry Pi**, escrito en **C++** con la librería **bcm2835**. Emite señales IR mediante un LED infrarrojo (protocolo NEC, portadora 38 kHz) para controlar televisores LG.

> Compatible con **Raspberry Pi OS de 32 bits** (armhf/armv7l) y **64 bits** (arm64/aarch64).

La versión de la aplicación se define en tiempo de compilación (se lee del archivo `VERSION`) y se muestra al iniciar y con `./bin/App --version`.

## Características

- Emisión de comandos **NEC** (9 ms + 4,5 ms de inicio, 32 bits `addr | ~addr | cmd | ~cmd`, bit de parada).
- Portadora de **38 kHz** generada por software (`bcm2835_gpio_write` + `bcm2835_delayMicroseconds`, periodo ≈ 26 µs).
- Tabla de códigos LG editable sin recompilar en `config/ir_codes.cfg`.
- Tres modos de uso: línea de comandos, consola interactiva y macros.
- Modo **aprendizaje** (`--learn`) opcional con receptor VS1838B en GPIO 17.
- Pantalla **OLED SSD1306** (I2C) opcional para mostrar estado.
- Dirección NEC configurable (LG usa 0x04 por defecto).

## Hardware

| Componente | Pin (BCM) | Notas |
|---|---|---|
| LED IR emisor (a través de transistor NPN) | GPIO 22 | Puerta del transistor Q1 |
| Receptor VS1838B (opcional) | GPIO 17 | Modo `--learn`, requiere pull-up 10 kΩ |
| OLED SSD1306 (opcional) | I2C-1 (SDA=GPIO 2, SCL=GPIO 3) | Dirección 0x3C |

### Esquema del emisor IR (LED + transistor)

```
Raspberry Pi (3.3V)            +5V
   |                              |
  R1 (220 Ω)                      |
   |                              +--[ LED IR 940 nm ]--+
   GPIO 22 (BCM) -- R2 (1 kΩ) -- B |                    |
                                  | Q1 (2N2222/BC337)    |
                                  E +----+               |
                                     |  C ----------+---> a la base
                                    GND           R3 (47 Ω)
                                                 / residencia de protección
Sugerencia de diseño con transistor NPN:
  - GPIO 22 → R2 1 kΩ → base de Q1
  - Colector de Q1 → R3 47 Ω → LED IR → +5V
  - Emisor de Q1 → GND
  - Cuando el GPIO está en ALTO, Q1 satura y el LED emite IR.
```

> El detalle completo (incluido el circuito del receptor VS1838B) está en [docs/HARDWARE.md](docs/HARDWARE.md).

## Requisitos

- Raspberry Pi (cualquier modelo con bcm2835, Pi 5 incluida para OLED por I2C ioctl).
- Raspberry Pi OS (32 o 64 bits).
- `bcm2835` (se instala con el script de dependencias).
- LED IR (940 nm), transistor NPN, resistencias.
- Opcional: sensor VS1838B, OLED SSD1306, Git/GitHub CLI `gh`.

## Instalación

1. Clonar el repositorio:

   ```bash
   git clone https://github.com/siliconvalleyar-oss/ir_remote_rpi.git
   cd ir_remote_rpi
   ```

2. Instalar dependencias (bcm2835 v1.71, g++, make, git, gh opcional):

   ```bash
   sudo bash scripts/install_deps.sh
   ```

3. Compilar:

   ```bash
   make clean && make -j4
   ```

## Uso

```bash
# Enviar el comando POWER (pulsador del mando LG)
sudo ./bin/App --send POWER

# Listar las teclas disponibles
sudo ./bin/App --list

# Consola interactiva
sudo ./bin/App --interactive

# Ejecutar una macro
sudo ./bin/App --macro encender_tv

# Aprender un código del mando real (requiere VS1838B)
sudo ./bin/App --learn VOLUME_UP

# Mostrar la versión (no necesita sudo)
./bin/App --version
```

> La aplicación necesita `sudo` para el acceso a `/dev/mem` vía bcm2835 (excepto `--version`/`--list`).

### Compilación remota en la Raspberry Pi (SSH sin contraseña)

```bash
ssh pi@raspi.local "cd /home/pi/src/ir_remote_rpi && git pull && make clean && make -j4 && make run"
```

O directamente:

```bash
make remote                              # alias del comando anterior
make remote ARGS="--list"                # con argumentos
```

Configura la clave pública una vez:

```bash
ssh-copy-id pi@raspi.local
```

## Estructura del proyecto

```
├── bin/           # binario App
├── config/        # configuración (config.cfg, hardware.cfg, ir_codes.cfg)
├── docs/          # documentación completa
├── examples/      # ejemplos de uso
├── include/       # cabeceras (core, drivers, engine, oled, nlohmann)
├── obj/           # objetos de compilación (generado)
├── scripts/       # instalación, Git, despliegue y build remoto
├── src/           # código fuente
├── Makefile       # compilación (define VERSION desde VERSION)
├── LICENSE        # MIT
└── VERSION        # número de versión (1.0.0)
```

## Documentación

Todos los temas (arquitectura, protocolo NEC, hardware, instalación, uso, testing, seguridad, etc.) están documentados en `docs/`. Recomendados para empezar:

- [docs/INFRARED.md](docs/INFRARED.md) — protocolo NEC y portadora de 38 kHz.
- [docs/HARDWARE.md](docs/HARDWARE.md) — esquemas de emisor y receptor.
- [docs/BUILD.md](docs/BUILD.md) — compilación, incluida la cruzada.
- [docs/USAGE.md](docs/USAGE.md) — todos los modos y ejemplos.

## Repositorio Git

El repositorio se configura (repositorio local, remoto en GitHub, push y tag de versión) con el script interactivo:

```bash
bash scripts/setup_git.sh
```

Requiere `gh` autenticado (`gh auth login`) o credenciales en `GH_TOKEN`. Si no está disponible, el script muestra los pasos manuales:

```bash
git remote add origin https://github.com/USUARIO/ir_remote_rpi.git
git push -u origin main
```

## Licencia

MIT — ver [LICENSE](LICENSE). Copyright (c) 2026 *ir_remote_rpi contributors*.