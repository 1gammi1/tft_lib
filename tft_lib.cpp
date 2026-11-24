#include "tft_lib.hpp"
#include <cstdint>
#include <string.h>


void TFT_ILI9488::TFT_WriteCommand(uint8_t cmd)
{
    tft_cs(0);
    tft_dc(DC_MODE_t::CMD);
    spi_transmit(&cmd, sizeof(cmd));
    tft_cs(1);
}

void TFT_ILI9488::TFT_WriteData(uint8_t *data, uint16_t size){
    tft_cs(0);
    tft_dc(DC_MODE_t::DATA);
    spi_transmit(data, size);
    tft_cs(1);
}

void TFT_ILI9488::TFT_Reset(){
    tft_rst(0);
    wait(20);
    tft_rst(1);
    wait(120);
}

void TFT_ILI9488::TFT_Init(){
    TFT_Reset();

    // Software Reset
    TFT_WriteCommand(COMMANDS[uint8_t(CMD::RESET_SW)]);
    wait(120);
    //todo убрать лишнее если оно есть
    // Power Control 1
    TFT_WriteCommand(0xC0);
    uint8_t c0[] = {0x17, 0x15};
    TFT_WriteData(c0, sizeof(c0));

    // Power Control 2
    TFT_WriteCommand(0xC1);
    uint8_t c1[] = {0x41};
    TFT_WriteData(c1, sizeof(c1));

    // VCOM Control
    TFT_WriteCommand(0xC5);
    uint8_t c5[] = {0x00, 0x12, 0x80};
    TFT_WriteData(c5, sizeof(c5));

    // Memory Access Control
    // MX = 1, BGR = 1
    TFT_WriteCommand(0x36);
    uint8_t madctl[] = {0x48};
    TFT_WriteData(madctl, sizeof(madctl));

    // Pixel Format: RGB111
    TFT_WriteCommand(0x3A);
    uint8_t pixel_format[] = {0x61};
    TFT_WriteData(pixel_format, sizeof(pixel_format));

    // Interface Mode Control
    TFT_WriteCommand(0xB0);
    uint8_t b0[] = {0x00};
    TFT_WriteData(b0, sizeof(b0));

    // Display Function Control
    TFT_WriteCommand(0xB6);
    uint8_t b6[] = {0x02, 0x02};
    TFT_WriteData(b6, sizeof(b6));

    // Sleep Out
    TFT_WriteCommand(0x11);
    wait(120);

    // Display ON
    TFT_WriteCommand(0x29);
    wait(20);

    Prod_ID_t* ID_pt = &ID;
    tft_cs(0);
    tft_dc(DC_MODE_t::CMD);
    spi_receive(COMMANDS[uint8_t(CMD::READ_DISP_ID)], castptr<uint8_t>(ID_pt), sizeof(Prod_ID_t));
}

void TFT_ILI9488::TFT_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
    uint8_t data[4];

    TFT_WriteCommand(COMMANDS[uint8_t(CMD::COL_ADR)]);
    data[0] = x0 >> 8;
    data[1] = x0 & 0xFF;
    data[2] = x1 >> 8;
    data[3] = x1 & 0xFF;
    TFT_WriteData(data, sizeof(data));

    TFT_WriteCommand(COMMANDS[uint8_t(CMD::LINE_ADR)]);
    data[0] = y0 >> 8;
    data[1] = y0 & 0xFF;
    data[2] = y1 >> 8;
    data[3] = y1 & 0xFF;
    TFT_WriteData(data, sizeof(data));

    TFT_WriteCommand(COMMANDS[uint8_t(CMD::WINDOW_WRITE)]);
}

void TFT_ILI9488::TFT_FillScreen(const COLOR_PAC_3B_t color_pac){
    TFT_SetAddressWindow(0, 0,MAX_COL_ADR,MAX_LINE_ADR);

    uint8_t buffer[256];

    for (int i = 0; i < 256; i++){
        memcpy(&buffer[i], &color_pac, sizeof(uint8_t));
    }

    uint32_t remaining_pixels = MAX_COL_ADR * MAX_LINE_ADR;
    tft_cs(0);
    tft_dc(DC_MODE_t::DATA);

    while (remaining_pixels > 0){
        uint16_t pixels = remaining_pixels > 256 ? 256 : remaining_pixels;

        spi_transmit(buffer, pixels);

        remaining_pixels -= pixels;
    }
    tft_cs(1);
}