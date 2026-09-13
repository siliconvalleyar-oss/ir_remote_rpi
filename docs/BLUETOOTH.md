# BLUETOOTH — Bluetooth (ámbito futuro)

El proyecto IR-LG-Remote comunica por **infrarrojos** (IR), no por Bluetooth.
Este documento documenta el ámbito y posibles integraciones futuras.

## Estado actual

- **No hay soporte Bluetooth** en v1.0.0. La comunicación es exclusivamente IR
  (NEC, 38 kHz) dirigida a un receptor y DAMA de TV LG.

## Aclaraciones

- IR y Bluetooth son tecnologías distintas: IR es óptico direccional (línea de
  vista), Bluetooth es RF (2.4 GHz, mayor alcance y sin línea de vista).
- Un mando LG usa IR; por lo tanto este emulador NO sustituye a un mando
  Bluetooth (p. ej. magic remote).

## Futuro posible

1. **Ambos**: añadir un perfil Bluetooth (ej. HID) para controlar dispositivos
   que solo admitan BT.
2. **Bridge**: que la Pi reciba comandos por Bluetooth (SPP/BLE) y los envíe
   por IR. Esto permitiría controlar la TV desde el móvil.
3. **Integración**: usar la misma tabla `config/ir_codes.cfg` para un perfil
   HID con el mismo teclado LG.

## Referencias

- Módulos típicos: HC-05/HC-06 (SPP clásico), HM-10/ESP32 (BLE).
- En el kernel de la Pi ya existen stacks BlueZ; `bluetoothctl` para probar.

Si se desea implementar, abrir un issue siguiendo `docs/CONTRIBUTING.md` y
planificar en `docs/ROADMAP.md`.