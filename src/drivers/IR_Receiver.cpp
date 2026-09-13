/**
 * @file IR_Receiver.cpp
 * @brief Implementación del driver de receptor IR (VS1838B).
 *
 * @details Captura y decodifica señales infrarrojas NEC usando un sensor
 * VS1838B conectado a un GPIO. No inicializa bcm2835 (asume que el
 * transmisor o Device_t ya lo ha inicializado).
 *
 * El decodificador usa polling con temporización precisa. Mide duraciones
 * de pulsos y espacios para extraer la dirección y el comando de cada trama.
 *
 * Duraciones NEC (tolerancia ±20%):
 *   - Inicio:   9000 µs ON + 4500 µs OFF
 *   - Repetición: 9000 µs ON + 2250 µs OFF
 *   - Bit 0:    562 µs ON + 562 µs OFF
 *   - Bit 1:    562 µs ON + 1687 µs OFF
 *
 * @author Proyecto IR-LG-Remote
 * @version 0.1.0
 */

#include "drivers/IR_Receiver.hpp"

#include <bcm2835.h>
#include <cstdio>

namespace IR {

IR_Receiver::IR_Receiver(uint8_t gpioPin)
    : pin_(gpioPin) {
}

bool IR_Receiver::begin() {
    // Asume que bcm2835 ya está inicializado (por el transmisor o Device_t).
    // Configura el GPIO del VS1838B como entrada.
    bcm2835_gpio_fsel(pin_, BCM2835_GPIO_FSEL_INPT);
    std::printf("Receptor IR inicializado en GPIO %d\n", pin_);
    return true;
}

uint32_t IR_Receiver::awaitPinChange(bool expected, uint32_t maxUs) {
    // Mide el tiempo que el pin permanece en el nivel `expected`.
    // Usa sondeo a 50 µs de intervalo.
    uint32_t elapsed = 0;
    uint32_t step = 50;

    while (bcm2835_gpio_lev(pin_) == (expected ? HIGH : LOW)) {
        bcm2835_delayMicroseconds(step);
        elapsed += step;
        if (elapsed >= maxUs) {
            break;
        }
    }
    return elapsed;
}

bool IR_Receiver::learn(uint16_t& address, uint8_t& command) {
    // 1. Esperar a que el pin esté en bajo (estado idle del VS1838B es HIGH).
    //    El VS1838B genera HIGH cuando no hay señal IR.
    uint32_t elapsed = 0;
    while (bcm2835_gpio_lev(pin_) == HIGH) {
        bcm2835_delayMicroseconds(50);
        elapsed += 50;
        if (elapsed > 50000) {
            return false;  // Timeout: 50 ms sin señal
        }
    }

    // 2. Medir el pulso inicial (debe ser ~9 ms para inicio o repetición).
    uint32_t pulse = awaitPinChange(true, 10000);  // espera subida, máximo 10 ms
    if (pulse < 8000) {
        return false;  // pulso demasiado corto: no es un inicio válido
    }

    // 3. Medir el espacio después del pulso inicial.
    //    - 4.5 ms  → inicio de trama NEC
    //    - 2.25 ms → señal de repetición
    uint32_t space = awaitPinChange(false, 5000);
    if (space > 3000 && space < 3500) {
        // Inicio de trama estándar: continuar decodificando 32 bits.
    } else if (space > 2000 && space < 2500) {
        // Repetición: no contiene datos nuevos, no procesar.
        return false;
    } else {
        // Space no reconocido.
        return false;
    }

    // 4. Decodificar 32 bits (LSB primero).
    uint32_t rawData = 0;
    for (int i = 0; i < 32; ++i) {
        // Medir pulso (~562 µs).
        uint32_t bitPulse = awaitPinChange(true, 1000);
        if (bitPulse < 400) {
            return false;  // pulso demasiado corto
        }

        // Medir espacio para diferenciar bit 0 vs bit 1.
        uint32_t bitSpace = awaitPinChange(false, 2000);

        rawData >>= 1;  // LSB first
        if (bitSpace > 1000) {
            // ~1.69 ms → bit 1
            rawData |= 0x80000000;
        }
        // Si bitSpace < 1000 (~562 µs) → bit 0
    }

    // 5. Extraer dirección y comando.
    uint8_t addrLo   = rawData & 0xFF;            // byte 0: dirección
    uint8_t addrHi   = (rawData >> 8) & 0xFF;     // byte 1: ~dirección
    uint8_t cmd      = (rawData >> 16) & 0xFF;    // byte 2: comando
    uint8_t cmdInv   = (rawData >> 24) & 0xFF;    // byte 3: ~comando

    // Validar paridad (los complementos deben invertir todos los bits).
    if ((addrLo ^ addrHi) != 0xFF || (cmd ^ cmdInv) != 0xFF) {
        std::printf("Advertencia: Trama IR con paridad incorrecta (posible ruido)\n");
        return false;
    }

    address  = addrLo;
    command  = cmd;
    return true;
}

uint8_t IR_Receiver::getGpioPin() const {
    return pin_;
}

} // namespace IR
