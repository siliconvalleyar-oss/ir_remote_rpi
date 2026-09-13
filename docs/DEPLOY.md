# DEPLOY — Despliegue del proyecto en la Raspberry Pi

## Opción A: compilación y ejecución remota (recomendada)

Compila en la propia Pi desde el último commit y ejecuta la app:

```bash
ssh pi@raspi.local "cd /home/pi/src/ir_remote_rpi && git pull && make clean && make -j4 && make run"
```

Equivalente: `make remote`. Con argumentos:

```bash
make remote ARGS="--list"
make remote ARGS="--send VOLUME_UP"
```

Requiere:
- Clave pública SSH: `ssh-copy-id pi@raspi.local`
- En la Pi: repo clonado y dependencias instaladas.

Variables de entorno alternativas: `RPI_HOST`, `RPI_DIR`, `RPI_ARGS`, `RPI_SUDO`.

## Opción B: despliegue del binario (scp)

Copia `bin/App` (sin recompilar en la Pi):

```bash
make deploy                       # solo copia
RPI_RUN=1 RPI_ARGS="--list" make deploy   # copia y ejecuta
```

## Opción C: compilación cruzada desde el PC

```bash
bash scripts/fetch_sysroot.sh     # trae bcm2835 de la Pi a sysroot/
make crossover64                  # aarch64
scp bin/App pi@raspi.local:/home/pi/bin/
```

## Comandos útiles en la Pi

```bash
sudo apt-get install -y i2c-tools     # verificación I2C
sudo i2cdetect -y 1                   # buscar la OLED en 0x3c
pgrep App                             # comprobar que no quede un proceso vivo
```