/**
 * @file IR_Engine.cpp
 * @brief Implementación del motor de codificación NEC.
 *
 * @details Genera la portadora de 38 kHz y las tramas NEC mediante
 * bcm2835_gpio_write() y bcm2835_delayMicroseconds(). Cada ciclo de la
 * portadora dura ~26.3 µs (13 µs alto + 13 µs bajo), lo que da una
 * frecuencia de ~38 kHz dentro de la tolerancia del receptor (±20%).
 *
 * @author Proyecto IR-LG-Remote
 * @version 0.1.0
 */

#include "engine/IR_Engine.hpp"

#include <bcm2835.h>

namespace IR {

// --- Constantes de temporización NEC (microsegundos) ---
// Start frame: 9000 µs burst + 4500 µs space
// Repeat frame: 9000 µs burst + 2250 µs space
// Bit 0: 562.5 µs burst + 562.5 µs space
// Bit 1: 562.5 µs burst + 1687.5 µs space
// Stop: 562.5 µs burst
// Carrier: 38 kHz -> 13 µs high, 13 µs low (26 µs period ≈ 38.46 kHz)

IR_Engine::IR_Engine(uint8_t gpioPin)
    : pin_(gpioPin) {
}

uint8_t IR_Engine::invertByte(uint8_t data) {
    return static_cast<uint8_t>(~data);
}

void IR_Engine::carrierBurst(uint16_t durationUs) {
    // Genera portadora 38 kHz toggleando el pin GPIO.
    // Cada ciclo = 13 µs alto + 13 µs bajo ≈ 26 µs (≈ 38.46 kHz).
    uint32_t cycles = durationUs / 26;  // número de ciclos completos
    for (uint32_t i = 0; i < cycles; ++i) {
        bcm2835_gpio_write(pin_, HIGH);
        bcm2835_delayMicroseconds(13);
        bcm2835_gpio_write(pin_, LOW);
        bcm2835_delayMicroseconds(13);
    }
}

void IR_Engine::spaceBurst(uint16_t durationUs) {
    // Asegura el pin en bajo durante el espacio.
    bcm2835_gpio_write(pin_, LOW);
    bcm2835_delayMicroseconds(durationUs);
}

void IR_Engine::sendStartFrame() {
    // Inicio NEC: 9 ms de portadora + 4.5 ms de espacio
    carrierBurst(9000);
    spaceBurst(4500);
}

void IR_Engine::sendByte(uint8_t data) {
    // Transmite 8 bits, LSB primero.
    // Bit 0: 560 µs burst + 560 µs space
    // Bit 1: 560 µs burst + 1690 µs space
    for (int i = 0; i < 8; ++i) {
        if (data & 0x01) {
            // Bit 1
            carrierBurst(560);
            spaceBurst(1690);
        } else {
            // Bit 0
            carrierBurst(560);
            spaceBurst(560);
        }
        data >>= 1;
    }
}

void IR_Engine::sendStopBit() {
    // Parada NEC: 560 µs de portadora
    carrierBurst(560);
}

void IR_Engine::sendRepeatFrame() {
    // Repetición NEC: 9 ms de portadora + 2.25 ms de espacio
    // (usado para teclas mantenidas)
    carrierBurst(9000);
    spaceBurst(2250);
}

void IR_Engine::sendNEC(uint8_t address, uint8_t command, uint8_t repeat) {
    // Trama NEC completa:
    // 1. Start frame (9 ms burst + 4.5 ms space)
    // 2. 32 bits: addr(8) | ~addr(8) | cmd(8) | ~cmd(8)  [LSB first]
    // 3. Stop bit (560 µs burst)
    sendStartFrame();
    sendByte(address);            // dirección
    sendByte(invertByte(address)); // complemento de dirección
    sendByte(command);             // comando
    sendByte(invertByte(command)); // complemento de comando
    sendStopBit();

    // Envía repeticiones si se solicita (gap de 45 ms + repeat frame)
    for (uint8_t r = 0; r < repeat; ++r) {
        // Gap entre tramas: al menos 45 ms sin portadora
        spaceBurst(45000);
        sendRepeatFrame();
    }
}

} // namespace IR
