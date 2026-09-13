# SETUP — Configuración inicial

## 1. Instalar dependencias

```bash
sudo bash scripts/install_deps.sh
```

Instala: build-essential, g++, make, git, bcm2835 v1.71 (desde fuente oficial),
herramientas I2C y GitHub CLI (opcional). Válido para Raspberry Pi OS de 32 y
64 bits.

## 2. Clonar / preparar el repositorio

```bash
git clone https://github.com/siliconvalleyar-oss/ir_remote_rpi.git
cd ir_remote_rpi
make clean && make -j4
```

## 3. Configurar el repositorio Git

```bash
bash scripts/setup_git.sh
```

Pregunta: usuario, repositorio y visibilidad. Usa `gh` si está disponible
o muestra los pasos manuales. Ver [docs/LEARNINGS.md](LEARNINGS.md) para las
reglas de versionado (tag = VERSION).

## 4. Configurar el hardware

Edite `config/hardware.cfg` si sus pines difieren. Esquemas en
[docs/HARDWARE.md](HARDWARE.md).

## 5. Compilación y despliegue remoto (SSH sin contraseña)

```bash
ssh-copy-id pi@raspi.local                       # clave pública
make remote                                      # git pull + make + run en la Pi
make remote ARGS="--send POWER"
```

## 6. Opcional: OLED SSD1306

```bash
sudo raspi-config   # Interface Options → I2C → Enable
sudo i2cdetect -y 1 # debe verse 0x3c
```