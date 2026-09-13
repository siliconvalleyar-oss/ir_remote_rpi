# INSTALL — Instalación del proyecto

## En la Raspberry Pi

```bash
# 1. Clonar
git clone https://github.com/siliconvalleyar-oss/ir_remote_rpi.git
cd ir_remote_rpi

# 2. Dependencias (bcm2835 v1.71, g++, make, git, gh opcional)
sudo bash scripts/install_deps.sh

# 3. Compilar
make clean && make -j4

# 4. Probar
./bin/App --version
sudo ./bin/App --list
```

## Verificar el hardware

- Emisor IR en GPIO 22 (LED + transistor). Ver [HARDWARE.md](HARDWARE.md).
- Ejecute `sudo ./bin/App --send POWER` apuntando al TV LG.

## Opcional: OLED SSD1306

- Active I2C en la Pi: `sudo raspi-config` → Interface Options → I2C → Enable.
- Conecte SDA=GPIO2, SCL=GPIO3, VCC=3.3V, GND=GND (dirección 0x3C).
- Instale `i2c-tools` y compruebe con `sudo i2cdetect -y 1`.

## Compilación remota (desde el PC)

```bash
make remote                                  # git pull + make + run en la Pi
make deploy                                  # scp bin/App a la Pi
```

Requiere clave pública configurada:

```bash
ssh-copy-id pi@raspi.local
```

## Repositorio Git

```bash
bash scripts/setup_git.sh                    # repo local + GitHub + tag
```

Véase [SETUP.md](SETUP.md) y [DEPLOY.md](DEPLOY.md) para más detalles.