/**
 * @file Device_t.cpp
 * @brief Implementación de la clase Device::Device_t (emulador IR-LG-Remote).
 *
 * @details Esta clase orquesta el funcionamiento completo del emulador de
 * control remoto LG. Parsea argumentos de línea de comandos, carga la
 * configuración desde archivos en config/, y ejecuta la acción solicitada
 * (enviar comando, modo interactivo, aprender, ejecutar macro, etc.).
 *
 * La pantalla OLED SSD1306 se inicializa opcionalmente para mostrar el
 * estado del dispositivo. El acceso I2C se realiza por /dev/i2c-N (ioctl),
 * compatible con todas las Raspberry Pi. El acceso GPIO se realiza mediante
 * bcm2835, inicializado por el emisor IR (IR_Transmitter).
 *
 * @author Proyecto IR-LG-Remote
 * @version 0.1.0
 */

#include "core/Device_t.hpp"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include <bcm2835.h>

#ifndef VERSION
#define VERSION "dev"
#endif

namespace Device {

/** @brief Dirección NEC del control LG (0x04). */
static constexpr uint8_t DEFAULT_IR_ADDRESS = 0x04;

/** @brief GPIO del emisor IR por defecto (GPIO 22). */
static constexpr uint8_t DEFAULT_GPIO_TX = 22;

/** @brief GPIO del receptor IR por defecto (GPIO 17). */
static constexpr uint8_t DEFAULT_GPIO_RX = 17;

Device_t::Device_t(int argc, char** argv)
    : oled_(nullptr),
      transmitter_(nullptr),
      receiver_(nullptr),
      irAddress_(DEFAULT_IR_ADDRESS),
      gpioTx_(DEFAULT_GPIO_TX),
      gpioRx_(DEFAULT_GPIO_RX),
      bcmReady_(false),
      oledReady_(false),
      runMode_(RunMode::HELP) {

    printVersion();

    // Carga configuraciones antes de parsear argumentos.
    loadHardwareConfig();
    loadIrCodes();

    parseArgs(argc, argv);
}

Device_t::~Device_t() {
    // El transmisor cierra bcm2835 en su destructor (end()).
    // El receptor no gestiona bcm2835 (lo asume inicializado).

    // Libera el OLED si se inicializó.
    if (oledReady_ && oled_) {
        oled_->OLEDPowerDown();
        oled_.reset();
    }

    std::printf("\nCierre completado.\n");
}

std::string Device_t::version() const {
    return std::string(VERSION);
}

void Device_t::printVersion() const {
    std::printf("IR-LG-Remote v%s\n", VERSION);
    std::printf("Compilado para: %s\n",
#ifdef __aarch64__
                "ARM64 (aarch64)"
#elif defined(__arm__)
                "ARM (32 bits)"
#else
                "x86-64"
#endif
    );
}

void Device_t::parseArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--version" || arg == "-v") {
            runMode_ = RunMode::SHOW_VERSION;
        } else if (arg == "--help" || arg == "-h") {
            runMode_ = RunMode::HELP;
        } else if (arg == "--send") {
            if (i + 1 < argc) {
                runMode_ = RunMode::SEND;
                sendKey_ = argv[++i];
            } else {
                std::printf("Error: --send requiere un nombre de tecla.\n");
                runMode_ = RunMode::HELP;
            }
        } else if (arg == "--interactive") {
            runMode_ = RunMode::INTERACTIVE;
        } else if (arg == "--macro") {
            if (i + 1 < argc) {
                runMode_ = RunMode::MACRO;
                macroName_ = argv[++i];
            } else {
                std::printf("Error: --macro requiere un nombre.\n");
                runMode_ = RunMode::HELP;
            }
        } else if (arg == "--learn") {
            if (i + 1 < argc) {
                runMode_ = RunMode::LEARN;
                learnKey_ = argv[++i];
            } else {
                std::printf("Error: --learn requiere un nombre de tecla.\n");
                runMode_ = RunMode::HELP;
            }
        } else if (arg == "--list") {
            runMode_ = RunMode::LIST_CODES;
        } else {
            std::printf("Argumento desconocido: %s\n", arg.c_str());
            runMode_ = RunMode::HELP;
        }
    }
}

bool Device_t::loadIrCodes() {
    // Carga la tabla de códigos IR desde config/ir_codes.cfg.
    // Formato: KEY_NAME=0xNN  (una por línea, # para comentarios)
    const std::string filePath = "config/ir_codes.cfg";
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::printf("Advertencia: No se encontró %s. Usando códigos por defecto.\n",
                     filePath.c_str());
        // Códigos por defecto LG NEC (dirección 0x04).
        irCodes_["POWER"]         = 0x08;
        irCodes_["VOLUME_UP"]     = 0x02;
        irCodes_["VOLUME_DOWN"]   = 0x03;
        irCodes_["MUTE"]          = 0x09;
        irCodes_["CHANNEL_UP"]    = 0x00;
        irCodes_["CHANNEL_DOWN"]  = 0x01;
        irCodes_["INPUT"]         = 0x0B;
        irCodes_["MENU"]          = 0x43;
        irCodes_["OK"]            = 0x44;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        // Elimina espacios en blanco en ambos lados.
        key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
        val.erase(std::remove_if(val.begin(), val.end(), ::isspace), val.end());

        try {
            uint8_t code = static_cast<uint8_t>(std::stoul(val, nullptr, 0));
            irCodes_[key] = code;
        } catch (...) {
            std::printf("Advertencia: Código inválido para '%s': %s\n",
                        key.c_str(), val.c_str());
        }
    }

    std::printf("Códigos IR cargados: %zu teclas\n", irCodes_.size());
    return true;
}

bool Device_t::loadHardwareConfig() {
    // Carga la configuración de hardware desde config/hardware.cfg.
    // Formato: PARAM=valor  (una por línea, # para comentarios)
    const std::string filePath = "config/hardware.cfg";
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::printf("Advertencia: No se encontró %s. Usando valores por defecto.\n",
                     filePath.c_str());
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
        val.erase(std::remove_if(val.begin(), val.end(), ::isspace), val.end());

        try {
            int num = std::stoi(val);
            if (key == "IR_TX_GPIO") {
                gpioTx_ = static_cast<uint8_t>(num);
            } else if (key == "IR_RX_GPIO") {
                gpioRx_ = static_cast<uint8_t>(num);
            } else if (key == "NEC_ADDRESS") {
                irAddress_ = static_cast<uint8_t>(num);
            }
        } catch (...) {
            // Ignora valores inválidos.
        }
    }

    std::printf("Config hardware: TX_GPIO=%d, RX_GPIO=%d, ADDR=0x%02X\n",
                gpioTx_, gpioRx_, irAddress_);
    return true;
}

void Device_t::printHelp() const {
    std::printf(R"(
IR-LG-Remote v%s — Emulador de control remoto infrarrojo LG

Uso: ./App [opciones]

Opciones:
  --version, -v       Muestra la versión y termina.
  --send <TECLA>      Envía el comando IR de la tecla especificada.
                      (ej: --send POWER)
  --interactive       Inicia la consola interactiva.
  --macro <nombre>    Ejecuta una macro predefinida.
                      (ej: --macro encender_tv)
  --learn <TECLA>     Modo aprendizaje: captura un comando IR real.
                      (ej: --learn VOLUME_UP)
  --list              Lista todas las teclas disponibles.
  --help, -h          Muestra esta ayuda.

Ejemplos:
  sudo ./App --send POWER
  sudo ./App --macro encender_tv
  sudo ./App --interactive
  sudo ./App --list

Teclas disponibles:
  POWER, VOLUME_UP, VOLUME_DOWN, MUTE, CHANNEL_UP, CHANNEL_DOWN,
  INPUT, MENU, OK, UP, DOWN, LEFT, RIGHT, NUM_0..NUM_9
)", VERSION);
}

void Device_t::transmit(uint8_t address, uint8_t command, const std::string& keyName) {
    if (!transmitter_) {
        std::printf("Error: Transmisor IR no inicializado.\n");
        return;
    }

    std::printf("> Enviando %s (cmd=0x%02X, addr=0x%02X)\n",
                keyName.c_str(), command, address);

    updateOled("Enviando: " + keyName);

    transmitter_->sendCommand(address, command, 0);
    std::printf("OK: %s enviado.\n", keyName.c_str());
}

void Device_t::updateOled(const std::string& message) {
    if (!oled_ || !oledReady_) return;

    oled_->OLEDclearBuffer();
    oled_->setTextColor(WHITE);
    oled_->setTextSize(1);
    oled_->setCursor(0, 0);
    oled_->print("IR-LG-Remote");
    oled_->setCursor(0, 12);

    // Trunca mensajes largos para el display de 128x64.
    std::string msg = message;
    if (msg.length() > 20) msg = msg.substr(0, 20);
    oled_->print(msg.c_str());

    // Dibaja barra de estado en la parte inferior.
    oled_->drawRect(0, 54, OLED_WIDTH, 8, WHITE);

    oled_->OLEDupdate();
}

void Device_t::cmdSend(const std::string& keyName) {
    // Busca la tecla en la tabla de códigos (insensible a mayúsculas).
    auto it = irCodes_.find(keyName);
    if (it == irCodes_.end()) {
        std::string upper = keyName;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        it = irCodes_.find(upper);
        if (it == irCodes_.end()) {
            std::printf("Error: Tecla '%s' no encontrada.\n", keyName.c_str());
            std::printf("Usa --list para ver las teclas disponibles.\n");
            return;
        }
    }

    transmit(irAddress_, it->second, keyName);
}

void Device_t::cmdMacro(const std::string& macroName) {
    // Busca la macro en config/config.cfg.
    // Formato: MACRO_<nombre>=<tecla1>[,<tecla2>,...]
    std::string macroKey = "MACRO_" + macroName;
    std::transform(macroKey.begin(), macroKey.end(), macroKey.begin(), ::toupper);

    std::ifstream file("config/config.cfg");
    if (!file.is_open()) {
        std::printf("Error: No se encontró config/config.cfg\n");
        return;
    }

    std::string line;
    bool found = false;
    std::string macroKeys;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = line.substr(0, eq);
        key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
        std::transform(key.begin(), key.end(), key.begin(), ::toupper);

        if (key == macroKey) {
            macroKeys = line.substr(eq + 1);
            found = true;
            break;
        }
    }

    if (!found) {
        std::printf("Error: Macro '%s' no encontrada.\n", macroName.c_str());
        return;
    }

    std::printf("> Ejecutando macro: %s\n", macroName.c_str());
    updateOled("Macro: " + macroName);

    // Parsea las teclas separadas por coma.
    std::stringstream ss(macroKeys);
    std::string key;
    while (std::getline(ss, key, ',')) {
        key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
        if (!key.empty()) {
            cmdSend(key);
            if (transmitter_) {
                // 50 ms de pausa entre teclas.
                bcm2835_delayMicroseconds(50000);
            }
        }
    }

    std::printf("Macro '%s' completada.\n", macroName.c_str());
}

void Device_t::cmdInteractive() {
    std::printf("Modo interactivo. Escribe 'help' para ayuda, 'exit' para salir.\n");
    std::printf("Teclas disponibles: ");
    for (auto it = irCodes_.begin(); it != irCodes_.end(); ++it) {
        if (it != irCodes_.begin()) std::printf(", ");
        std::printf("%s", it->first.c_str());
    }
    std::printf("\n");

    std::string input;
    while (true) {
        std::printf("\nir> ");
        if (!std::getline(std::cin, input)) break;

        if (input.empty()) continue;

        if (input == "exit" || input == "quit") {
            break;
        }
        if (input == "help") {
            std::printf("Comandos: <TECLA>, list, exit\n");
            std::printf("Ejemplo: POWER  (envía el comando POWER)\n");
        } else if (input == "list") {
            std::printf("Teclas: ");
            for (auto& kv : irCodes_) {
                std::printf("%s(0x%02X) ", kv.first.c_str(), kv.second);
            }
            std::printf("\n");
        } else {
            cmdSend(input);
        }
    }
}

void Device_t::cmdLearn(const std::string& keyName) {
    if (!receiver_) {
        std::printf("Error: Receptor IR no inicializado.\n");
        std::printf("El modo --learn requiere un VS1838B conectado a GPIO %d.\n", gpioRx_);
        return;
    }

    std::printf("Modo aprendizaje: %s\n", keyName.c_str());
    std::printf("Presiona la tecla '%s' en el control remoto físico...\n", keyName.c_str());
    std::printf("Esperando señal IR (15s de timeout)...\n");

    updateOled("Aprendiendo: " + keyName);

    auto start = std::chrono::steady_clock::now();
    uint16_t address = 0;
    uint8_t command = 0;

    while (true) {
        if (receiver_->learn(address, command)) {
            std::printf("Señal capturada: addr=0x%04X, cmd=0x%02X\n", address, command);

            // Guarda el código aprendido en config/ir_codes.cfg.
            std::ofstream file("config/ir_codes.cfg", std::ios::app);
            if (file.is_open()) {
                file << "\n# Aprendido con IR-LG-Remote\n";
                file << keyName << "=0x" << std::hex << static_cast<int>(command)
                     << "  # addr=0x" << std::hex << address << "\n";
                file.close();
                std::printf("Código guardado en config/ir_codes.cfg\n");
                updateOled("Guardado: " + keyName);
            } else {
                std::printf("Error: No se pudo escribir en config/ir_codes.cfg\n");
                updateOled("Error: guardar");
            }
            break;
        }

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
        if (elapsed >= 15) {
            std::printf("Timeout: No se recibió señal IR en 15s.\n");
            updateOled("Timeout aprendizaje");
            break;
        }
    }
}

void Device_t::run() {
    switch (runMode_) {
        case RunMode::SHOW_VERSION:
            printVersion();
            break;

        case RunMode::HELP:
            printHelp();
            break;

        case RunMode::SEND: {
            // Crea y configura el emisor IR (inicializa bcm2835 internamente).
            transmitter_ = std::make_unique<IR::IR_Transmitter>(gpioTx_);
            if (!transmitter_->begin()) {
                std::printf("Error: No se pudo inicializar el emisor IR.\n");
                return;
            }
            bcmReady_ = true;
            cmdSend(sendKey_);
            break;
        }

        case RunMode::MACRO: {
            transmitter_ = std::make_unique<IR::IR_Transmitter>(gpioTx_);
            if (!transmitter_->begin()) {
                std::printf("Error: No se pudo inicializar el emisor IR.\n");
                return;
            }
            bcmReady_ = true;
            cmdMacro(macroName_);
            break;
        }

        case RunMode::INTERACTIVE: {
            transmitter_ = std::make_unique<IR::IR_Transmitter>(gpioTx_);
            if (!transmitter_->begin()) {
                std::printf("Error: No se pudo inicializar el emisor IR.\n");
                return;
            }
            bcmReady_ = true;
            cmdInteractive();
            break;
        }

        case RunMode::LEARN: {
            // Necesitamos el emisor para inicializar bcm2835 (el receptor lo asume).
            transmitter_ = std::make_unique<IR::IR_Transmitter>(gpioTx_);
            if (!transmitter_->begin()) {
                std::printf("Error: No se pudo inicializar el emisor IR.\n");
                return;
            }
            bcmReady_ = true;

            receiver_ = std::make_unique<IR::IR_Receiver>(gpioRx_);
            if (!receiver_->begin()) {
                std::printf("Error: No se pudo inicializar el receptor IR.\n");
                return;
            }
            cmdLearn(learnKey_);
            break;
        }

        case RunMode::LIST_CODES: {
            std::printf("Códigos IR configurados (%zu teclas):\n", irCodes_.size());
            for (auto& kv : irCodes_) {
                std::printf("  %-18s = 0x%02X\n", kv.first.c_str(), kv.second);
            }
            break;
        }

        default:
            printHelp();
            break;
    }
}

} // namespace Device
