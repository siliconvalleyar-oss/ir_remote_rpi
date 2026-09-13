# TODO — Tareas pendientes

Organización de pendientes. Véase prioridades en `ROADMAP.md`.

## Alta prioridad

- [ ] Validar códigos LG de `config/ir_codes.cfg` contra un TV real y ajustar
      los códigos de colores/dígitos si difieren.
- [ ] Receptor robusto: interrupciones por flanco en `IR_Receiver`.
- [ ] Modo `--learn` con dirección por tecla (tramas LG de 13 bits).

## Media prioridad

- [ ] Añadir objetivo `install` (script) al Makefile.
- [ ] Tests automáticos de trama NEC con analizador lógico.
- [ ] `docs/DIAGRAMS.md`: gráficos de tiempos y secuencia de trama.

## Baja prioridad

- [ ] PWM hardware en GPIO 18 como back-end alternativo.
- [ ] Interfaz web para enviar teclas.
- [ ] Macros programables por horario.

## Proceso

- [ ] Completar smoke tests documentados en `TESTING.md`.
- [ ] Revisar `docs/PROMPT.md` quedó como referencia estable de requisitos.