#include "board.h"
#include "ch.h"
#include "hal.h"
#include "uart.h"
#include "led.h"
#include "kl_lib.h"
#include "MsgQ.h"
#include "dac8531.h"

#if 1 // ======================== Variables and defines ========================
// Forever
EvtMsgQ_t<EvtMsg_t, MAIN_EVT_Q_LEN> EvtQMain;
static const UartParams_t CmdUartParams(115200, CMD_UART_PARAMS);
static const UartParams_t CmdUart485Params(115200, CMD_UART485_PARAMS);
CmdUart_t Uart{&CmdUartParams};
CmdUart485_t Uart485{&CmdUart485Params, UART485_RXTX_PIN};
static void ITask();
static void OnCmd(Shell_t *PShell);

uint32_t ID = 1;

static Spi_t SpiSynth{SPI1};
static Spi_t SpiAtt{SPI2};
static Spi_t SpiDAC{SPI3};

Dac_t Dac(SPI3, GPIOB, 3, 5, AF6, GPIOB, 6);

// ==== Timers ====
//static TmrKL_t TmrEverySecond {MS2ST(1000), evtIdEverySecond, tktPeriodic};
//static TmrKL_t TmrEvery10ms {TIME_MS2I(10), evtId10ms, tktPeriodic}; // XXX
#endif

int main(void) {
    // ==== Init Vcore & clock system ====
    Clk.SetCoreClk(cclk16MHz);
//    Clk.SetCoreClk(cclk48MHz);
    Clk.UpdateFreqValues();

    // === Init OS ===
    halInit();
    chSysInit();
    EvtQMain.Init();

    // ==== Init hardware ====
    Uart.Init();
    Printf("\r%S %S\r", APP_NAME, XSTRINGIFY(BUILD_TIME));
    Clk.PrintFreqs();

    Uart485.Init();

    // Spi1
    PinSetupOut      (SPI1_CS,   omPushPull);
    PinSetupAlterFunc(SPI1_SCK,  omPushPull, pudNone, SPI1_AF);
    PinSetupAlterFunc(SPI1_MISO, omPushPull, pudNone, SPI1_AF);
    PinSetupAlterFunc(SPI1_MOSI, omPushPull, pudNone, SPI1_AF);
    PinSetHi(SPI1_CS);
    // MSB first, master, ClkLowIdle, FirstEdge, Baudrate no more than 6.5MHz
    SpiSynth.Setup(boMSB, cpolIdleLow, cphaFirstEdge, 1000000);
    SpiSynth.Enable();

    // Spi2
    PinSetupOut      (SPI2_CS,   omPushPull);
    PinSetupAlterFunc(SPI2_SCK,  omPushPull, pudNone, SPI2_AF);
    PinSetupAlterFunc(SPI2_MISO, omPushPull, pudNone, SPI2_AF);
    PinSetupAlterFunc(SPI2_MOSI, omPushPull, pudNone, SPI2_AF);
    PinSetHi(SPI2_CS);
    // MSB first, master, ClkLowIdle, FirstEdge, Baudrate no more than 6.5MHz
    SpiAtt.Setup(boMSB, cpolIdleLow, cphaFirstEdge, 1000000);
    SpiAtt.Enable();

    Dac.Init();


    // ==== Time and timers ====
//    TmrEverySecond.StartOrRestart();
//    TmrEvery10ms.StartOrRestart(); // XXX

    // Main cycle
    ITask();
}

//uint16_t DacV = 0; // XXX

__noreturn
void ITask() {
    while(true) {
        EvtMsg_t Msg = EvtQMain.Fetch(TIME_INFINITE);
        switch(Msg.ID) {
//            case evtIdEverySecond:
//                TimeS++;
//                ReadAndSetupMode();
//                break;

//            case evtId10ms: XXX
//                Dac.Set(DacV);
//                DacV += 1000;
//                break;

            case evtIdShellCmd:
                OnCmd((Shell_t*)Msg.Ptr);
                ((Shell_t*)Msg.Ptr)->SignalCmdProcessed();
                break;
            default: Printf("Unhandled Msg %u\r", Msg.ID); break;
        } // Switch
    } // while true
} // ITask()

#if 1 // ================= Command processing ====================
void OnCmd(Shell_t *PShell) {
	Cmd_t *PCmd = &PShell->Cmd;
	uint32_t FID;
    // Handle command
    if(PCmd->NameIs("Ping")) {
        if(PCmd->GetNext<uint32_t>(&FID) == retvOk and FID == ID) PShell->Ack(retvOk);
    }
    else if(PCmd->NameIs("Version")) {
        if(PCmd->GetNext<uint32_t>(&FID) == retvOk and FID == ID) {
            PShell->Print("%S %S\r", APP_NAME, XSTRINGIFY(BUILD_TIME));
        }
    }

    else if(PCmd->NameIs("SetDac")) {
        uint32_t FID;
        uint16_t FValue;
        if(PCmd->GetNext<uint32_t>(&FID) == retvOk and FID == ID) {
            if(PCmd->GetNext<uint16_t>(&FValue) == retvOk) {
                Dac.Set(FValue);
                PShell->Ack(retvOk);
                return;
            }
            else PShell->Ack(retvBadValue);
        } // if ID
    }



    else PShell->Ack(retvCmdUnknown);
}
#endif
