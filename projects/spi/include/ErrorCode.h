/*************************************************************************************************
 File: ErrorCode.c
 Copyright © 2017  iswift Inc. All rights reserved.

 This file and the related software (if any) may contain confidential and proprietary information of Infiswift Inc.
 and are made available solely for confidential use by the intended recipient. Unless expressly authorized by
 Infiswift Inc. in a separate written agreement, no part of this document or any related software may be used,
 reproduced, modified, disclosed or distributed, in any form or by any means.

 Restricted Rights Legend:  Any use, duplication or disclosure by the government is subject to the restrictions set
 forth in FAR Section 12.212 and DFAR 227.7202.  Infiswift Inc.

 Infiswift® is a registered trademark of Infiswift Inc.  All other trademarks used herein are the trademarks or
 registered trademarks of their respective owners.
 Infiswift Inc. may have patents, copyrights, trademarks and other intellectual property rights (or applications
 therefor) covering this file, its subject matter and any related software.  Unless expressly authorized by
 Infiswift Inc. in a separate written agreement, possession of this file does not give you any license under any of
 these rights.
 Infiswift Inc. makes no warranties, express or implied, and assumes no liability for any damage or loss with respect
 to or resulting from the use of this document or any related software.  All information is subject to change without
 notice.
 *********************************************************************************************/

/* Purpose: To maintain error code in one place for all the modules
 */

/* Note : Reserve 50 codes for each group.
 * 		  Whenever starting a new group of error code, ensure 50 codes are reserved for previous one.
 */
#ifndef ERROR_CODE_H
#define ERROR_CODE_H
typedef enum {

	// General code applicable for all groups, Reserved 50
	ERROR_SUCCESS = 0,
	ERROR_GENERAL_FAILURE,
	ERROR_COULD_NOT_OPEN_FILE,

	// GPIO errors
	ERROR_GPIO_FAILED_TO_EXPORT_PIN = 10,
	ERROR_GPIO_FAILED_TO_SET_DIRECTION,
	ERROR_GPIO_FAILED_TO_SET_PULLUP_DOWN,

	// String errors
	ERROR_STR_INDEX_LENGTH_ERROR = 40,

	// Shared memory related errors
	ERROR_SHARED_MEMORY_ACCESS = 51,
	ERROR_SHARED_MEMORY_ATTACH = 52,
	ERROR_SHARED_MEMORY_DETACH = 53,
	ERROR_SHARED_MEMORY_SEMAPHORE_OPEN = 60,

	//CAN ERRORS 70+
	ERROR_COULD_NOT_OPEN_CAN_SOCKET = 70,
	ERROR_CAN_IOCTL = 71,
	ERROR_COULD_NOT_CONFIGURE_GPIO_AS_OUTPUT = 72,
	ERROR_CAN_SOCKET_NOT_OPEN = 73,
	WARNING_EXTRA_IGNORED = 74,
	NO_CAN_DATA = 75,
	CAN_DATA_GOT_SUCCESS = 76,

	// PWM errors
	ERROR_PWM_MAX_DUTY_CYCLE_OUT_OF_RANGE = 80,
	ERROR_PWM_MIN_DUTY_CYCLE_OUT_OF_RANGE,
	ERROR_PWM_STEP_SIZE_OUT_OF_RANGE,
	ERROR_PWM_DUTY_CYCLE_OUT_OF_RANGE,

	// MQTT Errors
	ERROR_MQTT_SETTING_CREDENTIALS = 90,
	ERROR_MQTT_CONNECTING,
	ERROR_MQTT_PUBLISHING,
	ERROR_MQTT_SUBSCRIBE,
	ERROR_MQTT_LOOPING,

	// BBGW analog, GPIO, etc., related errors
	ERROR_ANALOG_READ = 100

} ErrorCode_Type;

#endif  // ERROR_CODE_H
