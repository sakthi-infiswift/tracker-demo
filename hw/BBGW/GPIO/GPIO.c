/**********************************************************************************************************************
 File: BBGWHwUtilities.c
 Copyright © 2017  Infiswift Inc. All rights reserved.

 This file and the related software (if any) may contain confidential and proprietary information of Infiswift Inc. and
 are made available solely for confidential use by the intended recipient. Unless expressly authorized by Infiswift Inc.
 in a separate written agreement, no part of this document or any related software may be used, reproduced, modified,
 isclosed or distributed, in any form or by any means.

 Restricted Rights Legend:  Any use, duplication or disclosure by the government is subject to the restrictions set forth
 in FAR Section 12.212 and DFAR 227.7202.  Infiswift Inc.

 Infiswift® is a registered trademark of Infiswift Inc.  All other trademarks used herein are the trademarks or registered
 trademarks of their respective owners.

 Infiswift Inc. may have patents, copyrights, trademarks and other intellectual property rights (or applications therefor)
 covering this file, its subject matter and any related software.  Unless expressly authorized by Infiswift Inc. in a
 separate written agreement, possession of this file does not give you any license under any of these rights.

 Infiswift Inc. makes no warranties, express or implied, and assumes no liability for any damage or loss with respect to or
 resulting from the use of this document or any related software.  All information is subject to change without notice.

 ***********************************************************************************************************************/
/**********************************************************************************************************************
 Purpose: This file provides functions that make it easy to use the BBGW's GPIO, A/D and PWM capabilities.
 ***********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "project.h"
#include "utilities/utilities.h"
#include "GPIO.h"

#if BBGW_USE_GPIO
	void DiscreteIO_init(unsigned int pinNumber, GPIO_Direction direction)
	{
		FILE *fileHandle = NULL;
		char systemMsg[MAX_SYSTEM_MSG_SIZE];

		// Enable GPIO for this specified pin
		fileHandle = fopen("/sys/class/gpio/export", "ab");
		if (fileHandle != NULL)
		{
			// Write the pin number in export
			snprintf(systemMsg, MAX_SYSTEM_MSG_SIZE, "%d", pinNumber);
			fwrite(systemMsg, sizeof(char), strlen(systemMsg), fileHandle);
			fclose(fileHandle);

			// Configure the pin direction
			snprintf(systemMsg, MAX_SYSTEM_MSG_SIZE, "/sys/class/gpio/gpio%d/direction", pinNumber);
			fileHandle = fopen(systemMsg, "rb+");
			if (fileHandle != NULL)
			{
				if(direction == GPIO_INPUT)
				{
					snprintf(systemMsg, MAX_SYSTEM_MSG_SIZE, "in");
				}
				else
				{
					snprintf(systemMsg, MAX_SYSTEM_MSG_SIZE, "out");
				}
				fwrite("systemMsg", sizeof(char), MAX_SYSTEM_MSG_SIZE, fileHandle);
				fclose(fileHandle);
			}
			else
			{
				printf("Could not open direction file for GPIO %d\n", pinNumber);
			}
		}
		else
		{
			/* This is caused by a design error, specifying the wrong pin. Instead of adding code to handle
			 * it, we just print an error message to the console.
			 */
			printf("Error exporting pin value in GPIO_init. Pin %d\n", pinNumber);
		}
	}

	void DiscreteIO_write(unsigned int pinNumber, GPIO_State pinState)
	{
		FILE *fileHandle = NULL;
		char systemMsg[MAX_SYSTEM_MSG_SIZE];

		snprintf(systemMsg, MAX_SYSTEM_MSG_SIZE, "/sys/class/gpio/gpio%d/value", pinNumber);
		fileHandle = fopen(systemMsg, "rb+");
		if (fileHandle != NULL)
		{
			if (pinState == GPIO_LOW)
			{
				fwrite("0", sizeof(char), 1, fileHandle);
			}
			else
			{
				fwrite("1", sizeof(char), 1, fileHandle);
			}
			fclose(fileHandle);
		}
		else
		{
			printf("Error opening file to write pin value. Pin %d\n", pinNumber);
		}
	}

	GPIO_State DiscreteIO_read(unsigned int pinNumber)
	{
		FILE *fileHandle = NULL;
		char systemMsg[MAX_SYSTEM_MSG_SIZE];
		char ReadValue[4];
		GPIO_State pinStatus = GPIO_LOW;

		snprintf(systemMsg, MAX_SYSTEM_MSG_SIZE, "/sys/class/gpio/gpio%d/value", pinNumber);
		fileHandle = fopen(systemMsg, "rb+");
		if (fileHandle != NULL)
		{
			if (fread(&ReadValue, sizeof(char), 1, fileHandle) == 1)
			{
				if (ReadValue[0] == '0')
				{
					pinStatus = GPIO_LOW;
				}
				else
				{
					pinStatus = GPIO_HIGH;
				}
			}
			else
			{
				printf("Error reading pin value. Pin %d\n", pinNumber);
			}

			fclose(fileHandle);
		}
		else
		{
			printf("Error opening file to read pin value. Pin %d\n", pinNumber);
		}

		return pinStatus;
	}
#endif	// BBGW_USE_GPIO

/**
 ======================================================================================================================
 A-to-D Functions
 ======================================================================================================================
 */
#if BBGW_USE_A_TO_D
	// A/D definitions
	#define A2D_BIT_RESOLUTION	12
	#define MAX_A2D_VOLTAGE	(1.8)
	#define MAX_A2D_VALUE		4095	/* 2^A2D_BIT_RESOLUTION */
	#define MAX_A2D_STRING_LENGTH	10

	void AtoD_init(void)
	{
		// Nothing to do. The A/D device overlay is loaded in /boot/uEnv.txt.
	}

	float AtoD_read(unsigned int A2DInput)

	{
		float AnalogValue = -1.0;
		char AnalogStr[MAX_A2D_STRING_LENGTH];				// This is a 12-bit A/D, so max size is???

		#if defined(DESKTOP)
			// Desktop build. Return a random value between 0 and the max A/D value.
			snprintf(AnalogStr, MAX_A2D_STRING_LENGTH, "%i", rand() % 4095);
		#else
			int FileDescriptor;
			char AnalogPinFilePath[MAX_SYSTEM_FILEPATH];

			// Read the specified A/D input
			snprintf(AnalogPinFilePath, MAX_SYSTEM_FILEPATH, "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw", A2DInput);
			FileDescriptor = open(AnalogPinFilePath, O_RDONLY);
			if (FileDescriptor >= 0)
			{	// Opened the file. Read the input.
				read(FileDescriptor, &AnalogStr, MAX_A2D_STRING_LENGTH);
			}
			else
			{
				printf("Error opening %s for reading\n", AnalogPinFilePath);
			}
			close(FileDescriptor);
		#endif

		// Convert read in string value to a floating point in the range of 0 to MAX_A2D_VOLTAGE
		AnalogValue = ((float)atoi(AnalogStr))/((float)(MAX_A2D_VALUE)) * MAX_A2D_VOLTAGE;
		return AnalogValue;
	}
#endif

#if BBGW_USE_PWM
	// Define paths to PWM files needed to configure PWM
	#define ENABLE_PWM_PATH	"/sys/class/pwm/pwmchip0/export"
	#define EHRPWM1A_PATH	"/sys/class/pwm/pwmchip0/pwm0"
	#define EHRPWM1B_PATH	"/sys/class/pwm/pwmchip0/pwm1"

	// Convert from duty cycle % to number of nanoseconds
	#define GET_PWM_PATH(a)	((a == EHRPWM1A) ? EHRPWM1A_PATH : EHRPWM1B_PATH)

	// Convert from ms to nanoseconds. Processor registers use nanosecond values.
	#define ConvertToNanoseconds(mSecs_float)	(unsigned long)(mSecs_float * 1000000.0)

	// Array of structs that contain the PWM configurations for each PWM pin
	typedef struct
	{
		float period_mSecs;
		float currentDutyCycle_mSecs;
		float minDutyCycle_mSecs;
		float maxDutyCycle_mSecs;
		float stepSize_mSecs;
	} PWMConfig_T;
	static PWMConfig_T PWMConfig[PWM_TOTAL_Pins];


	void PWM_init(
		PWMPin_T pin,
		float period_mSecs,
		float minDutyCycle_mSecs,
		float maxDutyCycle_mSecs,
		float stepSize_mSecs)
	{
		char systemMessage[MAX_SYSTEM_MSG_SIZE];

		if (maxDutyCycle_mSecs > period_mSecs)
		{
			printf("Max duty cycle, %f ms, cannot be > period of %f ms.\n", maxDutyCycle_mSecs, period_mSecs);
		}

		else if (minDutyCycle_mSecs > maxDutyCycle_mSecs)
		{
			printf("Min duty cycle, %f ms, cannot be > max duty cycle, %f ms.\n", minDutyCycle_mSecs, maxDutyCycle_mSecs);
		}

		else if (stepSize_mSecs > (maxDutyCycle_mSecs - minDutyCycle_mSecs))
		{
			printf("Step size, %f ms, cannot be greater than the duty cycle full range %f ms.\n",
					stepSize_mSecs, (maxDutyCycle_mSecs - minDutyCycle_mSecs));
		}

		else
		{	// Passed our error checks
			PWMConfig[pin].period_mSecs = period_mSecs;
			PWMConfig[pin].minDutyCycle_mSecs = minDutyCycle_mSecs;
			PWMConfig[pin].maxDutyCycle_mSecs = maxDutyCycle_mSecs;
			PWMConfig[pin].stepSize_mSecs = stepSize_mSecs;

		}

		// Enable the specified PWM
		if (pin == EHRPWM1A)
		{
			snprintf(systemMessage, MAX_SYSTEM_MSG_SIZE, "echo 0 > %s", ENABLE_PWM_PATH);
		}
		else
		{
			snprintf(systemMessage, MAX_SYSTEM_MSG_SIZE, "echo 1 > %s", ENABLE_PWM_PATH);
		}
		int result = system_desktopSafe(systemMessage);
		if ((result != 0) && (result != 1))
		{
			/* The system command returns -1 if it fails to execute, which shouldn't happen.
			 * What can happen is:
			 * result		Reason
			 * ------		-------
			 *  0				Everything is good. Export file exists, and our write enabled the PWM channel specified.
			 *  1				Value has already been written to export file. This is okay, keep running.
			 *  2				Export file doesn't exist, likely because the PWM overlay hasn't been loaded.
			 *
			 *  We'll keep running if we see 0 or 1. Otherwise, we'll abort and rely on systemd to restart it,
			 *  hopefully after the PWM overlay has been loaded.
			 */
			printf("Error %d writing to %s\n", result, ENABLE_PWM_PATH);
			exit(1);
		}

		// Setup the PWM period
		// We don't enable the PWM yet. That requires the current duty cycle to be specified.
		snprintf(systemMessage, MAX_SYSTEM_MSG_SIZE, "echo %lu > %s/period",
				ConvertToNanoseconds(PWMConfig[pin].period_mSecs), GET_PWM_PATH(pin));
		system_desktopSafe(systemMessage);
	}


	void PWM_enable(PWMPin_T pin)
	{
		char systemMessage[MAX_SYSTEM_MSG_SIZE];
		snprintf(systemMessage, MAX_SYSTEM_MSG_SIZE, "echo 1 > %s/enable", GET_PWM_PATH(pin));
		system_desktopSafe(systemMessage);
	}

	void PWM_disable(PWMPin_T pin)
	{
		char systemMessage[MAX_SYSTEM_MSG_SIZE];
		snprintf(systemMessage, MAX_SYSTEM_MSG_SIZE, "echo 0 > %s/enable", GET_PWM_PATH(pin));
		system_desktopSafe(systemMessage);
	}

	bool PWM_setPosition(PWMPin_T pin, float dutyCycle_mSecs)
	{
		// Range limit the duty cycle
		if (dutyCycle_mSecs > PWMConfig[pin].maxDutyCycle_mSecs)
		{	// Limit the duty cycle to be in range
			dutyCycle_mSecs = PWMConfig[pin].maxDutyCycle_mSecs;
		}
		// else, not out of range

		if (dutyCycle_mSecs < PWMConfig[pin].minDutyCycle_mSecs)
		{	// Limit the duty cycle to be in range
			dutyCycle_mSecs = PWMConfig[pin].minDutyCycle_mSecs;
		}
		// else, not out of range

		if (dutyCycle_mSecs != PWMConfig[pin].currentDutyCycle_mSecs)
		{	// We need to drive the servo
			char systemMessage[MAX_SYSTEM_MSG_SIZE];

			PWMConfig[pin].currentDutyCycle_mSecs = dutyCycle_mSecs;
			snprintf(systemMessage, MAX_SYSTEM_MSG_SIZE, "echo %lu > %s/duty_cycle",
					ConvertToNanoseconds(PWMConfig[pin].currentDutyCycle_mSecs), GET_PWM_PATH(pin));
			system_desktopSafe(systemMessage);

			// Enable the PWM. If it's already enabled, that's fine.
			PWM_enable(pin);
			return true;
		}
		else
		{	// No need to drive the servo
			// Disable this
			PWM_disable(pin);
			return false;
		}
	}

	bool PWM_step(PWMPin_T pin, bool increaseDutyCycle)
	{
		float currentDutyCycle = PWMConfig[pin].currentDutyCycle_mSecs;

		if (increaseDutyCycle)
		{
			currentDutyCycle += PWMConfig[pin].stepSize_mSecs;
		}
		else
		{	// decrease the duty cycle
			currentDutyCycle -= PWMConfig[pin].stepSize_mSecs;
		}

		return PWM_setPosition(pin, currentDutyCycle);
	}

	float PWM_getDutyCycle_mSecs(PWMPin_T pin)
	{
		return PWMConfig[pin].currentDutyCycle_mSecs;
	}
#endif	// BBGW_USE_PWM
