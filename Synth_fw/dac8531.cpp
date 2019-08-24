#include "dac8531.h"
#include "uart.h"

void Dac_t::Init() {
    PinSetupOut((GPIO_TypeDef*)CSGpio, Cs, omPushPull);
    PinSetupAlterFunc((GPIO_TypeDef*)SpiGpio, Sck, omPushPull, pudNone, IAlterfunc);
    PinSetupAlterFunc((GPIO_TypeDef*)SpiGpio, Mosi, omPushPull, pudNone, IAlterfunc);
    CsHi();
    // ==== SPI ====    MSB first, master, ClkLowIdle, FirstEdge, freq up to 30 MHz
    ISpi.Setup(boMSB, cpolIdleLow, cphaSecondEdge, 30000000);
    ISpi.Enable();
}

void Dac_t::Set(uint16_t AData) {
    CsLo();
    ISpi.ReadWriteByte(0);
    ISpi.ReadWriteByte((AData>>8) & 0xFF);
    ISpi.ReadWriteByte(AData & 0xFF);
    CsHi();
}
