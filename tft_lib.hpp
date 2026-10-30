#ifndef __TFT_LIB__
#define __TFT_LIB__

#include <cstdint>
#include <endian.h>
// #include "stm32f407xx.h"

// struct PIN{
//     GPIO_TypeDef* port;
//     uint32_t mask;   
// };

// struct IOConfig {
//     SPI_TypeDef* spi;
//     PIN cs, dc, rst, blk;          // rst, blk: port == nullptr значит «нет»
//     void (*delayMs)(uint32_t);
// };

class TFT_ILI9488{
public:
    // в файле проекта нужно будет "сконструировать объект", передав в конструктор функции конкретных действий 
    //! проверить сигнатуры для tx и rx
    TFT_ILI9488(uint8_t(*spi_tx_funx)(const uint8_t, const uint8_t*, uint16_t),
                uint8_t(*spi_rx_funx)(const uint8_t, uint8_t*, uint16_t), 
                void(*select_disp_funx) (const bool), void(*set_led_funx) (const bool),
                void(*set_DC_funx) (const bool), void(*set_reset_funx) (const bool), 
                void(*spi_rst_funx) (), void(*dellay_funx)(uint32_t)):
                spi_transmit(spi_tx_funx), spi_receive(spi_rx_funx), spi_cs(select_disp_funx),
                spi_led(set_led_funx), spi_dc(set_DC_funx), tft_rst(set_reset_funx),
                spi_rst(spi_rst_funx), wait(dellay_funx)   {}

    void init();
    void txByte(uint8_t data);
    void txWindow(uint16_t )

    enum class CMD: uint8_t{
        RESET_SW = 0,
        COL_ADR,            //адрес столбца
        LINE_ADR,           //адрес строки
        WINDOW_WRITE,       //Запись в окно с начала
        CONT_WINDOW_WRITE,  //Продолжить запись без сброса адреса.

        FPS,                //установка фпс

    //чтение с дисплея
        READ_ID4,
        READ_DISP_ID,


        CNT
    };
private:
    enum class MAJOR_STATE : uint8_t {
        MAIN = 1,
        TRANSMIT,
        RECIEVE,
        PROCESS_COMMUNICATE,
        DISCONNECT,

        CNT
    };

    enum class MINOR_STATE : uint8_t {
        INIT = 0,
        DEFOULT,
        
        
        CNT
    };

    enum class DISCONNECT_STATE : uint8_t {
        LOST_CONNECT = 0,
        CHECK,
        CONNECTED,

        CNT
    };

    struct __attribute__((packed)) TxData_t{
        uint8_t cmd;
        uint8_t par1;
        uint8_t par2;
    };

//* сервесные методы
    void setCmd(uint16_t cmd);

//* КОНСТАНТЫ
    const uint16_t MAX_COL_ADR = 319;
    const uint16_t MAX_LINE_ADR = 479;

static constexpr uint8_t   COMMANDS[uint8_t(CMD::CNT)] = {
        [uint8_t(CMD::RESET_SW)] = 0x01,
        [uint8_t(CMD::COL_ADR)] = 0x2A,
        [uint8_t(CMD::LINE_ADR)] = 0x2B,
        [uint8_t(CMD::WINDOW_WRITE)] = 0x2C,
        [uint8_t(CMD::CONT_WINDOW_WRITE)] = 0x3C,
        [uint8_t(CMD::FPS)] = 0xB1,                 //описание на 221 стр
        [uint8_t(CMD::READ_ID4)] = 0x3D,
        [uint8_t(CMD::READ_DISP_ID)] = 0x04,
    };
    
    uint8_t(* const spi_transmit)(const uint8_t addr, const uint8_t* data, 
                                  uint16_t size) = nullptr;
    uint8_t(* const spi_receive)(const uint8_t addr, uint8_t* data, 
                                 uint16_t size) = nullptr;
    void(* const spi_cs)(const bool state) = nullptr;   //0 - выбран, 1 - откл
    void(* const spi_led)(const bool state) = nullptr;  //0-выкл, 1-вкл
    void(* const spi_dc)(const bool state) = nullptr;   //передача 0-команды, 1- параметра или данных
    void(* const spi_rst)() = nullptr;  //? хз
    void(* const tft_rst)(const bool state) = nullptr;  //? хз
    void(* const wait)(uint32_t t_ms) = nullptr;
};





#endif