// SPDX-FileCopyrightText: 2016-2025 KUNBUS GmbH
//
// SPDX-License-Identifier: MIT

/*!
 * Project: piTest
 * Demo source code for usage of piControl driver
 *
 * \file piTest.c
 *
 * \brief PI Control Test Program
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <getopt.h>
#include <time.h>
#include <sys/types.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>

#include "piControlIf.h"
#include "piControl.h"

#define PROGRAM_VERSION		"2.0.0"

#define SEC_AS_USEC 1000000
#define NUM_SPINS_PER_SECOND 16

/* long option names */
# define MODULE_LONG_ARG_NAME "module"
# define FORCE_LONG_ARG_NAME "force"
# define ASSUME_YES_LONG_ARG_NAME "assume-yes"

/* long option indices */
# define MODULE_LONG_ARG_INDEX 0
# define FORCE_LONG_ARG_INDEX  1
# define ASSUME_YES_LONG_ARG_INDEX 2

/***********************************************************************************/
/*!
 * @brief Get message text for read error
 *
 * Get the message text for an error on read from control process.
 *
 * @param[in]   Error number.
 *
 * @return Pointer to the error message
 *
 ************************************************************************************/
char *getReadError(int error)
{
	static char *ReadError[] = {
		"Cannot connect to control process",
		"Offset seek error",
		"Cannot read from control process",
		"Unknown error"
	};
	switch (error) {
	case -1:
		return ReadError[0];
		break;
	case -2:
		return ReadError[1];
		break;
	case -3:
		return ReadError[2];
		break;
	default:
		return ReadError[3];
		break;
	}
}

/***********************************************************************************/
/*!
 * @brief Get message text for write error
 *
 * Get the message text for an error on write to control process.
 *
 * @param[in]   Error number.
 *
 * @return Pointer to the error message
 *
 ************************************************************************************/
char *getWriteError(int error)
{
	static char *WriteError[] = {
		"Cannot connect to control process",
		"Offset seek error",
		"Cannot write to control process",
		"Unknown error"
	};
	switch (error) {
	case -1:
		return WriteError[0];
		break;
	case -2:
		return WriteError[1];
		break;
	case -3:
		return WriteError[2];
		break;
	default:
		return WriteError[3];
		break;
	}
}

/***********************************************************************************/
/*!
 * @brief Get module type as string
 *
 *
 * @param[in]   module type number
 *
 * @return      Pointer to the string with the name
 *
 ************************************************************************************/
char *getModuleName(uint16_t moduletype)
{
	switch (moduletype) {
	case 95:
		return "RevPi Core";
	case 96:
		return "RevPi DIO";
	case 97:
		return "RevPi DI";
	case 98:
		return "RevPi DO";
	case 103:
		return "RevPi AIO";
	case 104:
		return "RevPi Compact";
	case 105:
		return "RevPi Connect";
	case 109:
		return "RevPi CON CAN";
	case 110:
		return "RevPi CON M-Bus";
	case 111:
		return "RevPi CON BT";
	case 118:
		return "RevPi MIO";
	case 135:
		return "RevPi Flat";
	case 136:
		return "RevPi Connect 4";
	case 137:
		return "RevPi RO";
	case 138:
		return "RevPi Connect 5";

	case PICONTROL_SW_MODBUS_TCP_SLAVE:
		return "ModbusTCP Slave Adapter";
	case PICONTROL_SW_MODBUS_RTU_SLAVE:
		return "ModbusRTU Slave Adapter";
	case PICONTROL_SW_MODBUS_TCP_MASTER:
		return "ModbusTCP Master Adapter";
	case PICONTROL_SW_MODBUS_RTU_MASTER:
		return "ModbusRTU Master Adapter";
	case PICONTROL_SW_PROFINET_CONTROLLER:
		return "Profinet Controller Adapter";
	case PICONTROL_SW_PROFINET_DEVICE:
		return "Profinet Device Adapter";
	case PICONTROL_SW_REVPI_SEVEN:
		return "RevPi7 Adapter";
	case PICONTROL_SW_REVPI_CLOUD:
		return "RevPi Cloud Adapter";

	case 71:
		return "Gateway CANopen";
	case 72:
		return "Gateway CC-Link";
	case 73:
		return "Gateway DeviceNet";
	case 74:
		return "Gateway EtherCAT";
	case 75:
		return "Gateway EtherNet/IP";
	case 76:
		return "Gateway Powerlink";
	case 77:
		return "Gateway Profibus";
	case 78:
		return "Gateway Profinet RT";
	case 79:
		return "Gateway Profinet IRT";
	case 80:
		return "Gateway CANopen Master";
	case 81:
		return "Gateway SercosIII";
	case 82:
		return "Gateway Serial";
	case 85:
		return "Gateway EtherCAT Master";
	case 92:
		return "Gateway ModbusRTU";
	case 93:
		return "Gateway ModbusTCP";
	case 100:
		return "Gateway DMX";

	default:
		return "unknown moduletype";
		break;
	}
}

/***********************************************************************************/
/*!
 * @brief Show device list
 *
 * Show all devices connected to control process and print their info data
 *
 ************************************************************************************/
int showDeviceList(void)
{
	int devcount;
	int dev;
	SDeviceInfo asDevList[REV_PI_DEV_CNT_MAX];

	// Get device info
	devcount = piControlGetDeviceInfoList(asDevList);
	if (devcount < 0) {
		return devcount;
	}

	printf("Found %d devices:\n\n", devcount);

	for (dev = 0; dev < devcount; dev++) {
		// Show device number, address and module type
		printf("Address: %d module type: %d (0x%x) %s V%d.%d\n", asDevList[dev].i8uAddress,
		       asDevList[dev].i16uModuleType, asDevList[dev].i16uModuleType,
		       getModuleName(asDevList[dev].i16uModuleType & PICONTROL_NOT_CONNECTED_MASK),
		       asDevList[dev].i16uSW_Major, asDevList[dev].i16uSW_Minor);

		if (asDevList[dev].i8uActive) {
			printf("Module is present\n");
		} else {
			if (asDevList[dev].i16uModuleType & PICONTROL_NOT_CONNECTED) {
				printf("Module is NOT present, data is NOT available!!!\n");
			} else {
				printf("Module is present, but NOT CONFIGURED!!!\n");
			}
		}

		// Show offset and length of input section in process image
		printf("     input offset: %d length: %d\n", asDevList[dev].i16uInputOffset,
		       asDevList[dev].i16uInputLength);

		// Show offset and length of output section in process image
		printf("    output offset: %d length: %d\n", asDevList[dev].i16uOutputOffset,
		       asDevList[dev].i16uOutputLength);
		printf("\n");
	}

	piShowLastMessage();

	return devcount;
}

/***********************************************************************************/
/*!
 * @brief Read data
 *
 * Read <length> bytes at a specific offset.
 *
 * @param[in]   Offset
 * @param[in]   Length
 *
 ************************************************************************************/
int readData(uint16_t offset, uint16_t length, bool cyclic, char format, bool quiet)
{
	int rc;
	uint8_t *pValues;
	int val;
	int line_len = 10;	// for decimal
	if (format == 'h')
		line_len = 16;
	else if (format == 'b')
		line_len = 4;

	// Get memory for the values
	pValues = malloc(length);
	if (pValues == NULL) {
		fprintf(stderr, "Not enough memory\n");
		return -ENOMEM;
	}

	do {
		rc = piControlRead(offset, length, pValues);
		if (rc < 0) {
			if (!quiet) {
				fprintf(stderr, "read error %s\n", getReadError(rc));
				if (!cyclic)
					return rc;
			}
		} else {
			for (val = 0; val < length; val++) {
				if (format == 'h') {
					printf("%02x ", pValues[val]);
				} else if (format == 'b') {
					printf("%c%c%c%c%c%c%c%c ",
						pValues[val] & 0x80 ? '1' : '0',
						pValues[val] & 0x40 ? '1' : '0',
						pValues[val] & 0x20 ? '1' : '0',
						pValues[val] & 0x10 ? '1' : '0',
						pValues[val] & 0x08 ? '1' : '0',
						pValues[val] & 0x04 ? '1' : '0',
						pValues[val] & 0x02 ? '1' : '0',
						pValues[val] & 0x01 ? '1' : '0');
				} else if (format == 's') {
					uint16_t ui;
					int16_t *psi;
					ui = pValues[val] + (pValues[val + 1] << 8);
					psi = (int16_t *) & ui;
					printf("%6d ", *psi);
					val++;
				} else {
					printf("%3d ", pValues[val]);
				}
				if ((val % line_len) == (line_len - 1))
					printf("\n");
			}
			if ((val % line_len) != 0)
				printf("\n");
		}
		if (cyclic)
			sleep(1);
	} while (cyclic);

	return 0;
}

/***********************************************************************************/
/*!
 * @brief Read variable value
 *
 * Read the value of a variable from process image
 *
 * @param[in]   Variable name
 *
 ************************************************************************************/
int readVariableValue(char *pszVariableName, bool cyclic, char format, bool quiet)
{
	int rc;
	SPIVariable sPiVariable;
	SPIValue sPIValue;
	uint8_t i8uValue;
	uint16_t i16uValue;
	uint32_t i32uValue;

	snprintf(sPiVariable.strVarName, sizeof(sPiVariable.strVarName), "%s", pszVariableName);
	rc = piControlGetVariableInfo(&sPiVariable);
	if (rc < 0) {
		fprintf(stderr, "Failed to find variable '%s'\n", pszVariableName);
		return rc;
	}
	if (sPiVariable.i16uLength == 1) {
		sPIValue.i16uAddress = sPiVariable.i16uAddress;
		sPIValue.i8uBit = sPiVariable.i8uBit;

		do {
			rc = piControlGetBitValue(&sPIValue);
			if (rc < 0) {
				fprintf(stderr, "Failed to get bit value\n");
				if (!cyclic)
					return rc;
			} else {
				if (!quiet)
					printf("Bit value: %d\n", sPIValue.i8uValue);
				else
					printf("%d\n", sPIValue.i8uValue);
			}
			if (cyclic)
				sleep(1);
		} while (cyclic);
	} else if (sPiVariable.i16uLength == 8) {
		do {
			rc = piControlRead(sPiVariable.i16uAddress, 1, (uint8_t *) & i8uValue);
			if (rc < 0) {
				fprintf(stderr, "Failed to read variable\n");
				if (!cyclic)
					return rc;
			} else {
				if (format == 'h') {
					if (!quiet)
						printf("1 Byte-Value of %s: %02x hex (=%d dez)\n", pszVariableName,
						       i8uValue, i8uValue);
					else
						printf("%x\n", i8uValue);
				} else if (format == 'b') {
					if (!quiet)
						printf("1 Byte-Value of %s: ", pszVariableName);

					printf("%c%c%c%c%c%c%c%c\n",
						i8uValue & 0x80 ? '1' : '0',
						i8uValue & 0x40 ? '1' : '0',
						i8uValue & 0x20 ? '1' : '0',
						i8uValue & 0x10 ? '1' : '0',
						i8uValue & 0x08 ? '1' : '0',
						i8uValue & 0x04 ? '1' : '0',
						i8uValue & 0x02 ? '1' : '0',
						i8uValue & 0x01 ? '1' : '0');
				} else {
					if (!quiet)
						printf("1 Byte-Value of %s: %d dez (=%02x hex)\n", pszVariableName,
						       i8uValue, i8uValue);
					else
						printf("%d\n", i8uValue);
				}
			}
			if (cyclic)
				sleep(1);
		} while (cyclic);
	} else if (sPiVariable.i16uLength == 16) {
		do {
			rc = piControlRead(sPiVariable.i16uAddress, 2, (uint8_t *) & i16uValue);
			if (rc < 0) {
				fprintf(stderr, "Failed to read variable\n");
				if (!cyclic)
					return rc;
			} else {
				if (format == 'h') {
					if (!quiet)
						printf("2 Byte-Value of %s: %04x hex (=%d dez)\n", pszVariableName,
						       i16uValue, i16uValue);
					else
						printf("%x\n", i16uValue);
				} else if (format == 'b') {
					if (!quiet)
						printf("2 Byte-Value of %s: ", pszVariableName);

					printf("%c%c%c%c%c%c%c%c ",
					       i16uValue & 0x8000 ? '1' : '0',
					       i16uValue & 0x4000 ? '1' : '0',
					       i16uValue & 0x2000 ? '1' : '0',
					       i16uValue & 0x1000 ? '1' : '0',
					       i16uValue & 0x0800 ? '1' : '0',
					       i16uValue & 0x0400 ? '1' : '0',
					       i16uValue & 0x0200 ? '1' : '0',
					       i16uValue & 0x0100 ? '1' : '0');

					printf("%c%c%c%c%c%c%c%c\n",
					       i16uValue & 0x0080 ? '1' : '0',
					       i16uValue & 0x0040 ? '1' : '0',
					       i16uValue & 0x0020 ? '1' : '0',
					       i16uValue & 0x0010 ? '1' : '0',
					       i16uValue & 0x0008 ? '1' : '0',
					       i16uValue & 0x0004 ? '1' : '0',
					       i16uValue & 0x0002 ? '1' : '0',
					       i16uValue & 0x0001 ? '1' : '0');
				} else {
					if (!quiet)
						printf("2 Byte-Value of %s: %d dez (=%04x hex)\n", pszVariableName,
						       i16uValue, i16uValue);
					else
						printf("%d\n", i16uValue);
				}
			}
			if (cyclic)
				sleep(1);
		} while (cyclic);
	} else if (sPiVariable.i16uLength == 32) {
		do {
			rc = piControlRead(sPiVariable.i16uAddress, 4, (uint8_t *) & i32uValue);
			if (rc < 0) {
				fprintf(stderr, "Failed to read variable\n");
				if (!cyclic)
					return rc;
			} else {
				if (format == 'h') {
					if (!quiet)
						printf("4 Byte-Value of %s: %08x hex (=%d dez)\n", pszVariableName,
						       i32uValue, i32uValue);
					else
						printf("%x\n", i32uValue);
				} else if (format == 'b') {
					if (!quiet)
						printf("4 Byte-Value of %s: ", pszVariableName);

					printf("%c%c%c%c%c%c%c%c ",
						i32uValue & 0x80000000 ? '1' : '0',
						i32uValue & 0x40000000 ? '1' : '0',
						i32uValue & 0x20000000 ? '1' : '0',
						i32uValue & 0x10000000 ? '1' : '0',
						i32uValue & 0x08000000 ? '1' : '0',
						i32uValue & 0x04000000 ? '1' : '0',
						i32uValue & 0x02000000 ? '1' : '0',
						i32uValue & 0x01000000 ? '1' : '0');

					printf("%c%c%c%c%c%c%c%c ",
						i32uValue & 0x00800000 ? '1' : '0',
						i32uValue & 0x00400000 ? '1' : '0',
						i32uValue & 0x00200000 ? '1' : '0',
						i32uValue & 0x00100000 ? '1' : '0',
						i32uValue & 0x00080000 ? '1' : '0',
						i32uValue & 0x00040000 ? '1' : '0',
						i32uValue & 0x00020000 ? '1' : '0',
						i32uValue & 0x00010000 ? '1' : '0');

					printf("%c%c%c%c%c%c%c%c ",
					       i32uValue & 0x00008000 ? '1' : '0',
					       i32uValue & 0x00004000 ? '1' : '0',
					       i32uValue & 0x00002000 ? '1' : '0',
					       i32uValue & 0x00001000 ? '1' : '0',
					       i32uValue & 0x00000800 ? '1' : '0',
					       i32uValue & 0x00000400 ? '1' : '0',
					       i32uValue & 0x00000200 ? '1' : '0',
						i32uValue & 0x00000100 ? '1' : '0');

					printf("%c%c%c%c%c%c%c%c\n",
					       i32uValue & 0x00000080 ? '1' : '0',
					       i32uValue & 0x00000040 ? '1' : '0',
					       i32uValue & 0x00000020 ? '1' : '0',
					       i32uValue & 0x00000010 ? '1' : '0',
					       i32uValue & 0x00000008 ? '1' : '0',
					       i32uValue & 0x00000004 ? '1' : '0',
					       i32uValue & 0x00000002 ? '1' : '0',
						i32uValue & 0x00000001 ? '1' : '0');
				} else {
					if (!quiet)
						printf("4 Byte-Value of %s: %d dez (=%08x hex)\n", pszVariableName,
						       i32uValue, i32uValue);
					else
						printf("%d\n", i32uValue);
				}
			}
			if (cyclic)
				sleep(1);
		} while (cyclic);
	} else {
		fprintf(stderr,
			"Got invalid length %u for read variable %s\n",
			sPiVariable.i16uLength, pszVariableName);
		return -1;
	}

	return 0;
}

/***********************************************************************************/
/*!
 * @brief Write data to process image
 *
 * Write <length> bytes to a specific offset of process image.
 *
 * @param[in]   Offset
 * @param[in]   Length
 * @param[in]   Value to write
 *
 ************************************************************************************/
int writeData(int offset, int length, unsigned long i32uValue)
{
	int rc;

	if (length != 1 && length != 2 && length != 4) {
		fprintf(stderr, "Length must be one of 1|2|4\n");
		return -EINVAL;
	}
	rc = piControlWrite(offset, length, (uint8_t *) & i32uValue);
	if (rc < 0) {
		fprintf(stderr, "write error %s\n", getWriteError(rc));
		return rc;
	} else {
		printf("Write value %lx hex (=%ld dez) to offset %d.\n", i32uValue, i32uValue, offset);
		return 0;
	}
}

/***********************************************************************************/
/*!
 * @brief Write variable value
 *
 * Write a value to a variable.
 *
 * @param[in]   Variable name
 * @param[in]   Value to write
 *
 ************************************************************************************/
int writeVariableValue(char *pszVariableName, uint32_t i32uValue)
{
	int rc;
	SPIVariable sPiVariable;
	SPIValue sPIValue;
	uint8_t i8uValue;
	uint16_t i16uValue;

	snprintf(sPiVariable.strVarName, sizeof(sPiVariable.strVarName), "%s", pszVariableName);
	rc = piControlGetVariableInfo(&sPiVariable);
	if (rc < 0) {
		fprintf(stderr, "Cannot find variable '%s'\n", pszVariableName);
		return rc;
	}

	if (sPiVariable.i16uLength == 1) {
		sPIValue.i16uAddress = sPiVariable.i16uAddress;
		sPIValue.i8uBit = sPiVariable.i8uBit;
		sPIValue.i8uValue = i32uValue;
		rc = piControlSetBitValue(&sPIValue);
		if (rc < 0) {
			fprintf(stderr, "Set bit error %s\n", getWriteError(rc));
			return rc;
		} else
			printf("Set bit %d on byte at offset %d. Value %d\n", sPIValue.i8uBit, sPIValue.i16uAddress,
			       sPIValue.i8uValue);
	} else if (sPiVariable.i16uLength == 8) {
		i8uValue = i32uValue;
		rc = piControlWrite(sPiVariable.i16uAddress, 1, (uint8_t *) & i8uValue);
		if (rc < 0) {
			fprintf(stderr, "Write error %s\n", getWriteError(rc));
			return rc;
		} else
			printf("Write value %d dez (=%02x hex) to offset %d.\n", i8uValue, i8uValue,
			       sPiVariable.i16uAddress);
	} else if (sPiVariable.i16uLength == 16) {
		i16uValue = i32uValue;
		rc = piControlWrite(sPiVariable.i16uAddress, 2, (uint8_t *) & i16uValue);
		if (rc < 0) {
			fprintf(stderr, "Write error %s\n", getWriteError(rc));
			return rc;
		} else
			printf("Write value %d dez (=%04x hex) to offset %d.\n", i16uValue, i16uValue,
			       sPiVariable.i16uAddress);
	} else if (sPiVariable.i16uLength == 32) {
		rc = piControlWrite(sPiVariable.i16uAddress, 4, (uint8_t *) & i32uValue);
		if (rc < 0) {
			fprintf(stderr, "Write error %s\n", getWriteError(rc));
			return rc;
		} else
			printf("Write value %d dez (=%08x hex) to offset %d.\n", i32uValue, i32uValue,
			       sPiVariable.i16uAddress);
	}

	return 0;
}

/***********************************************************************************/
/*!
 * @brief Set a bit
 *
 * Write a bit at a specific offset to a device.
 *
 * @param[in]   Offset
 * @param[in]   Bit number (0 - 7)
 * @param[in]   Value to write (0/1)
 *
 ************************************************************************************/
int setBit(int offset, int bit, int value)
{
	int rc;
	SPIValue sPIValue;

	// Check bit
	if (bit < 0 || bit > 7) {
		fprintf(stderr, "Wrong bit number. Try 0 - 7\n");
		return -EINVAL;
	}
	// Check value
	if (value != 0 && value != 1) {
		fprintf(stderr, "Wrong value. Try 0/1\n");
		return -EINVAL;
	}

	sPIValue.i16uAddress = offset;
	sPIValue.i8uBit = bit;
	sPIValue.i8uValue = value;
	// Set bit
	rc = piControlSetBitValue(&sPIValue);
	if (rc < 0) {
		fprintf(stderr, "Set bit error %s\n", getWriteError(rc));
		return rc;
	} else {
		printf("Set bit %d on byte at offset %d. Value %d\n", bit, offset, value);
	}

	return 0;
}

/***********************************************************************************/
/*!
 * @brief Get a bit
 *
 * Read a bit at a specific offset from a device.
 *
 * @param[in]   Offset
 * @param[in]   Bit number (0 - 7)
 *
 ************************************************************************************/
int getBit(int offset, int bit, bool quiet)
{
	int rc;
	SPIValue sPIValue;

	// Check bit
	if (bit < 0 || bit > 7) {
		fprintf(stderr, "Wrong bit number. Try 0 - 7\n");
		return -EINVAL;
	}

	sPIValue.i16uAddress = offset;
	sPIValue.i8uBit = bit;
	// Get bit
	rc = piControlGetBitValue(&sPIValue);
	if (rc < 0) {
		fprintf(stderr, "Failed to get bit value\n");
		return rc;
	} else if (quiet) {
		printf("%d\n", sPIValue.i8uValue);
	} else {
		printf("Get bit %d at offset %d. Value %d\n", bit, offset, sPIValue.i8uValue);
	}

	return 0;
}

/***********************************************************************************/
/*!
 * @brief Show infos for a specific variable name from process image
 *
 * @param[in]   Variable name
 *
 ************************************************************************************/
int showVariableInfo(char *pszVariableName)
{
	int rc;
	SPIVariable sPiVariable;

	snprintf(sPiVariable.strVarName, sizeof(sPiVariable.strVarName), "%s", pszVariableName);
	rc = piControlGetVariableInfo(&sPiVariable);
	if (rc < 0) {
		fprintf(stderr, "Failed to read variable info\n");
		return rc;
	} else {
		printf("variable name: %s\n", sPiVariable.strVarName);
		printf("       offset: %d\n", sPiVariable.i16uAddress);
		printf("       length: %d\n", sPiVariable.i16uLength);
		printf("          bit: %d\n", sPiVariable.i8uBit);
	}

	return 0;
}

static void printVersion(char *programname)
{
	printf("%s version %s\n", programname, PROGRAM_VERSION);
}

static void *spinner_thread_start(void *arg)
{
	char spinner_states[] = { '-', '\\', '|', '/' };
	size_t spinner_states_len = sizeof(spinner_states) / sizeof(spinner_states[0]);
	int spinner_pos;

	for (spinner_pos = 0; ; spinner_pos = (spinner_pos + 1) % spinner_states_len) {
		printf("%c\r", spinner_states[spinner_pos]);
		if (fflush(stdout) != 0) {
			fprintf(stderr, "spinner thread: error flushing stdout: %d (%s)\n", errno, strerror(errno));
			return NULL;
		}
		// transition spinner state 16 times per second
		if (usleep(SEC_AS_USEC / NUM_SPINS_PER_SECOND) != 0) {
			fprintf(stderr, "spinner thread: error sleeping: %d (%s)\n", errno, strerror(errno));
			return NULL;
		}
	}
}

static int handleFirmwareUpdate(int module_address, int force_update, int assume_yes, bool quiet)
{
	int rc;
	int ret = 0;
	ssize_t read = 0;
	char *buf;
	size_t buf_len = 0;
	char response = 'X';
	pthread_t spinner_thread_id;

	if (module_address < 0) {
		fprintf(stderr,
			"A module address must be given for an update and it must be placed before the -f parameter\n");
		return -EINVAL;
	}

	if (!assume_yes) {
		printf("Are you sure you want to update the firmware of a RevPi module? (y/N) ");
		read = getline(&buf, &buf_len, stdin);
		if (read < 0 && !feof(stdin)) {
			fprintf(stderr, "error occurred while reading from stdin: %d (%s)\n", errno, strerror(errno));
			free(buf);
			return -errno;
		}

		if (read < 1 || tolower(buf[0]) != 'y') {
			printf("Aborting firmware update\n");
			free(buf);
			return 0;
		}

		free(buf);
	}

	if (!quiet) {
		rc = pthread_create(&spinner_thread_id, NULL, &spinner_thread_start, NULL);
		if (rc != 0) {
			fprintf(stderr, "error creating spinner thread: %d (%s)\n", rc, strerror(rc));
			return -rc;
		}
	}

	rc = piControlUpdateFirmware(module_address, force_update);
	if (rc != 0) {
		fprintf(stderr, "failed to update firmware: %s\n", strerror(-rc));
		ret = rc;
		goto cleanupSpinnerThread;
	}

cleanupSpinnerThread:
	if (!quiet) {
		rc = pthread_cancel(spinner_thread_id);
		if (rc != 0) {
			fprintf(stderr, "error cancelling spinner thread: %d (%s)\n", rc, strerror(rc));
			return -rc;
		}
	}

	return ret;
}

/***********************************************************************************/
/*!
 * @brief Shows help for this program
 *
 * @param[in]   Program name
 *
 ************************************************************************************/
void printHelp(char *programname)
{
	printf("Usage: %s [OPTION]\n", programname);
	printf("- Shows infos from RevPiCore control process\n");
	printf("- Reads values of RevPiCore process image\n");
	printf("- Writes values to RevPiCore process image\n");
	printf("\n");
	printf("Options:\n");
	printf("                 -d: Get device list.\n");
	printf("\n");
	printf("      -v <var_name>: Shows infos for a variable.\n");
	printf("\n");
	printf("                 -V: Print this programs version.\n");
	printf("\n");
	printf("                 -1: execute the following read only once.\n");
	printf("\n");
	printf("                 -q: execute the following read quietly, print only the value.\n");
	printf("                     Can also be used to suppress the spinner output from a firmware update.\n");
	printf("\n");
	printf("-r <var_name>[,<f>]: Reads value of a variable.\n");
	printf("                     <f> defines the format: h for hex, d for decimal (default), b for binary\n");
	printf("                     E.g.: -r Input_001,h\n");
	printf("                     Read value from variable 'Input_001'.\n");
	printf("                     Shows values cyclically every second.\n");
	printf("                     Break with Ctrl-C.\n");
	printf("\n");
	printf("   -r <o>,<l>[,<f>]: Reads <l> bytes at offset <o>.\n");
	printf("                     <f> defines the format: h for hex, d for decimal (default), b for binary\n");
	printf("                     E.g.: -r 1188,16\n");
	printf("                     Read 16 bytes at offset 1188.\n");
	printf("                     Shows values cyclically every second.\n");
	printf("                     Break with Ctrl-C.\n");
	printf("\n");
	printf("  -w <var_name>,<v>: Writes value <v> to variable.\n");
	printf("                     E.g.: -w Output_001,23:\n");
	printf("                     Write value 23 dez (=17 hex) to variable 'Output_001'.\n");
	printf("\n");
	printf("     -w <o>,<l>,<v>: Writes <l> bytes with value <v> (as hex) to offset <o>.\n");
	printf("                     length should be one of 1|2|4.\n");
	printf("                     E.g.: -w 0,4,31224205:\n");
	printf("                     Write value 31224205 hex (=824328709 dez) to offset 0.\n");
	printf("\n");
	printf("         -g <o>,<b>: Gets bit number <b> (0-7) from byte at offset <o>.\n");
	printf("                     E.g.: -b 0,5:\n");
	printf("                     Get bit 5 from byte at offset 0.\n");
	printf("\n");
	printf("   -s <o>,<b>,<0|1>: Sets 0|1 to bit <b> (0-7) of byte at offset <o>.\n");
	printf("                     E.g.: -b 0,5,1:\n");
	printf("                     Set bit 5 to 1 of byte at offset 0.\n");
	printf("\n");
	printf("     -R <addr>,<bs>: Reset counters/encoders in a digital input module like DIO or DI.\n");
	printf("                     <addr> is the address of module as displayed with option -d.\n");
	printf("                     <bs> is a bitset. If the counter on input pin n must be reset,\n");
	printf("                     the n-th bit must be set to 1.\n");
	printf("                     E.g.: -R 32,0x0014:\n");
	printf("                     Reset the counters on input pin I_3 and I_5.\n");
	printf("\n");
	printf("          -C <addr>: Retrieve RO relay counters\n");
	printf("                     <addr> is the address of module as displayed with option -d.\n");
	printf("\n");
	printf("                 -S: Stop/Restart I/O update.\n");
	printf("\n");
	printf("                 -x: Reset piControl process.\n");
	printf("\n");
	printf("                 -l: Wait for reset of piControl process.\n");
	printf("\n");
	printf("                 -f: Update firmware. (see tutorials on website for more info)\n");
	printf("                     The option \"--module <addr>\" can be given before this one to specify the address of the module to update.\n");
	printf("                     If the \"--module <addr>\" is not given before it a module to update will be selected automatically.\n");
	printf("                     The option \"--force \" can be given before this one to ignore the firmware version check.\n");
	printf("\n");
	printf("    --module <addr>: <addr> specifies the address of the module to use for another option.\n");
	printf("                     This options can be used with the \"-f\" flag to specify a specific module to update.\n");
	printf("                     In order for the \"-f\" flag to recognize the address, this option has to be given directly before it.\n");
	printf("                     E.g.: --module 31 -f\n");
	printf("                     It can be combined with the \"--force\" option.\n");
	printf("\n");
	printf("            --force: Enforce the firmware update.\n");
	printf("                     This options can be used with the \"-f\" flag to force a firmware update.\n");
	printf("                     In order for the \"-f\" flag to recognize it, this option has to be given before it.\n");
	printf("                     E.g.: --force -f\n");
	printf("                     It can be combined with the \"--module\" option.\n");
	printf("\n");
	printf("       --assume-yes: Don't ask for confirmation when updating the firmware with -f\n");
	printf("                     In order to have an effect this needs to be given before the -f option.\n");
	printf("                     E.g.: --assume-yes -f\n");
	printf("\n");
	printf("  -c <addr>,<c>,<m>,<x>,<y>: Do the calibration. (see tutorials on website for more info)\n");
	printf("                     <addr> is the address of module as displayed with option -d.\n");
	printf("                     <c> is the bitmap of channels\n");
	printf("                     <m> is the mode\n");
	printf("                     <x> is the check point on x axix\n");
	printf("                     <y> is the check point on y axis\n");
}

/***********************************************************************************/
/*!
 * @brief main program
 *
 * @param[in]   Program name and arguments
 *
 ************************************************************************************/
int main(int argc, char *argv[])
{
	int c;
	int rc;
	int offset;
	int length;
	int address;
	unsigned int val;
	char format;
	int bit;
	bool cyclic = true;	// default is cyclic output
	bool quiet = false;	// default is verbose output
	unsigned long value;
	// Used for the `-f` option. If `--module <arg>` is not given *before* the
	// `-f` option the default value of `0` is used, which will automatically
	// choose which module to update.
	int module_address = -1;
	int assume_yes = 0;
	char szVariableName[256];
	char *pszTok, *progname;
	int force_update = 0;

	progname = strrchr(argv[0], '/');
	if (!progname) {
		progname = argv[0];
	} else {
		progname++;
	}

	if (!strcmp(progname, "piControlReset")) {
		rc = piControlReset();
		if (rc)
			fprintf(stderr, "Failed to reset driver\n");
		return 1;
	}

	if (argc == 1) {
		printHelp(progname);
		return 0;
	}

	struct option long_options[] = {
		[MODULE_LONG_ARG_INDEX] = { MODULE_LONG_ARG_NAME, required_argument, NULL, 0 },
		[FORCE_LONG_ARG_INDEX] = { FORCE_LONG_ARG_NAME, no_argument, &force_update, 1 },
		[ASSUME_YES_LONG_ARG_INDEX] = { ASSUME_YES_LONG_ARG_NAME, no_argument, &assume_yes, 1 },
		{0, 0, 0, 0}
	};
	int option_index = 0;

	// Scan argument
	while ((c = getopt_long(argc, argv, "dv:V1qr:w:s:R:C:c:g:xlfS",
				long_options, &option_index)) != -1) {
		switch (c) {
		case 0:
			/* long option specified */
			switch (option_index) {
				case MODULE_LONG_ARG_INDEX:
				{
					char *endptr = NULL;
					module_address = strtoul(optarg, &endptr, 10);
					if (endptr == optarg) {
						fprintf(stderr, "Invalid argument '%s' to option '%s'\n", optarg,
							long_options[option_index].name);
						return 1;
					}
					if (module_address < 0) {
						fprintf(stderr,
							"The address of a module must be a positive number\n");
						return 1;
					}
					break;
				}

				case FORCE_LONG_ARG_INDEX:
					break;

				case ASSUME_YES_LONG_ARG_INDEX:
					break;

				default:
					fprintf(stderr, "Invalid long option index %d\n", option_index);
					return 1;
					break;
			}
			break;

		case 'd':
			rc = showDeviceList();
			if (rc < 0) {
				fprintf(stderr, "Cannot retrieve device list\n");
				return 1;
			}
			break;

		case 'v':
			if (strlen(optarg) > 0) {
				showVariableInfo(optarg);
			} else {
				fprintf(stderr, "No variable name\n");
				return 1;
			}
			break;

		case 'V':
			printVersion(progname);
			break;

		case '1':	// execute the command only once, not cyclic
			cyclic = false;
			break;

		case 'q':	// execute the command quietly
			quiet = true;
			break;

		case 'r':
			format = 'd';
			rc = sscanf(optarg, "%d,%d,%c", &offset, &length, &format);
			if (rc == 3) {
				rc = readData(offset, length, cyclic, format, quiet);
				if (rc < 0) {
					fprintf(stderr, "Failed to read data\n");
					return 1;
				}
				return 0;
			}
			rc = sscanf(optarg, "%d,%d", &offset, &length);
			if (rc == 2) {
				rc = readData(offset, length, cyclic, format, quiet);
				if (rc < 0) {
					fprintf(stderr, "Failed to read data\n");
					return 1;
				}
				return 0;
			}
			rc = sscanf(optarg, "%s", szVariableName);
			if (rc == 1) {
				pszTok = strtok(szVariableName, ",");
				if (pszTok != NULL) {
					pszTok = strtok(NULL, ",");
					if (pszTok != NULL) {
						format = *pszTok;
					}
				}
				rc = readVariableValue(szVariableName, cyclic, format, quiet);
				if (rc < 0) {
					fprintf(stderr, "Failed to read variable value\n");
					return 1;
				}
				return 0;
			}
			fprintf(stderr, "Wrong arguments for read function\n");
			fprintf(stderr, "1.) Try '-r variablename'\n");
			fprintf(stderr, "2.) Try '-r offset,length' (without spaces)\n");
			return 1;
			break;

		case 'w':
			rc = sscanf(optarg, "%d,%d,%lu", &offset, &length, &value);
			if (rc == 3) {
				rc = writeData(offset, length, value);
				if (rc < 0) {
					fprintf(stderr, "Failed to write data\n");
					return 1;
				}
				return 0;
			}
			pszTok = strtok(optarg, ",");
			if (pszTok != NULL) {
				snprintf(szVariableName, sizeof(((SPIVariable *)0)->strVarName), "%s", pszTok);
				pszTok = strtok(NULL, ",");
				if (pszTok != NULL) {
					value = strtol(pszTok, NULL, 10);
					rc = writeVariableValue(szVariableName, value);
					if (rc < 0) {
						fprintf(stderr, "Failed to write value to variable\n");
						return 1;
					}
					return 0;
				}
			}
			fprintf(stderr, "Wrong arguments for write function\n");
			fprintf(stderr, "1.) Try '-w variablename,value' (without spaces)\n");
			fprintf(stderr, "2.) Try '-w offset,length,value' (without spaces)\n");
			return 1;
			break;

		case 's':
			rc = sscanf(optarg, "%d,%d,%lu", &offset, &bit, &value);
			if (rc != 3) {
				fprintf(stderr, "Wrong arguments for set bit function\n");
				fprintf(stderr, "Try '-s offset,bit,value' (without spaces)\n");
				return 1;
			}
			rc = setBit(offset, bit, value);
			if (rc < 0) {
				fprintf(stderr, "Failed to set bit\n");
				return 1;
			}
			return 0;
			break;

		case 'R':	// reset counter
			rc = sscanf(optarg, "%d,0x%x", &address, &val);
			if (rc != 2) {
				rc = sscanf(optarg, "%d,%u", &address, &val);
				if (rc != 2) {
					fprintf(stderr, "Wrong arguments for counter reset function\n");
					fprintf(stderr, "Try '-R address,value' (without spaces)\n");
					return 1;
				}
			}
			rc = piControlResetCounter(address, val);
			if (rc < 0) {
				fprintf(stderr, "Failed to reset counter\n");
				return 1;
			}
			return 0;
			break;
		case 'C':	// get RO counters
			rc = sscanf(optarg, "%d", &address);
			if (rc != 1) {
				fprintf(stderr, "Wrong arguments for retrieving RO counters\n");
				fprintf(stderr, "Try '-C address'\n");
				return 1;
			}
			rc = piControlGetROCounters(address);
			if (rc < 0) {
				fprintf(stderr, "Failed to get RO counters\n");
				return 1;
			}
			return 0;
			break;

		case 'c':
		{
			unsigned int addr, channl, mode, x_val, y_val;
			rc = sscanf(optarg, "%u,0x%x,0x%x,0x%x,0x%x",
					&addr, &channl, &mode, &x_val, &y_val);
			if (rc != 5) {
				rc = sscanf(optarg, "%u,%u,%u,%u,%u",
					&addr, &channl, &mode, &x_val, &y_val);
				if (rc != 5) {
					fprintf(stderr, "Wrong arguments to calibrate\n");
					fprintf(stderr, "Try '-c address,channels,modes,"
						"x,y'(without spaces)\n");
					return 1;
				}
			}
			rc = piControlCalibrate(addr, channl, mode, x_val, y_val);
			if (rc < 0) {
				fprintf(stderr, "Failed to calibrate\n");
				return 1;
			}
			printf("calibrated dev:%d,chnnls:%d,mode:%d,x:%d,y:%d\n",
					addr, channl, mode, x_val, y_val);
			return 0;
		}
			break;
		case 'g':
			rc = sscanf(optarg, "%d,%d", &offset, &bit);
			if (rc != 2) {
				fprintf(stderr, "Wrong arguments for get bit function\n");
				fprintf(stderr, "Try '-g offset,bit' (without spaces)\n");
				return 1;
			}
			rc = getBit(offset, bit, quiet);
			if (rc < 0) {
				fprintf(stderr, "Failed to get bit value\n");
				return 1;
			}
			return 0;
			break;

		case 'x':
			rc = piControlReset();
			if (rc) {
				fprintf(stderr, "Failed to reset driver\n");
				return 1;
			}
			break;

		case 'l':
			rc = piControlWaitForEvent();
			if (rc < 0) {
				fprintf(stderr, "Failed to wait for event\n");
				return rc;
			} else if (rc == 1) {
				printf("WaitForEvent returned: Reset\n");
				return rc;
			} else {
				printf("WaitForEvent returned: %d\n", rc);
				return rc;
			}
			break;

		case 'f':
			rc = handleFirmwareUpdate(module_address, force_update, assume_yes, quiet);
			if (rc) {
				fprintf(stderr, "error when updating firmware: %s\n", strerror(-rc));
				return rc;
			}
			break;

		case 'S':
			rc = piControlStopIO(2);	// toggle mode of I/Os
			if (rc < 0) {
				fprintf(stderr, "error in setting I/O update mode: %d\n", rc);
				return 1;
			} else if (rc == 0) {
				printf("I/Os and process image are updated\n");
			} else {
				printf("update of I/Os and process image is stopped\n");
			}
			break;

		case 'h':
		default:
			printHelp(progname);
			break;
		}
	}

	return 0;
}
