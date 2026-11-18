#ifndef __TFT_LIB__
#define __TFT_LIB__

#include <cstdint>

enum class DC_MODE_t : uint8_t{
        CMD = 0,
        PARAMS,
        DATA = uint8_t(PARAMS)
    };

enum class COLOR_3B_t : uint8_t{
        BLACK = 0,
        RED,
        GREEN,
        BLUE,
        YELLOW,
        CYAN,       //голубой
        MAGENTA,     //розовый
        WHITE,

        CNT
    };

struct __attribute__((packed)) COLOR_PAC_3B_t{
    uint8_t r :1 = 0;
    uint8_t g :1 = 0;
    uint8_t b :1 = 0;
    
};

static constexpr COLOR_PAC_3B_t COLORS_3B_PAR[uint8_t(COLOR_3B_t::CNT)] = {
        [uint8_t(COLOR_3B_t::BLACK)] = {0,0,0},
        [uint8_t(COLOR_3B_t::RED)] = {1,0,0},
        [uint8_t(COLOR_3B_t::GREEN)] = {0,1,0},
        [uint8_t(COLOR_3B_t::BLUE)] = {0,0,1},
        [uint8_t(COLOR_3B_t::YELLOW)] = {1,1,0},
        [uint8_t(COLOR_3B_t::CYAN)] = {0,1,1},
        [uint8_t(COLOR_3B_t::MAGENTA)] = {1,0,1},
        [uint8_t(COLOR_3B_t::WHITE)] = {1,1,1},
    };

class TFT_ILI9488{
public:
    // в файле проекта нужно будет "сконструировать объект", передав в конструктор функции конкретных действий 
    //! проверить сигнатуры для tx и rx
    TFT_ILI9488(uint8_t(*spi_tx_funx)( const uint8_t*, uint16_t),
                uint8_t(*spi_rx_funx)(const uint8_t, uint8_t*, uint16_t), 
                void(*select_disp_funx) (const bool), void(*set_led_funx) (const bool),
                void(*set_DC_funx) (const DC_MODE_t), void(*set_reset_funx) (const bool), 
                void(*spi_rst_funx) (), void(*dellay_funx)(uint32_t)):
                spi_transmit(spi_tx_funx), spi_receive(spi_rx_funx), tft_cs(select_disp_funx),
                tft_led(set_led_funx), tft_dc(set_DC_funx), tft_rst(set_reset_funx),
                spi_rst(spi_rst_funx), wait(dellay_funx)   {}

    ///@brief инициализация дисплея
    void TFT_Init();
    // заливка цветом
    void TFT_FillScreen(const COLOR_PAC_3B_t color_pac);

    enum class CMD: uint8_t{
        RESET_SW = 0,
    //команды инициализации 
        SET_PIXEL_FORMAT,   //3bit - 0x01, 16bit - 0x05, 18bit - 0x06
        FPS,                //установка фпс

    //команды вывода
        COL_ADR,            //адрес столбца
        LINE_ADR,           //адрес строки
        WINDOW_WRITE,       //Запись в окно с начала
        CONT_WINDOW_WRITE,  //Продолжить запись без сброса адреса.

    //чтение с дисплея
        READ_ID4,
        READ_DISP_ID,


        CNT
    };

    



private:
    enum class FSM_STATE : uint8_t {
        INIT = 0,
        IDLE,
        TRANSMIT,
        PROCESS_COMMUNICATE,
        DISCONNECT,

        CNT
    };


    struct __attribute__((packed)) TxData_t{
        uint8_t cmd;
        uint8_t par1;
        uint8_t par2;
    };

    struct __attribute__((packed)) WindCoordinates{
        uint16_t x0;
        uint16_t y0;
        uint16_t x1;
        uint16_t y1;
    };

static constexpr uint8_t   COMMANDS[uint8_t(CMD::CNT)] = {
        [uint8_t(CMD::RESET_SW)] = 0x01,
        [uint8_t(CMD::SET_PIXEL_FORMAT)] = 0x3A,    // 121 стр
        [uint8_t(CMD::FPS)] = 0xB1,                 //описание на 221 стр
        [uint8_t(CMD::COL_ADR)] = 0x2A,
        [uint8_t(CMD::LINE_ADR)] = 0x2B,
        [uint8_t(CMD::WINDOW_WRITE)] = 0x2C,
        [uint8_t(CMD::CONT_WINDOW_WRITE)] = 0x3C,
        [uint8_t(CMD::READ_ID4)] = 0x3D,
        [uint8_t(CMD::READ_DISP_ID)] = 0x04,
    };
    

    
    uint8_t(* const spi_transmit)(const uint8_t* data, 
                                  uint16_t size) = nullptr;
    uint8_t(* const spi_receive)(const uint8_t addr, uint8_t* data, 
                                 uint16_t size) = nullptr;
    void(* const tft_cs)(const bool state) = nullptr;   //0 - выбран, 1 - откл
    void(* const tft_led)(const bool state) = nullptr;  //0-выкл, 1-вкл
    void(* const tft_dc)(const DC_MODE_t state) = nullptr;   //передача 0-команды, 1- параметра или данных
    void(* const spi_rst)() = nullptr;  //? хз
    void(* const tft_rst)(const bool state) = nullptr;  //? хз
    void(* const wait)(uint32_t t_ms) = nullptr;

//* КОНСТАНТЫ
    static constexpr uint16_t MAX_LINE_ADR = 319;
    static constexpr uint16_t MAX_COL_ADR = 479;
    static constexpr uint8_t ACTIVE_WIND_WEIGH = 255;
    static constexpr uint8_t ACTIVE_WIND_HEIGH = 169;

    //* сервисные методы
    ///@brief отправка одной команды
    void TFT_WriteCommand(const uint8_t cmd);

    ///@brief отправкамассива данных
    void TFT_WriteData(uint8_t *data, uint16_t size);

    ///@brief перезапуск дисплея
    void TFT_Reset();

    // Установка области рисования
    void TFT_SetAddressWindow(const uint16_t x0, const uint16_t y0, const uint16_t x1, const uint16_t y1);
    
    



    //*Переменные
    CMD actualcmd = CMD::CNT;//?rm
    CMD txcmd = CMD::CNT;

    uint8_t txbuff[sizeof(TxData_t)] = {0};
    uint8_t rxbuff[sizeof(uint16_t)] = {0};

    uint32_t ts_ms = 0;
    uint16_t txsize = 0;

    COLOR_PAC_3B_t active_window_pixels_buf[UINT8_MAX];
    
};

/*Главное, что было придумано:
    отрисовка происходит по столбцам, в буффер загружаются столбцы поочередно

*/



#endif