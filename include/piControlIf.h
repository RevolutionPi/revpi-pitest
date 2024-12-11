// SPDX-FileCopyrightText: 2017-2024 KUNBUS GmbH
//
// SPDX-License-Identifier: MIT

#ifndef PICONTROLIF_H_
#define PICONTROLIF_H_

/******************************************************************************/
/********************************  Includes  **********************************/
/******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <piControl.h>


/******************************************************************************/
/*********************************  Types  ************************************/
/******************************************************************************/

extern int PiControlHandle_g;


/******************************************************************************/
/*******************************  Prototypes  *********************************/
/******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

int piControlReset(void);
int piControlRead(uint32_t Offset, uint32_t Length, uint8_t *pData);
int piControlWrite(uint32_t Offset, uint32_t Length, uint8_t *pData);
int piControlGetDeviceInfo(SDeviceInfo *pDev);
int piControlGetDeviceInfoList(SDeviceInfo *pDev);
int piControlGetBitValue(SPIValue *pSpiValue);
int piControlSetBitValue(SPIValue *pSpiValue);
int piControlGetVariableInfo(SPIVariable *pSpiVariable);
int piControlFindVariable(const char *name);
int piControlResetCounter(int address, int bitfield);
int piControlGetROCounters(int address);
int piControlWaitForEvent(void);
int piControlUpdateFirmware(uint32_t addr_p, bool force_update);
int piControlStopIO(int stop);
void piShowLastMessage(void);
int piControlCalibrate(int addr, int channl, int mode, int xval, int yval);

void piControlClose(void);

#ifdef __cplusplus
}
#endif

#endif /* PICONTROLIF_H_ */
