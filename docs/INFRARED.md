# INFRARED — Protocolo NEC y portadora de 38 kHz

Este documento explica el protocolo de infrarrojos **NEC** usado por los
televisores LG y cómo IR-LG-Remote lo genera **por software** en la Raspberry Pi.

## 1. Principios básicos de la comunicación IR

Un control remoto IR emite pulsos de luz infrarroja (940 nm) modulados a una
frecuencia portadora. El receptor (p. ej. VS1838B) demodula la portadora y
entrega un tren de flancos digitales que codifica los datos.

- **Portadora:** 38 kHz (periodo ≈ 26.3 µs). Tolerancia típica del receptor ±20 %.
- **Burst (marca):** la portadora está activa (LED encendido/parpadeando).
- **Space (pausa):** la portadora está inactiva (LED apagado).
- La información se transmite en la **duración** de los espacios.

## 2. Tiempos del protocolo NEC

| Elemento | Burst (marca) | Space (pausa) |
|---|---|---|
| Inicio de trama | 9000 µs | 4500 µs |
| Repetición (tecla mantenida) | 9000 µs | 2250 µs |
| Bit "0" | 562.5 µs | 562.5 µs |
| Bit "1" | 562.5 µs | 1687.5 µs |
| Bit de parada (fin) | 562.5 µs | — |

La trama completa:

```
9000µs ON  4500µs OFF  |  addr(8)  ~addr(8)  cmd(8)  ~cmd(8)  |  562µs ON
     marca               \+------ 32 bits (LSB primero) -----+/        parada
```

- Se transmiten **32 bits**: dirección (8), complemento (8), comando (8),
  complemento (8), siempre **LSB primero**.
- El complemento sirve como **verificación de paridad**: el receptor valida
  que `addr ^ ~addr == 0xFF` y que `cmd ^ ~cmd == 0xFF`.

### Ejemplo: POWER de un mando LG

Con dirección `0x04` y comando `0x08` (POWER):

```
addr=00000100  ~addr=11111011  cmd=00001000  ~cmd=11110111   (Lﯢbit en LSB first:
 bytes ya invertidos en orden de transmisión)
```

Bloque de datos de 32 bits transmitidos por el aire, en orden:
`addr`, `~addr`, `cmd`, `~cmd`, cada byte LSB primero.

## 3. Generación por software (sin PWM)

IR-LG-Remote genera la portadora de 38 kHz **toggleando un GPIO** en un bucle
(`src/engine/IR_Engine.cpp`):

```cpp
uint32_t cycles = durationUs / 26;            // ciclos de ~26 µs
for (uint32_t i = 0; i < cycles; ++i) {
    bcm2835_gpio_write(pin_, HIGH);           // marca: LED encendido
    bcm2835_delayMicroseconds(13);            // 13 µs en alto
    bcm2835_gpio_write(pin_, LOW);            // marca: LED apagado
    bcm2835_delayMicroseconds(13);            // 13 µs en bajo
}
```

Cada ciclo dura ~26 µs (13 µs ON + 13 µs OFF) → ~38.46 kHz, dentro de la
tolerancia de ±20 %. El receptor VS1838B integra esa ráfaga y la entrega como
una "marca" simple; por eso el tiempo de ciclo concreto no es crítico mientras
rondé los 26 µs.

### Calidad de temporización

`bcm2835_delayMicroseconds()` usa `nanosleep()` y, en la práctica, introduce
una sobrecarga de unas decenas de microsegundos en el bucle. Para mitigarlo:

- Los valores NEC usados (562 µs, 9000 µs, etc.) tienen margen holgado (±20 %).
- Un ciclo de portadora de 26 µs con retardos de 13 µs es estable porque ambas
  mitades sufren la misma sobrecarga y el ciclo total se mantiene cercano a 26 µs.

### Alternativas hardware

Si se desea precisión absoluta se puede usar:

- **PWM hardware** (GPIO 18): genera los 38 kHz sin consumo de CPU.
- **SPI** con a-cero (GPIO 12): útil para emitir patrones largos con DMA.

No están implementadas en esta versión; el enfoque por software es suficiente
para el receptor LG.

## 4. Verificación de una señal válida

El receptor `IR_Receiver` (`src/drivers/IR_Receiver.cpp`) valida:

1. Pulso inicial ≥ 8000 µs (marca de inicio).
2. Espacio inicial entre 3000-3500 µs (4500 nominal) → trama de datos, o
   2000-2500 µs (2250 nominal) → repetición (sin datos).
3. Para cada bit, pulso ≈ 562 µs y espacio que discrimina 0/1.
4. Paridad: `addr ^ ~addr == 0xFF` y `cmd ^ ~cmd == 0xFF`.

## 5. Códigos LG típicos (dirección 0x04)

| Tecla | Código | Tecla | Código |
|---|---|---|---|
| POWER | 0x08 | UP | 0x60 |
| VOLUME_UP | 0x02 | DOWN | 0x6F |
| VOLUME_DOWN | 0x03 | LEFT | 0x61 |
| MUTE | 0x09 | RIGHT | 0x75 |
| CHANNEL_UP | 0x00 | MENU | 0x43 |
| CHANNEL_DOWN | 0x01 | OK | 0x44 |
| INPUT | 0x0B | Dígitos 0-9 | 0x00-0x09 |

Los códigos se cargan desde `config/ir_codes.cfg` y son editables sin
recompilar.

## 6. Referencias

- Protocolo NEC: https://www.sbprojects.net/projects/ir/nec.php
- Librería bcm2835: http://www.airspayce.com/mikem/bcm2835/