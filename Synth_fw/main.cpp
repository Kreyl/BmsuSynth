#include "board.h"
#include "ch.h"
#include "hal.h"
#include "uart.h"
#include "led.h"
#include "kl_lib.h"
#include "MsgQ.h"
#include "dac8531.h"
#include "ADF5356.h"
#include "DAT31.h"
#include "SaveToFlash.h"

#if 1 // ======================== Variables and defines ========================
// Forever
EvtMsgQ_t<EvtMsg_t, MAIN_EVT_Q_LEN> EvtQMain;
static const UartParams_t CmdUartParams(115200, CMD_UART_PARAMS);
static const UartParams_t CmdUart485Params(115200, CMD_UART485_PARAMS);
CmdUart_t Uart{&CmdUartParams};
CmdUart485_t Uart485{&CmdUart485Params, UART485_RXTX_PIN};
static void ITask();
static void OnCmd(Shell_t *PShell);

#define ID_MIN      1
#define ID_MAX      18
#define ID_DEFAULT  1
uint32_t ID = ID_DEFAULT;
static uint8_t ISetID(int32_t NewID);
void ReadIDfromFlash();

uint32_t AdfRegBuf[ADF_REG_CNT];
ADF5356_t Synth(SPI1, GPIOA, 5, 7, AF5, GPIOA, 4);
Dac_t Dac(SPI3, GPIOB, 3, 5, AF6, GPIOB, 6);
DAT31_t Att(GPIOB, 13, 15, GPIOB, 12);

// ==== Timers ====
//static TmrKL_t TmrEverySecond {MS2ST(1000), evtIdEverySecond, tktPeriodic};
#endif

int main(void) {
    // ==== Init Vcore & clock system ====
//    Clk.SetVoltageRange(mvrHiPerf);
    Clk.SetCoreClk(cclk16MHz);
//    Clk.SetCoreClk(cclk48MHz);
    Clk.UpdateFreqValues();

    // === Init OS ===
    halInit();
    chSysInit();
    EvtQMain.Init();

    // ==== Init hardware ====
    Uart.Init();
    ReadIDfromFlash();
    Printf("\r%S %S; ID: %u\r", APP_NAME, XSTRINGIFY(BUILD_TIME), ID);
    Clk.PrintFreqs();

    Uart485.Init();

    Synth.Init();
    Dac.Init();
    Att.Init();

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
        uint16_t FValue;
        if(PCmd->GetNext<uint32_t>(&FID) == retvOk and FID == ID) {
            if(PCmd->GetNext<uint16_t>(&FValue) == retvOk) {
                Dac.Set(FValue);
                PShell->Ack(retvOk);
            }
            else PShell->Ack(retvBadValue);
        } // if ID
    }

#if 1 // ============ Synth ============
    else if(PCmd->NameIs("SetAdf14")) {
        if(PCmd->GetNext<uint32_t>(&FID) == retvOk and FID == ID) {
            if(PCmd->GetArray<uint32_t>(AdfRegBuf, ADF_REG_CNT) == retvOk) {
                Synth.Write14Regs(AdfRegBuf);
                PShell->Ack(retvOk);
            }
            else PShell->Ack(retvBadValue);
        } // if ID
    }

    else if(PCmd->NameIs("SetAdf5")) {
        if(PCmd->GetNext<uint32_t>(&FID) == retvOk and FID == ID) {
            if(PCmd->GetArray<uint32_t>(AdfRegBuf, 5) == retvOk) {
                Synth.Write5Regs(AdfRegBuf);
                PShell->Ack(retvOk);
            }
            else PShell->Ack(retvBadValue);
        } // if ID
    }

    else if(PCmd->NameIs("SyntL1")) {
        if(PCmd->GetNext<uint32_t>(&FID) == retvOk and FID == ID) {
            Synth.Write14Regs(ADFRegs_L1);
            PShell->Ack(retvOk);
        }
    }
    else if(PCmd->NameIs("SyntL2")) {
        if(PCmd->GetNext<uint32_t>(&FID) == retvOk and FID == ID) {
            Synth.Write14Regs(ADFRegs_L2);
            PShell->Ack(retvOk);
        }
    }
    else if(PCmd->NameIs("SyntL5")) {
        if(PCmd->GetNext<uint32_t>(&FID) == retvOk and FID == ID) {
            Synth.Write14Regs(ADFRegs_L5);
            PShell->Ack(retvOk);
        }
    }
#endif

    else if(PCmd->NameIs("SetAtt")) {
        if(PCmd->GetNext<uint32_t>(&FID) == retvOk and FID == ID) {
            uint8_t FValue;
            if(PCmd->GetNext<uint8_t>(&FValue) == retvOk) {
                Att.Write(FValue);
                PShell->Ack(retvOk);
            }
            else PShell->Ack(retvBadValue);
        } // if ID
    }

    else if(PCmd->NameIs("GetID")) PShell->Reply("ID", ID);

    else if(PCmd->NameIs("SetID")) {
        int32_t FID;
        if(PCmd->GetNext<int32_t>(&FID) != retvOk) { PShell->Ack(retvCmdError); return; }
        PShell->Ack(ISetID(FID));
    }



    else PShell->Ack(retvCmdUnknown);
}
#endif

#if 1 // =========================== ID management =============================
void ReadIDfromFlash() {
    Flash::Load(&ID, sizeof(ID)); // Read device ID
    if(ID < ID_MIN or ID > ID_MAX) {
        Printf("\rUsing default ID\r");
        ID = ID_DEFAULT;
    }
}

uint8_t ISetID(int32_t NewID) {
    if(NewID < ID_MIN or NewID > ID_MAX) return retvFail;
    uint8_t rslt = Flash::Save(&NewID, sizeof(NewID));
    if(rslt == retvOk) {
        ID = NewID;
        Printf("New ID: %u\r", ID);
        return retvOk;
    }
    else {
        Printf("Flash Write error: %u\r", rslt);
        return retvFail;
    }
}
#endif
