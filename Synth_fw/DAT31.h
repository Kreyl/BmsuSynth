/*
 * DAT31.h
 *
 *  Created on: 25 рту. 2019 у.
 *      Author: Kreyl
 */

#pragma once

class DAT31_t {
private:
    const GPIO_TypeDef *SpiGpio, *CSGpio;
    const uint16_t Sck, Mosi, Cs;
    void CsHi() { PinSetHi((GPIO_TypeDef*)CSGpio, Cs); }
    void CsLo() { PinSetLo((GPIO_TypeDef*)CSGpio, Cs); }
public:
    // SpiGPIO, SckPin, MosiPin, CSGPIO, CSPin
    DAT31_t(GPIO_TypeDef *ASpiGpio, uint16_t ASck, uint16_t AMosi, GPIO_TypeDef *ACSGpio, uint16_t ACs) :
                SpiGpio(ASpiGpio), CSGpio(ACSGpio), Sck(ASck), Mosi(AMosi), Cs(ACs) {}

    void Init() {
        PinSetupOut((GPIO_TypeDef*)CSGpio, Cs, omPushPull);
        PinSetupOut((GPIO_TypeDef*)SpiGpio, Sck, omPushPull);
        PinSetupOut((GPIO_TypeDef*)SpiGpio, Mosi, omPushPull);
        PinSetLo((GPIO_TypeDef*)SpiGpio, Sck);
        CsLo(); // When LE is brought LOW, data in the shift register is latched.
    }

    // C16-C8-C4-C2-C1-0, 6 bit
    void Write(uint8_t AValue) {
        AValue <<= 3;
        CsLo();
        for(int i=0; i<6; i++) {
            PinSetLo((GPIO_TypeDef*)SpiGpio, Sck);
            if(AValue & 0x80) PinSetHi((GPIO_TypeDef*)SpiGpio, Mosi);
            else PinSetLo((GPIO_TypeDef*)SpiGpio, Mosi);
            AValue <<= 1;
            PinSetHi((GPIO_TypeDef*)SpiGpio, Sck);
        }
        __NOP();
        PinSetLo((GPIO_TypeDef*)SpiGpio, Sck);
        PinSetLo((GPIO_TypeDef*)SpiGpio, Mosi);
        CsHi();
        __NOP();
        CsLo();
    }
};
