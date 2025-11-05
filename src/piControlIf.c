// SPDX-FileCopyrightText: 2017-2023 KUNBUS GmbH
//
// SPDX-License-Identifier: MIT

/*!
 * Project: Pi Control
 * Demo source code for usage of piControl driver
 *
 * \file piControlIf.c
 *
 * \brief PI Control Interface
 */

/******************************************************************************/
/********************************  Includes  **********************************/
/******************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "piControlIf.h"

#include "piControl.h"

/******************************************************************************/
/******************************  Global Vars  *********************************/
/******************************************************************************/

int PiControlHandle_g = -1;

/******************************************************************************/
/*******************************  Functions  **********************************/
/******************************************************************************/

/***********************************************************************************/
/*!
 * @brief Open Pi Control Interface
 *
 * Initialize the Pi Control Interface
 *
 ************************************************************************************/
int piControlOpen(void)
{
	/* open handle if needed */
	if (PiControlHandle_g < 0) {
		PiControlHandle_g = open(PICONTROL_DEVICE, O_RDWR);
		if (PiControlHandle_g < 0) {
			fprintf(stderr, "Failed to open " PICONTROL_DEVICE ": %s\n",
				strerror(errno));
			return -1;
		}
	}

	return 0;
}

/***********************************************************************************/
/*!
 * @brief Close Pi Control Interface
 *
 * Clsoe the Pi Control Interface
 *
 ************************************************************************************/
void piControlClose(void)
{
	/* open handle if needed */
	if (PiControlHandle_g > 0) {
		close(PiControlHandle_g);
		PiControlHandle_g = -1;
	}
}

/***********************************************************************************/
/*!
 * @brief Reset Pi Control Interface
 *
 * Initialize the Pi Control Interface
 *
 ************************************************************************************/
int piControlReset(void)
{
	int ret;

	ret = piControlOpen();
	if (ret < 0)
		return ret;

	if (ioctl(PiControlHandle_g, KB_RESET, NULL) < 0)
		return -errno;

	return 0;
}

/***********************************************************************************/
/*!
 * @brief Wait for Reset of Pi Control Interface
 *
 * Wait for Reset of Pi Control Interface
 *
 ************************************************************************************/
int piControlWaitForEvent(void)
{
	int event;
	int ret;

	ret = piControlOpen();
	if (ret < 0)
		return ret;

	if (ioctl(PiControlHandle_g, KB_WAIT_FOR_EVENT, &event) < 0)
		return -errno;

	return event;
}

/***********************************************************************************/
/*!
 * @brief Get Processdata
 *
 * Gets Processdata from a specific position
 *
 * @param[in]   Offset
 * @param[in]   Length
 * @param[out]  pData
 *
 * @return Number of Bytes read or error if negative
 *
 ************************************************************************************/
int piControlRead(uint32_t Offset, uint32_t Length, uint8_t * pData)
{
	int BytesRead;
	int ret;

	ret = piControlOpen();
	if (ret < 0)
		return ret;

	/* seek */
	if (lseek(PiControlHandle_g, Offset, SEEK_SET) < 0)
		return -errno;
	/* read */
	BytesRead = read(PiControlHandle_g, pData, Length);
	if (BytesRead < 0)
		return -errno;

	return BytesRead;
}

/***********************************************************************************/
/*!
 * @brief Set Processdata
 *
 * Writes Processdata at a specific position
 *
 * @param[in]   Offset
 * @param[in]   Length
 * @param[out]  pData
 *
 * @return Number of Bytes written or error if negative
 *
 ************************************************************************************/
int piControlWrite(uint32_t Offset, uint32_t Length, uint8_t * pData)
{
	int BytesWritten;
	int ret;

	ret = piControlOpen();
	if (ret < 0)
		return ret;

	/* seek */
	if (lseek(PiControlHandle_g, Offset, SEEK_SET) < 0)
		return -errno;

        /* Write */
	BytesWritten = write(PiControlHandle_g, pData, Length);
	if (BytesWritten < 0)
		return -errno;

	return BytesWritten;
}

/***********************************************************************************/
/*!
 * @brief Get Device Info
 *
 * Get Description of connected devices.
 *
 * @param[in/out]   Pointer to one element of type SDeviceInfo.
 *
 * @return 0 on success
 *
 ************************************************************************************/
int piControlGetDeviceInfo(SDeviceInfo * pDev)
{
	int ret;

	ret = piControlOpen();
	if (ret < 0)
		return ret;

	if (ioctl(PiControlHandle_g, KB_GET_DEVICE_INFO, pDev) < 0)
		return -errno;

	return 0;
}

/***********************************************************************************/
/*!
 * @brief Get Device Info List
 *
 * Get Description of connected devices.
 *
 * @param[in/out]   Pointer to an array of 20 entries of type SDeviceInfo.
 *
 * @return Number of detected devices
 *
 ************************************************************************************/
int piControlGetDeviceInfoList(SDeviceInfo * pDev)
{
	int cnt;
	int ret;

	ret = piControlOpen();
	if (ret < 0)
		return ret;

	if (PiControlHandle_g < 0)
		return -ENODEV;

	cnt = ioctl(PiControlHandle_g, KB_GET_DEVICE_INFO_LIST, pDev);
	if (cnt < 0)
		return -errno;

	return cnt;
}

/***********************************************************************************/
/*!
 * @brief Get Bit Value
 *
 * Get the value of one bit in the process image.
 *
 * @param[in/out]   Pointer to SPIValue.
 *
 * @return 0 or error if negative
 *
 ************************************************************************************/
int piControlGetBitValue(SPIValue * pSpiValue)
{
	int ret;

	ret = piControlOpen();
	if (ret < 0)
		return ret;

	pSpiValue->i16uAddress += pSpiValue->i8uBit / 8;
	pSpiValue->i8uBit %= 8;

	if (ioctl(PiControlHandle_g, KB_GET_VALUE, pSpiValue) < 0)
		return -errno;

	return 0;
}

/***********************************************************************************/
/*!
 * @brief Set Bit Value
 *
 * Set the value of one bit in the process image.
 *
 * @param[in/out]   Pointer to SPIValue.
 *
 * @return 0 or error if negative
 *
 ************************************************************************************/
int piControlSetBitValue(SPIValue * pSpiValue)
{
	int ret;

	ret = piControlOpen();
	if (ret < 0)
		return ret;

	pSpiValue->i16uAddress += pSpiValue->i8uBit / 8;
	pSpiValue->i8uBit %= 8;

	if (ioctl(PiControlHandle_g, KB_SET_VALUE, pSpiValue) < 0)
		return -errno;

	return 0;
}

/***********************************************************************************/
/*!
 * @brief Get Variable Info
 *
 * Get the info for a variable.
 *
 * @param[in/out]   Pointer to SPIVariable.
 *
 * @return 0 or error if negative
 *
 ************************************************************************************/
int piControlGetVariableInfo(SPIVariable * pSpiVariable)
{
	int ret;

	ret = piControlOpen();
	if (ret < 0)
		return ret;

	if (ioctl(PiControlHandle_g, KB_FIND_VARIABLE, pSpiVariable) < 0)
		return -errno;

	return 0;
}

/***********************************************************************************/
/*!
 * @brief Reset a counter or encoder in a RevPi DI or DIO module
 *
 * The DIO and DI modules some of the inputs can be configured as counter or encoder.
 * When the module is turned on, both start with the value 0. They are implemented as
 * signed 32-bit integer variable. This means they count upwards to (2^15)-1 and
 * the flip to -(2^15). All counters/encoders can the reset to 0 at thw same time
 * with this command. The argument is a bitfield. If bit n is set to 1, the
 * counter/encoder on input n+1 will be reset. A value of 0xffff will reset all
 * counters/encoders.
 *
 * @param[in]   address		address of the module as displayed ba PiCtory or 'piTest -d'
 *		bitfield	bitfield defining the counters/endcoders to reset.
 *
 * @return      == 0    no error
 *		< 0     in case of error, errno will be set
 *
 ************************************************************************************/
int piControlResetCounter(int address, int bitfield)
{
	SDIOResetCounter tel;
	int ret;

	ret = piControlOpen();
	if (ret < 0)
		return ret;

	tel.i8uAddress = address;
	tel.i16uBitfield = bitfield;

	ret = ioctl(PiControlHandle_g, KB_DIO_RESET_COUNTER, &tel);
	if (ret < 0)
		perror("Counter reset not possible");

	return ret;
}

int piControlGetROCounters(int address)
{
	struct revpi_ro_ioctl_counters ioc;
	int ret;
	int i;

	ret = piControlOpen();
	if (ret < 0)
		return ret;

	ioc.addr = address;

	ret = ioctl(PiControlHandle_g, KB_RO_GET_COUNTER, &ioc);
	if (ret < 0) {
		perror("Failed to get RO counters");
		return ret;
	}

	printf("RO relay counters:\n");
	for (i = 0; i < REVPI_RO_NUM_RELAYS; i++)
		printf("     Relay %i: %u\n", i + 1, ioc.counter[i]);

	return ret;
}


/***********************************************************************************/
/*!
 * @brief Update firmware
 *
 * KUNBUS provides "*.fwu" files with new firmware for RevPi I/O and RevPi Gate modules.
 * These are provided in the debian paket revpi-firmware. Use 'sudo apt-get install revpi-firmware'
 * to get the latest firmware files. Afterwards you can update the firmware with this ioctl call.
 * Unforunatelly old modules hang or block the piBridge communication if a modules is updated.
 * Therefore the update is only possible when only one module is connected to the RevPi.
 * The module must be on the right side of the RevPi Core and on the left side of the RevPi Connect.
 * This ioctl reads the version number from the module and compares it to the lastet available
 * firmware file. If a new firmware is available, it is flashed to the module.
 * If forced update is specified the firmware is flashed even if its version number is equal to or
 * smaller than the one running on the target module. This requires a module address.
 *
 * @param[in]   addr_p		address of module to update. 0 for automatic selection of the module
 * 				to update.
 * @param[in]   force_update	skip the firmware version check.
 *
 * @return 0 or error if negative
 *
 ************************************************************************************/
int piControlUpdateFirmware(uint32_t addr_p, bool force_update, int hw_revision)
{
	int ret;

	ret = piControlOpen();
	if (ret < 0)
		return ret;

	printf("Updating Firmware%s!\n", force_update ? " (forced)" : "");
	printf("This can take a while. Do not switch off the system!\n");

	if (!addr_p) { /* only supported with legacy ioctl */
		if (force_update) {
			fprintf(stderr,
				"Error: no module address given for forced firmware update.\n");
			return -EINVAL;
		}
		ret = ioctl(PiControlHandle_g, KB_UPDATE_DEVICE_FIRMWARE, NULL);
		if (ret < 0) {
			fprintf(stderr, "Failed to update firmware of module with address %"
				PRIu32 ": %s\n", addr_p, strerror(errno));
			return -1;
		} else {
			printf("Firmware updated successfully.\n");
		}
	} else {
		struct picontrol_firmware_upload fwu;

		memset(&fwu, 0, sizeof(fwu));
		fwu.addr = addr_p;

		if (force_update)
			fwu.flags |= PICONTROL_FIRMWARE_FORCE_UPLOAD;
		if (hw_revision >= 0) {
			fwu.flags |= PICONTROL_FIRMWARE_RESCUE_MODE;
			fwu.rescue_mode_hw_revision = hw_revision;
			printf("Using firmware rescue mode with hw revision %u\n",
				hw_revision);
		}

		ret = ioctl(PiControlHandle_g, PICONTROL_UPLOAD_FIRMWARE, &fwu);
		if (ret < 0) {
			fprintf(stderr, "Failed to update firmware of module with address %"
				PRIu32 ": %s\n", addr_p, strerror(errno));
			return -1;
		} else if (ret == 0) {
			printf("Firmware for module with address %" PRIu32
				" updated successfully.\n", addr_p);
		} else if (ret == 1) {
			printf("Firmware of module with address %" PRIu32
				" is already up to date.\n", addr_p);
			printf("Use '--force' to force firmware update.\n");
		}
	}

	if (ret)
		return -errno;

	return 0;
}

/***********************************************************************************/
/*!
 * @brief Stop/Start I/O update
 *
 * This ioctl stops, starts or toggles the update of I/Os. If the I/O updates are stopped,
 * piControls writes 0 to the outputs instead of the values from the process image.
 * The input values are not written to the process images. The I/O communication is
 * runnging as normal. On the update of DIO, DI, DO, AIO, Gate modules and the RevPi
 * itself is stopped. There is no change in the handling of virtual modules.
 * The function can used for simulation of I/Os. A simulation application can be started
 * additionally to the other control and application processes. It stops the I/O update
 * and simulates the hardware by setting and reading the values in the process image.
 * The application does not notice this.
 *
 * @param[in]   stop==0	Start the I/O update
 *		stop==1	Stop the I/O update
 *		stop==2 Toggle the mode of I/O update
 *
 * @return 	0/1 the new state
 *		<0 in case of an error
 *
 ************************************************************************************/
int piControlStopIO(int stop)
{
	int ret;

	ret = piControlOpen();
	if (ret < 0)
		return ret;

	ret = ioctl(PiControlHandle_g, KB_STOP_IO, &stop);
	if (ret < 0)
		perror("ioctl(KB_STOP_IO) returned error");

	return ret;
}


/***********************************************************************************/
/*!
 * @brief Get a message from the last ioctl call.
 * 
 * Check, if the last ioctl() call produced a message and display it if necessary.
 * 
 ***********************************************************************************/
void piShowLastMessage(void)
{
	char cMsg[REV_PI_ERROR_MSG_LEN];
    
	if (ioctl(PiControlHandle_g, KB_GET_LAST_MESSAGE, cMsg) == 0 && cMsg[0])
		puts(cMsg);
}

int piControlCalibrate(int addr, int channl, int mode, int xval, int yval)
{
	struct pictl_calibrate cali;
	int ret;

	cali.address = addr;
	cali.mode = mode;
	cali.channels = channl;
	cali.x_val = xval;
	cali.y_val = yval;

	ret = piControlOpen();
	if (ret < 0)
		return ret;

	ret = ioctl(PiControlHandle_g, KB_AIO_CALIBRATE, &cali);
	return ret;
}
