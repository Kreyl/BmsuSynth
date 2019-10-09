/*
 * ADF5356.h
 *
 *  Created on: 25 àâã. 2019 ã.
 *      Author: Kreyl
 */

#pragma once

#define ADF_REG_CNT         14 // 0...13

#if 1 // =========================== Reg values set ============================
/* ==== L1 ====
  fout =5939,58; fvco =5939,58; fref =10 ÌÃö, fpfd=5 ÌÃö,  Step=0.001 ÌÃö
 fvco /fpfd= 1187.916= N,  INT=1187,  FRAC1=0.916*224=0.916*16777216=
15 367929, FRAC2=107, MOD2=125
*/
/*static const uint32_t ADFRegs_L1[ADF_REG_CNT] = {
        0x00304A30, // 0, // 1
        0x01AC07D2, // 2
        0x00000003, // 3
        0x32008984, // 4
        0x00800025, // 5
        0x35004476, // 6
        0x060000E7, // 7
        0x15596568, // 8
        0x04027CC9, // 9
        0x00C0033A, // 10
        0x0061200B, // 11
        0x000015FC, // 12
        0x0000000D, // 13
};*/

static const uint32_t ADFRegs_L1[ADF_REG_CNT] = {  //regs added by Notiel
		0x00302510, //0
		0x0F53f7C1, //1
		0x01D007D2, //2
		0x00000003, //3
		0x32008984, //4
		0x00800025, //5
		0x35008476, //6
		0x060000E7, //7
		0x15596568, //8
		0x07047CC9, //9
		0x00C0067A, //10
		0x0061200B, //11
		0x100005FC, //12
		0x0000000D, //13
};


/* ==== L2 ====
fout =6287,40; fvco =6287,40; fref =10 ÌÃö, fpfd=5ÌÃö, Step=0.001 ÌÃö
 fvco /fpfd= 1257.48= N,  INT=1257,  FRAC1=0.48*224=0.48*16777216=8 053063,
 FRAC2=17, MOD2=25
*/
/*static const uint32_t ADFRegs_L2[ADF_REG_CNT] = {
        0x00304E90, // 0
        0x07AE1471, // 1
        0x00440192, // 2
        0x00000003, // 3
        0x32008984, // 4
        0x00800025, // 5
        0x35004476, // 6
        0x060000E7, // 7
        0x15596568, // 8
        0x04027CC9, // 9
        0x00C0033A, // 10
        0x0061200B, // 11
        0x000015FC, // 12
        0x0000000D, // 13
};*/

static const uint32_t ADFRegs_L2[ADF_REG_CNT] = {  //regs added by Notiel
		0x00302740, //0
		0x0BD70A31, //1
		0x00540192, //2
		0x00000003, //3
		0x32008984, //4
		0x00800025, //5
		0x35008476, //6
		0x060000E7, //7
		0x15596568, //8
		0x07047CC9, //9
		0x00C0067A, //10
		0x0061200B, //11
		0x000015FC, //12
		0x0000000D, //13
};

/* ==== L5 ====
fout =6338,55; fvco =6338,55; fref =20 ÌÃö, fpfd=10 ÌÃö,  Step=0.001 ÌÃö
 fvco /fpfd= 633.855= N,  INT=633,  FRAC1=14344 519,FRAC2=17, MOD2=25
*/
/*static const uint32_t ADFRegs_L5[ADF_REG_CNT] = {
        0x00302790, // 0
        0x0DAE1471, // 1
        0x00440192, // 2
        0x00000003, // 3
        0x32008984, // 4
        0x00800025, // 5
        0x35008476, // 6
        0x060000E7, // 7
        0x15596568, // 8
        0x07047CC9, // 9
        0x00C0067A, // 10
        0x0061200B, // 11
        0x000015FC, // 12
        0x0000000D, // 13
};*/

static const uint32_t ADFRegs_L5[ADF_REG_CNT] = {  //regs added by Notiel
		0x00302790, //0
		0x0DAE1471, //1
		0x00440192, //2
		0x00000003, //3
		0x32008984, //4
		0x00800025, //5
		0x35008476, //6
		0x060000E7, //7
		0x15596568, //8
		0x07047CC9, //9
		0x00C0067A, //10
		0x0061200B, //11
		0x000015FC, //12
		0x0000000D, //13
};

#endif

class ADF5356_t {
private:
    const Spi_t ISpi;
    const GPIO_TypeDef *SpiGpio, *CSGpio;
    const uint16_t Sck, Mosi, Cs;
    const AlterFunc_t IAlterfunc;
    void CsHi() { PinSetHi((GPIO_TypeDef*)CSGpio, Cs); }
    void CsLo() { PinSetLo((GPIO_TypeDef*)CSGpio, Cs); }
    bool FirstWriteDone = false;

    void WriteReg(uint32_t AValue) {
        CsLo();
        ISpi.ReadWriteWord(0xFFFF & (AValue >> 16));
        ISpi.ReadWriteWord(0xFFFF & (AValue >>  0));
        CsHi();
    }
public:
    // SPI, SpiGPIO, SckPin, MosiPin, CSGPIO, CSPin
    ADF5356_t(SPI_TypeDef *ASpi,
            GPIO_TypeDef *ASpiGpio, uint16_t ASck, uint16_t AMosi, AlterFunc_t AAlterfunc,
            GPIO_TypeDef *ACSGpio, uint16_t ACs) :
                ISpi(ASpi), SpiGpio(ASpiGpio), CSGpio(ACSGpio),
                        Sck(ASck), Mosi(AMosi), Cs(ACs), IAlterfunc(AAlterfunc) {}

    void Init() {
        PinSetupOut((GPIO_TypeDef*)CSGpio, Cs, omPushPull);
        PinSetupAlterFunc((GPIO_TypeDef*)SpiGpio, Sck, omPushPull, pudNone, IAlterfunc);
        PinSetupAlterFunc((GPIO_TypeDef*)SpiGpio, Mosi, omPushPull, pudNone, IAlterfunc);
        CsHi();
        // ==== SPI ====    MSB first, master, ClkLowIdle, FirstEdge, freq up to 50 MHz
        ISpi.Setup(boMSB, cpolIdleLow, cphaFirstEdge, 50000000, bitn16);
        ISpi.Enable();
    }

    // Regs 0, 1, 2, ..., 13
    void Write14Regs(const uint32_t *PRegs) {
        // 13, 12, ..., 1
        for(int i=(ADF_REG_CNT-1); i>=1; i--) {
            WriteReg(PRegs[i]);
        }
        // Wait a little
        chThdSleepMilliseconds(1);
        WriteReg(PRegs[0]);
    }

    // Regs 0, 1, 2, 10, 13
    void Write5Regs(const uint32_t *PRegs) {
        WriteReg(PRegs[4]); // 13
        WriteReg(PRegs[3]); // 10
        WriteReg(PRegs[2]); // 2
        WriteReg(PRegs[1]); // 1
        // Wait a little
        chThdSleepMilliseconds(1);
        WriteReg(PRegs[0]); // 0
    }
};

