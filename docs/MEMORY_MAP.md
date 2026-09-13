# MEMORY_MAP — Mapa de memoria y de recursos

## Memoria dinámica (C++)

El proyecto usa **RAII**; no hay `new`/`delete` en el código de usuario.

| Objeto | Gestor | Ciclo de vida |
|---|---|---|
| `Device::Device_t` | `unique_ptr` en `main` | Toda la app; se libera al salir de `main`. |
| `SSD1306 oled_` | `unique_ptr` en `Device_t` | Creado en `run()` si procede; liberado en dtor de `Device_t`. |
| `IR::IR_Transmitter` | `unique_ptr` en `Device_t` | Creado en `run()`; dtor cierra bcm2835. |
| `IR::IR_Receiver` | `unique_ptr` en `Device_t` | Solo en modo `--learn`. |
| `irCodes_` (`map<string,uint8_t>`) | Miembro de `Device_t` | Carga en constructor; liberado al destruir `Device_t`. |
| `IR_Engine engine_` | Miembro de `IR_Transmitter` | Ciclo de vida del transmisor. |

## Recursos de hardware

| Recurso | Propietario | Apertura | Cierre |
|---|---|---|---|
| `/dev/mem` (bcm2835) | `IR_Transmitter` | `bcm2835_init()` | `bcm2835_close()` en `end()` |
| GPIO 22 (LED IR) | `IR_Transmitter` | `begin()` (fsel OUTP) | `end()` (LOW y close) |
| GPIO 17 (VS1838B) | `IR_Receiver` | `begin()` (fsel INPT) | — (asume bcm2835 abierto) |
| `/dev/i2c-1` (OLED) | `SSD1306` | `OLEDinit()` (open+ioctl) | `OLEDPowerDown()` (cierra fd) |

## Pila

Consumo modesto: `std::string`, `std::map`, buffers de la OLED (~1 KB de
framebuffer en `_oled_buffer`). Sin recursión relevante.

## Notas

- `bcm2835_init()` es idempotente; si otra instancia lo llama, no hay doble
  reserva de recursos.
- El destructor de `IR_Transmitter` garantiza `bcm2835_close()` aunque falte
  `end()` explícito; por eso el orden de destrucción de los `unique_ptr` de
  `Device_t` (transmitter antes que receiver) es seguro.