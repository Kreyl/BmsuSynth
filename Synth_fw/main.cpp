#include "board.h"
#include "ch.h"
#include "hal.h"
#include "uart.h"
#include "led.h"
#include "kl_lib.h"
#include "MsgQ.h"
//#include "main.h"

#if 1 // ======================== Variables and defines ========================
// Forever
EvtMsgQ_t<EvtMsg_t, MAIN_EVT_Q_LEN> EvtQMain;
static const UartParams_t CmdUartParams(115200, CMD_UART_PARAMS);
static const UartParams_t CmdUart485Params(115200, CMD_UART485_PARAMS);
CmdUart_t Uart{&CmdUartParams};
CmdUart485_t Uart485{&CmdUart485Params, UART485_RXTX_PIN};
static void ITask();
static void OnCmd(Shell_t *PShell);

static Spi_t Spi1{SPI1};
static Spi_t Spi2{SPI2};
static Spi_t SpiDAC{SPI3};

// ==== Timers ====
//static TmrKL_t TmrEverySecond {MS2ST(1000), evtIdEverySecond, tktPeriodic};
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
    Spi1.Setup(boMSB, cpolIdleLow, cphaFirstEdge, 1000000);
    Spi1.Enable();

    // Spi2
    PinSetupOut      (SPI2_CS,   omPushPull);
    PinSetupAlterFunc(SPI2_SCK,  omPushPull, pudNone, SPI2_AF);
    PinSetupAlterFunc(SPI2_MISO, omPushPull, pudNone, SPI2_AF);
    PinSetupAlterFunc(SPI2_MOSI, omPushPull, pudNone, SPI2_AF);
    PinSetHi(SPI2_CS);
    // MSB first, master, ClkLowIdle, FirstEdge, Baudrate no more than 6.5MHz
    Spi2.Setup(boMSB, cpolIdleLow, cphaFirstEdge, 1000000);
    Spi2.Enable();

    // SpiDAC
    PinSetupOut      (SPI3_CS,   omPushPull);
    PinSetupAlterFunc(SPI3_SCK,  omPushPull, pudNone, SPI3_AF);
    PinSetupAlterFunc(SPI3_MOSI, omPushPull, pudNone, SPI3_AF);
    PinSetHi(SPI3_CS);
    // MSB first, master, ClkLowIdle, FirstEdge, Baudrate no more than 6.5MHz
    SpiDAC.Setup(boMSB, cpolIdleLow, cphaFirstEdge, 1000000);
    SpiDAC.Enable();


    // ==== Time and timers ====
//    TmrEverySecond.StartOrRestart();

    // Main cycle
    ITask();
}

__noreturn
void ITask() {
    while(true) {
        EvtMsg_t Msg = EvtQMain.Fetch(TIME_INFINITE);
        switch(Msg.ID) {
//            case evtIdEverySecond:
//                TimeS++;
//                ReadAndSetupMode();
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
    __attribute__((unused)) int32_t dw32 = 0;  // May be unused in some configurations
//    Uart.Printf("%S\r", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) {
        PShell->Ack(retvOk);
    }
    else if(PCmd->NameIs("Version")) PShell->Print("%S %S\r", APP_NAME, XSTRINGIFY(BUILD_TIME));

    else PShell->Ack(retvCmdUnknown);
}
#endif
