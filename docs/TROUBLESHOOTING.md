# TROUBLESHOOTING — Solución de problemas

## El TV no responde al enviar POWER

1. **Permisos:** accione con sudo. `bcm2835` abre `/dev/mem`:
   `sudo ./bin/App --send POWER`.
2. **GPIO correcto:** verifique `IR_TX_GPIO` en `config/hardware.cfg` (22).
3. **Transistor:** compruebe R2/R3 y la saturación de Q1. Sin R3 el LED puede
   quedar sin corriente.
4. **Distancia/ángulo:** el LED IR es direccional; apunte directamente al
   receptor del TV (no use cristal delante).
5. **Está emitiendo algo:** apunte el LED a la cámara del móvil; debe verse
   brillo violeta intermitente.
6. **Códigos equivocados:** pruebe `--list` y consulte los códigos LG; los de
   `ir_codes.cfg` son los más comunes pero algunos modelos LG varían.

## `bcm2835_init()` falla

- Ejecute con sudo. Verifique que no haya otro proceso usando el GPIO.
- En Pi 5/bootloader: `bcm2835` para GPIO funciona vía `/dev/mem`; si falla,
  revise `dmesg | grep -i bcm2835`.

## El modo `--learn` no captura nada

- VS1838B en GPIO 17 con pull-up de 10 kΩ a 3.3V.
- Invierta Vcc/GND del sensor (módulos baratos a veces lo marcan distinto).
- Acérquese al mando original (5-30 cm) y mantenga pulsado.

## Errores de compilación

- **`No existe bcm2835.h`:** no se instaló bcm2835. Ejecute
  `sudo bash scripts/install_deps.sh`.
- **Enlazado `libbcm2835.a` ausente en cruzado:** ejecute
  `bash scripts/fetch_sysroot.sh`.
- **OLED no encontrada:** los `.cpp` de OLED incluyen
  `-Iinclude/oled`; el Makefile ya lo añade.

## La OLED no se enciende

- Active I2C: `sudo raspi-config` → Interface Options → I2C → Enable.
- Compruebe dirección con `sudo i2cdetect -y 1` (debe aparecer `0x3c`).
- Verifique alimentación 3.3V y GND de la pantalla.

## Errores de permisos de GPIO aun con sudo

- Compruebe que gpio no esté exportado/ocupado por otro proceso:
  `ls /sys/class/gpio/`.

## El TV apaga/enciende pero con errores

- Acorte la distancia o inserte pausas entre repeticiones.
- Algunos LG usan la dirección 0x08 en lugar de 0x04: edite `NEC_ADDRESS` en
  `config/hardware.cfg`.