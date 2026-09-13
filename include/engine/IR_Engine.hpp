/**
 * @file IR_Engine.hpp
 * @brief Motor de codificación NEC para transmisión de señales infrarrojas.
 *
 * @details Implementa el protocolo de comunicación NEC (modo estándar) usado
 * por los controles remotos infrarrojos. Genera la portadora de 38 kHz por
 * software mediante bcm2835_gpio_write() con retardos de microsegundos,
 * siguiendo la especificación del protocolo NEC.
 *
 * Estructura de una trama NEC:
 *   - Inicio: 9000 µs de portadora + 4500 µs de espacio
 *   - 32 bits de datos (LSB primero): addr(8) | ~addr(8) | cmd(8) | ~cmd(8)
 *   - Parada: 560 µs de portadora
 *
 * Cada bit:
 *   - Bit 0: 560 µs de portadora + 560 µs de espacio
 *   - Bit 1: 560 µs de portadora + 1690 µs de espacio
 *
 * @author Proyecto IR-LG-Remote
 * @version 0.1.0
 */

#ifndef IR_ENGINE_HPP
#define IR_ENGINE_HPP

#include <cstdint>
#include <vector>

namespace IR {

/**
 * @class IR_Engine
 * @brief Motor de codificación NEC de bajo nivel.
 *
 * @details Genera la secuencia de pulsos NEC para controlar
 * un emisor de LED infrarrojo. La portadora de 38 kHz se genera
 * toggleando un pin GPIO con retardos precisos de microsegundos.
 */
class IR_Engine {
public:
    /**
     * @brief Constructor.
     * @param gpioPin Pin GPIO (numeración BCM) al que está conectado el LED IR.
     */
    explicit IR_Engine(uint8_t gpioPin);

    /** @brief Destructor. */
    ~IR_Engine() = default;

    /**
     * @brief Envía una trama NEC completa.
     * @param address Dirección de 8 bits (ej. 0x04 para LG).
     * @param command Comando de 8 bits.
     * @param repeat Número de repeticiones tras la trama inicial.
     */
    void sendNEC(uint8_t address, uint8_t command, uint8_t repeat = 0);

    /**
     * @brief Envía el byte de inicio NEC (9 ms burst + 4.5 ms space).
     */
    void sendStartFrame();

    /**
     * @brief Envía un byte NEC (LSB primero).
     * @param data Byte a transmitir.
     */
    void sendByte(uint8_t data);

    /**
     * @brief Enciende la portadora 38 kHz por un tiempo dado (burst).
     * @param durationUs Duración del pulso en microsegundos.
     */
    void carrierBurst(uint16_t durationUs);

    /**
     * @brief Mantiene el pin en bajo (espacio) por un tiempo dado.
     * @param durationUs Duración del espacio en microsegundos.
     */
    void spaceBurst(uint16_t durationUs);

    /**
     * @brief Envía el bit de parada NEC (560 µs de portadora).
     */
    void sendStopBit();

    /**
     * @brief Envía un marco de repetición NEC (9 ms burst + 2.25 ms space).
     */
    void sendRepeatFrame();

    /**
     * @brief Convierte un byte a su representación binaria inversa.
     * @param data Byte de entrada.
     * @return Complemento a 1 del byte.
     */
    static uint8_t invertByte(uint8_t data);

private:
    /** @brief Pin GPIO del LED IR. */
    uint8_t pin_;
};

} // namespace IR

#endif // IR_ENGINE_HPP
