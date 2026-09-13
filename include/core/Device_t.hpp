/**
 * @file Device_t.hpp
 * @brief Clase principal del dispositivo IR-LG-Remote.
 *
 * @details Define la clase Device::Device_t que encapsula la inicialización
 * de bcm2835, la carga de configuración, el parsing de argumentos de línea
 * de comandos y la orquestación de las operaciones de emisión/recepción IR.
 *
 * La clase también gestiona la pantalla OLED SSD1306 (opcional) para mostrar
 * el estado del dispositivo.
 *
 * @author Proyecto IR-LG-Remote
 * @version 0.1.0
 */

#ifndef DEVICE_T_HPP
#define DEVICE_T_HPP

#include <memory>
#include <cstdint>
#include <string>
#include <map>

// Incluye la cabecera de la pantalla OLED (SSD1306) vía I2C.
#include "oled/SSD1306_OLED.hpp"

// La librería OLED define una macro global "swap(a,b)" que interfiere con
// std::swap de la STL. Anulamos la macro después de incluir los headers OLED.
#ifdef swap
#undef swap
#endif

#include "drivers/IR_Transmitter.hpp"
#include "drivers/IR_Receiver.hpp"

/**
 * @namespace Device
 * @brief Componentes de alto nivel del dispositivo IR.
 */
namespace Device {

/**
 * @class Device_t
 * @brief Orchestra el funcionamiento del emulador de control remoto LG.
 *
 * @details Inicializa bcm2835 en el constructor, parsea argumentos de línea
 * de comandos y ejecuta la acción solicitada (enviar comando, modo interactivo,
 * aprender, etc.). La memoria del OLED se gestiona con unique_ptr (RAII).
 */
class Device_t {
public:
    /** @brief Ancho del display OLED en píxeles. */
    static constexpr int16_t OLED_WIDTH = 128;
    /** @brief Alto del display OLED en píxeles. */
    static constexpr int16_t OLED_HEIGHT = 64;

    /**
     * @brief Constructor.
     * @param argc Número de argumentos de la línea de comandos.
     * @param argv Vector de argumentos de la línea de comandos.
     *
     * @details Inicializa bcm2835, parsea los argumentos y carga la
     * tabla de códigos IR desde config/ir_codes.cfg.
     */
    Device_t(int argc, char** argv);

    /**
     * @brief Destructor. Libera bcm2835 y el OLED.
     */
    ~Device_t();

    // No copiable (gestión de recursos de hardware).
    Device_t(const Device_t&) = delete;
    Device_t& operator=(const Device_t&) = delete;

    /**
     * @brief Ejecuta la lógica principal según los argumentos parseados.
     */
    void run();

    /**
     * @brief Devuelve la versión de la aplicación.
     * @return Cadena con el número de versión (definida en tiempo de compilación).
     */
    std::string version() const;

private:
    /**
     * @brief Muestra la versión de la aplicación por consola.
     */
    void printVersion() const;

    /**
     * @brief Parsea los argumentos de la línea de comandos.
     * @param argc Número de argumentos.
     * @param argv Vector de argumentos.
     */
    void parseArgs(int argc, char** argv);

    /**
     * @brief Carga la tabla de códigos IR desde config/ir_codes.cfg.
     * @return true si la carga fue exitosa.
     */
    bool loadIrCodes();

    /**
     * @brief Carga la configuración de hardware desde config/hardware.cfg.
     * @return true si la carga fue exitosa.
     */
    bool loadHardwareConfig();

    /**
     * @brief Acción: envía un comando IR por nombre de tecla.
     * @param keyName Nombre de la tecla (ej. "POWER").
     */
    void cmdSend(const std::string& keyName);

    /**
     * @brief Acción: ejecuta una macro predefinida.
     * @param macroName Nombre de la macro (ej. "encender_tv").
     */
    void cmdMacro(const std::string& macroName);

    /**
     * @brief Acción: modo interactivo (consola REPL).
     */
    void cmdInteractive();

    /**
     * @brief Acción: modo aprendizaje (captura códigos IR).
     * @param keyName Nombre de la tecla a aprender.
     */
    void cmdLearn(const std::string& keyName);

    /**
     * @brief Muestra la ayuda de la aplicación.
     */
    void printHelp() const;

    /**
     * @brief Transmite un comando IR por dirección y código.
     * @param address Dirección NEC.
     * @param command Código del comando.
     * @param keyName Nombre legible de la tecla (para OLED).
     */
    void transmit(uint8_t address, uint8_t command, const std::string& keyName);

    /**
     * @brief Actualiza la pantalla OLED con el estado actual.
     * @param message Mensaje a mostrar.
     */
    void updateOled(const std::string& message);

    /** @brief Instancia de la pantalla OLED (memoria gestionada automáticamente). */
    std::unique_ptr<SSD1306> oled_;

    /** @brief Emisor IR principal. */
    std::unique_ptr<IR::IR_Transmitter> transmitter_;

    /** @brief Receptor IR (para modo --learn). */
    std::unique_ptr<IR::IR_Receiver> receiver_;

    /** @brief Tabla de códigos IR: nombre_de_tecla -> código NEC (8 bits). */
    std::map<std::string, uint8_t> irCodes_;

    /** @brief Dirección NEC (address) para el control LG. */
    uint8_t irAddress_;

    /** @brief GPIO del emisor IR LED. */
    uint8_t gpioTx_;

    /** @brief GPIO del receptor IR. */
    uint8_t gpioRx_;

    /** @brief Indica si bcm2835 está inicializado. */
    bool bcmReady_;

    /** @brief Indica si el OLED se inicializó. */
    bool oledReady_;

    /** @brief Modo de ejecución (sin argumentos = info por defecto). */
    enum class RunMode {
        HELP,
        SEND,
        INTERACTIVE,
        MACRO,
        LEARN,
        SHOW_VERSION,
        LIST_CODES
    } runMode_;

    /** @brief Nombre de la tecla a enviar (modo SEND). */
    std::string sendKey_;

    /** @brief Nombre de la macro a ejecutar (modo MACRO). */
    std::string macroName_;

    /** @brief Nombre de la tecla a aprender (modo LEARN). */
    std::string learnKey_;
};

} // namespace Device

#endif // DEVICE_T_HPP
