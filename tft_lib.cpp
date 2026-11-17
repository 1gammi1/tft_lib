#include "tft_lib.hpp"
#include <cstdint>
#include <string.h>

bool TFT_ILI9488::init(){
    memset(&txbuff, 0, sizeof(txbuff));
    memset(&rxbuff, 0, sizeof(rxbuff));


    return false;
}

void TFT_ILI9488::setCMD(CMD cmd){
    txcmd = cmd;
}

void TFT_ILI9488::sendWindow(WindCoordinates coordinate, COLOR_PAC_3B_t (*pixels_buf)[(coordinate.x1 - coordinate.x0) * (coordinate.y1 - coordinate.y0)]){
    
}