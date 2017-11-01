/**********************************************************************************************************************
 File: BBGWHwUtilities.h
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

#ifndef BBGWHWUTILITIES_H
#define BBGWHWUTILITIES_H

#include <stdbool.h>

#include "project.h"
#include "ErrorCode.h"

// Size for all system messages
#define MAX_SYSTEM_MSG_SIZE 1024
#define MAX_SYSTEM_FILEPATH	256

/**
 ======================================================================================================================
 Discrete I/O functions
 ======================================================================================================================
 */
#if BBGW_USE_GPIO
	// Possible pass the argument for GPIO pins
	typedef enum
	{
		GPIO_INPUT,
		GPIO_OUTPUT
	} GPIO_Direction;

	// Possible pass the argument for GPIO pins
	typedef enum
	{
		GPIO_LOW = 0,
		GPIO_HIGH = 1,
	} GPIO_State;

	/********************************************************************************************************************
	 * This function configures a GPIO pin.
	 *
	 * Inputs:	pinNumber - GPIO pin number to configure.
	 *						Example input like 60, Ref-GPIO_60(pin 12 on the P9 header).
	 * 					direction	-	Configure the pin as an input or output.
	 *
	 * Return:	None. We could return an error code if we have a problem configuring the pin, but this type of problem
	 * 					indicates a design issue, like choosing an incorrect pin. Any such issue will be obvious during system
	 * 					testing. Rather than complicate the code with such error checks, we just print a console message.
	 *
	 * Outputs:	Updated OS files that configure the GPIO.
	 *
	 * Notes:		This code relies on the PWM overlay having been loaded. To do this, this line needs to be in the
	 * 					/boot/uEnv.txt file.
	 *						cape_enable=bone_capemgr.enable_partno=BB-PWM1
	 *
	 *					Pull-up/down options don't seem to be controllable using the sys file system. They're controlled by the
	 * 					device tree overlay.
	 ********************************************************************************************************************/
	void DiscreteIO_init(unsigned int pinNumber, GPIO_Direction direction);

	/********************************************************************************************************************
	 * Sets the specified pin high or low.
	 *
	 * Inputs: 	pinNumber - GPIO pin number in BBGW board.
	 *  				pinStatus - value to set the pin to.
	 *
	 * Return: 	None. We could return an error code if we have a problem configuring the pin, but this type of problem
	 * 					indicates a design issue, like choosing an incorrect pin. Any such issue will be obvious during system
	 * 					testing. Rather than complicate the code with such error checks, we just print a console message.
	 *
	 * Outputs: Updated OS file that sets the pin value.
	 *
	 * Notes:		None
	 *
	 ********************************************************************************************************************/
	void DiscreteIO_write(unsigned int pinNumber, GPIO_State pinState);

	/********************************************************************************************************************
	 * This function to read GPIO pin status and return 1(HIGH) or 0(LOW) to function call.
	 *
	 * Inputs: 	pinNumber - GPIO pin number in BBGW board.
	 *
	 * Return: 	Return pin status, high or low.
	 *
	 * Outputs: None
	 *
	 * Notes:		None
	 *
	 ********************************************************************************************************************/
	GPIO_State DiscreteIO_read(unsigned int pinNumber);
#endif	// BBGW_USE_GPIO

/**
 ======================================================================================================================
 A-to-D functions
 ======================================================================================================================
 */
#if BBGW_USE_A_TO_D
	// The BBGW has a 12-bit A/D, giving us a range of 0 - 4095
	#define A2D_BIT_RANGE		4096

	/********************************************************************************************************************
	 * Configure the BBGW to read analog inputs. It does this by executing the needed shell script.
	 *
	 * Inputs: 	None
	 *
	 * Return:	None
	 *
	 * Outputs: Writes to the sys file system results in new files that are used to read the A/D input.
	 *
	 * Notes:		This code relies on the PWM overlay having been loaded. To do this, this line needs to be in the
	 * 					/boot/uEnv.txt file.
	 *						cape_enable=bone_capemgr.enable_partno=BB-ADC
	 *
	 ********************************************************************************************************************/
	void AtoD_init();


	/********************************************************************************************************************
	 * Reads the specified A/D input, and returns the current value in floating point format.
	 *
	 * Inputs:	A2DInput	- the input A/D pin to read, from 0 to 6 on BBBW.
	 *
	 * Outputs: None
	 *
	 * Returns:	The read value, or -1.0 if we coulnd't read the value.
	 *
	 * Notes:		The A/D on BBGW read 0 - 1.8V inputs. Input voltages need to be scaled down to within that range.
	 ********************************************************************************************************************/
	float AtoD_read(unsigned int A2DInput);

#endif	// BBGW_USE_A_TO_D

#if BBGW_USE_PWM

	// Supported PWM pins
	typedef enum
	{
		EHRPWM1A = 0,
		EHRPWM1B,
		PWM_TOTAL_Pins
	} PWMPin_T;

	/**********************************************************************************************************************
	 * Configures the specified PWM pin. This does not enable the PWM, or specify the duty cycle when it is enabled.
	 * That's done by calling enablePWM();
	 *
	 * Inputs:	PWM_Pin									- The PWM pin to configure
	 * 					period_nSecs						- Total period of the PWM pulse, in nanoseconds.
	 * 					minDutyCycle_Percentage	- minimum duty cycle allowed, from 0 to 100%.
	 * 					maxDutyCycle_Percentage	- maximum duty cycle allowed, from 0 to 100%.
	 * 					stepSize_Percentge			- When "stepping" the motor, the step size, from 0 to 100%.
	 *
	 * Outputs: Updated configuration settings for the specified PWM pin.
	 *
	 * Returns:	None. If we have a configuration error, this should be obvious from system testing, where the PWM doesn't
	 * 					work as expected. Rather than add code to check unlikely errors, we ignore them, but we do print out an
	 * 					error, which can be seen if the executable is run from the command line.
	 *
	 * Notes:		Currently, only EHRPWM1A and EHRPWM1B pins are supported on the BBGW.
	 **********************************************************************************************************************/
	void PWM_init(
		PWMPin_T PWM_Pin,
		float period_mSecs,
		float minDutyCycle_mSecs,
		float maxDutyCycle_mSecs,
		float stepSize_mSecs);

	/**********************************************************************************************************************
	 * Enables the specified PWM.
	 *
	 * Inputs:	PWM_Pin	- The PWM pin to disable.
	 *
	 * Outputs: System write to the file system to enable the PWM pin.
	 *
	 * Returns:	None
	 *
	 * Notes:		None
	 **********************************************************************************************************************/
	void PWM_enable(PWMPin_T pin);

	/**********************************************************************************************************************
	 * Disables the specified PWM.
	 *
	 * Inputs:	PWM_Pin	- The PWM pin to disable.
	 *
	 * Outputs: System write to the file system to disable the PWM pin.
	 *
	 * Returns:	None
	 *
	 * Notes:		None
	 **********************************************************************************************************************/
	void PWM_disable(PWMPin_T pin);

	/**********************************************************************************************************************
	 * Enables the specified PWM pin, setting the duty cycle.
	 *
	 * Inputs:	PWM_Pin						- The PWM pin to enable.
	 * 					dutyCycle_Percentage	- The desired duty cycle %.
	 *
	 * Outputs: Updated configuration settings for the specified PWM pin.
	 * 					System writes to the file system to enable the PWM at the specified duty cycle.
	 *
	 * Returns:	true if we moved the servo position. false if we did not.
	 * 					An error is printed if the specified duty cycle is out of range.
	 *
	 * Notes:		To reduce redundant sw checks, the rnage of the duty cycle is not checked.
	 * 					Make sure you call this function with a valid duty cycle value.
	 **********************************************************************************************************************/
	bool PWM_setPosition(PWMPin_T PWM_Pin, float dutyCycle_mSecs);

	/**********************************************************************************************************************
	 * Steps the PWM duty cycle up or down
	 *
	 * Inputs:	PWM_Pin	- The PWM pin to disable.
	 * 					increaseDutyCycle	- If TRUE, increases the duty cycle. If FALSE, decreases the duty cycle.
	 *
	 * Outputs: System write to the file system to update the the PWM duty cycle.
	 *
	 * Returns:	true if we stepped the specified servo.
	 * 					false if we didn't step the servo because we're at a range limit.
	 *
	 * Notes:		This function prevents the position from exceeding the previously configures max or min duty cycle.
	 **********************************************************************************************************************/
	bool PWM_step(PWMPin_T pin, bool increaseDutyCycle);

	/**********************************************************************************************************************
	 * Returns the current PWM duty cycle.
	 *
	 * Inputs:	PWM_Pin	- The PWM pin to disable.
	 *
	 * Outputs:	None
	 *
	 * Returns:	Current PWM duty cycle in mSecs.
	 *
	 * Notes:		The return is a float. If you try and compare it to another value, you should leave some room for floating
	 * 					point math error.
	 **********************************************************************************************************************/
	float PWM_getDutyCycle_mSecs(PWMPin_T pin);

#endif	// BBGW_USE_PWM

#endif	// BBGWHWUTILITIES_H
