#pragma once

#include "kl_lib.h"
#include "ch.h"
#include "board.h"

class Dac_t {
private:
    const Spi_t ISpi;
    const GPIO_TypeDef *SpiGpio, *CSGpio;
    const uint16_t Sck, Mosi, Cs;
    const AlterFunc_t IAlterfunc;
    void CsHi() { PinSetHi((GPIO_TypeDef*)CSGpio, Cs); }
    void CsLo() { PinSetLo((GPIO_TypeDef*)CSGpio, Cs); }
public:
    // SPI, SpiGPIO, SckPin, MosiPin, CSGPIO, CSPin
    Dac_t(SPI_TypeDef *ASpi,
            GPIO_TypeDef *ASpiGpio, uint16_t ASck, uint16_t AMosi, AlterFunc_t AAlterfunc,
            GPIO_TypeDef *ACSGpio, uint16_t ACs) :
                ISpi(ASpi), SpiGpio(ASpiGpio), CSGpio(ACSGpio),
                        Sck(ASck), Mosi(AMosi), Cs(ACs), IAlterfunc(AAlterfunc) {}
    void Init();
    void Set(uint16_t AData);
};
