# HARDWARE — Esquemas del circuito emisor/receptor IR

Documentación del hardware de IR-LG-Remote. Todos los pines se indican con la
numeración **BCM** (Broadcom) usada por `bcm2835` y el conector de 40 pines.

## Resumen de conexiones

| Señal | GPIO (BCM) | Pin físico 40-pin | Hardware |
|---|---|---|---|
| IR_TX (LED emisor) | 22 | 15 | Base del transistor NPN Q1 |
| IR_RX (VS1838B) | 17 | 11 | Salida del sensor (pull-up 10 kΩ) |
| I2C SDA | 2 | 3 | OLED SSD1306 |
| I2C SCL | 3 | 5 | OLED SSD1306 |

> Los GPIO se configuran en `config/hardware.cfg` (`IR_TX_GPIO`, `IR_RX_GPIO`,
> `OLED_I2C_BUS`, `OLED_I2C_ADDRESS`).

## 1. Emisor IR (LED + transistor NPN)

El GPIO de la Raspberry Pi solo puede entregar ~16 mA @ 3.3 V. Un LED IR típico
necesita ~30-100 mA a plena potencia, por lo que se usa un **transistor NPN**
(2N2222, BC337, S8050...) como interruptor.

```
                        +5V (pin 2/4 del conector)
                         |
                     R3 47 Ω  (limitación de corriente del LED)
                         |
                     +---|<|----+          LED IR 940 nm
                     |    D1      |        (ánodo → colector)
                     |    |       +-----+
                     |               C   |
                     |               |   |
    GPIO 22 ── R2 1 kΩ ─── B  Q1  E    GND
                   (1015 Ω | 2N2222)   (emisor a GND)
```

- **GPIO 22** (salida 3.3 V) → **R2 1 kΩ** → **base** de Q1.
- **Colector** de Q1 → **R3 47 Ω** → fotodiodo IR (LED de 940 nm) → **+5V**.
- **Emisor** de Q1 → **GND**.
- En ALTO el GPIO satura Q1 y el LED emite luz IR; en BAJO el LED queda apagado.

Variante PNP (activo en BAJO, si se prefiere lógica invertida) — no usada.

> Sustituya los valores de R2/R3 en función de su LED (compruebe la ficha:
> Vf y corriente máxima). R2 1kΩ limita la corriente de base (~2.6 mA); R3 47Ω
> limita la corriente del LED (~60 mA con un LED de 2 V de Vf).

## 2. Receptor IR opcional (VS1838B) — modo `--learn`

El VS1838B es un receptor demodulador de 38 kHz con salida de **colector
abierto**: en reposo deja el pin flotando/pull-up (nivel alto) y en cada marca
de 38 kHz lo baja a 0.

```
    +3.3V (pin 1)
     |
    R4 10 kΩ  (pull-up a 3.3V)
     |
     +-------------------- GPIO 17  (BCM 17 → pin físico 11)
     |
  VS1838B (3 pines, vista desde abajo)
     |  |  |
     V        OUT        GND
     |         +-------------------- GPIO 17
     +3.3V (pin 1)        GND (pin 6)
```

- **Pin OUT** del VS1838B → **GPIO 17** con **pull-up externo de 10 kΩ** a 3.3V.
- **Vcc** → 3.3 V y **GND** → masa (requiere condensador 100 nF junto al sensor).
- La señal demodulada entra como evento en `bcm2835_gpio_lev()` (polling en
  `IR_Receiver`).

## 3. Pantalla OLED SSD1306 (I2C) — opcional

```
    +3.3V  ── VCC (y pin RES izo opcional)
    GND    ── GND
    GPIO 2 (SDA, pin 3) ── SDA
    GPIO 3 (SCL, pin 5) ── SCL
```

- Dirección I2C: **0x3C** (configurable en `config/hardware.cfg`).
- El driver usa ioctl sobre `/dev/i2c-1` (capa `SSD1306_I2C`), por lo que
  funciona también en Raspberry Pi 5 / CM5.

## 4. Verificación rápida (sin soldar)

1. Conecte un LED IR directamente (con resistencia) a GPIO 22 + GND.
2. Ejecute `sudo ./bin/App --send POWER` apuntando a la cámara del móvil
   (la cámara ve el IR como un brillo violeta).
3. Si el TV no responde, compruebe con un receptor VS1838B + `--learn` o con
   un logic analyzer que la señal salga por GPIO 22.

## 5. Imágenes

Véase `docs/raspberry_pi_4.png` (diagrama de pines de referencia de la Pi).