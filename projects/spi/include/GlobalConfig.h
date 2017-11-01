/**********************************************************************************************************************
  File: GlobalConfig.h
  Copyright © 2017  Infiswift Inc. All rights reserved.

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
 **********************************************************************************************************************/

/* Purpose: To read configuration details from a configuration file file
 */

#include <stdbool.h>
#include <stdlib.h>

#include "ErrorCode.h"

#define MAX_SYSTEM_MSG_SIZE 1024
#define MAX_JSON_MSG_LENGTH	1024

// Make broker strings visible
extern char	MQTT_HOST_NAME[];
extern int	MQTT_PORT;
extern char	MQTT_CLIENT_ID[];
extern char	MQTT_USERNAME[];
extern char	MQTT_PASSWORD[];
extern int	MQTT_PROTOCOL_VERSION;
extern int	MQTT_KEEP_ALIVE_INTERVAL;
extern bool	MQTT_CLEAN_SESSION;
extern int 	MQTT_QOS;

// MQTT topics and file paths to saved topic files.
#define MAX_MQTT_TOPIC_ID_LENGTH 	1024
extern char MQTT_TOPIC_ID_TRACKER[];
extern char MQTT_TOPIC_ID_IRRADIANCES[];
extern char MQTT_TOPIC_ID_STOW[];
extern char MQTT_TOPIC_ID_STOW_WEBAPP[];
extern char MQTT_TOPIC_ID_STOW_RULES_ENGINE[];

// Servo settings
extern unsigned long SERVO_PWM_PERIOD_mSecs;

extern float SERVO_PWM_AZIMUTH_MIN_DUTY_CYCLE_mSecs;
extern float SERVO_PWM_AZIMUTH_MAX_DUTY_CYCLE_mSecs;
extern float SERVO_PWM_AZIMUTH_TOTAL_ANGLE_DEGREES;
extern float SERVO_PWM_AZIMUTH_STEP_SIZE_mSecs;

extern float SERVO_PWM_ALTITUDE_MIN_DUTY_CYCLE_mSecs;
extern float SERVO_PWM_ALTITUDE_MAX_DUTY_CYCLE_mSecs;
extern float SERVO_PWM_ALTITUDE_TOTAL_ANGLE_DEGREES;
extern float SERVO_PWM_ALTITUDE_STEP_SIZE_mSecs;

extern bool SERVO_PWM_AZIMUTH_REVERSE_POLARITY;
extern bool SERVO_PWM_ALTITUDE_REVERSE_POLARITY;

// Specify the start and stow positions
extern float SERVO_AZIMUTH_START_POSITION_PWM_mSecs;
extern float SERVO_ALTITUDE_START_POSITION_PWM_mSecs;
extern float SERVO_ALTITUDE_STOW_POSITION_PWM_mSecs;

// A/D settings
extern unsigned int LIGHT_SENSOR_LEFT_TOP;
extern unsigned int LIGHT_SENSOR_RIGHT_TOP;
extern unsigned int LIGHT_SENSOR_LEFT_BOTTOM;
extern unsigned int LIGHT_SENSOR_RIGHT_BOTTOM;
extern float LIGHT_SENSOR_MIN_DIFFERENCE_TO_MOVE_TRACKER;

// Tuning parameters for Ambient light sensors
#define NUM_LIGHT_SENSORS	4
extern float lightSensorScalar[NUM_LIGHT_SENSORS];
extern float lightSensorOffset[NUM_LIGHT_SENSORS];

// Stow settings
extern float STOW_MONITOR_RATE_SECS;

// Update interval
extern float PWM_UPDATE_RATE_SECONDS;
extern float BROKER_REPORT_RATE_SECONDS;

/**********************************************************************************************************************
 * Reads configuration values from the file path configured in BROKER_CONFIGURE_FILE_PATH and assign it to
 * corresponding variables
 *
 * Inputs :	configFile	- the configuration file to parse
 *
 * Return :	ERROR_COULD_NOT_OPEN_FILE if we could not open the file.
 * 					Otherwise, ERROR_SUCCESS.
 *
 * Outputs: None
 *
 * Notes  :	None
 *
 **********************************************************************************************************************/
ErrorCode_Type readAndAssignConfigurationDetails(char *configFile);


