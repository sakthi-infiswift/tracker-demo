/**********************************************************************************************************************
 File: GlobalConfig.c
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <jsmn.h>

#include "log.h"
#include "utilities/utilities.h"

#include "GlobalConfig.h"


// Variables declaration to hold configuration values
#define MAX_BROKER_NUMERIC_PARAMETER_LENGTH	10
#define MAX_PARAMETER_TEXT_LENGTH	1024

char MQTT_HOST_NAME[MAX_PARAMETER_TEXT_LENGTH];
int MQTT_PORT;
char MQTT_CLIENT_ID[MAX_PARAMETER_TEXT_LENGTH];
char MQTT_USERNAME[MAX_PARAMETER_TEXT_LENGTH];
char MQTT_PASSWORD[MAX_PARAMETER_TEXT_LENGTH];
int MQTT_KEEP_ALIVE_INTERVAL;
int MQTT_PROTOCOL_VERSION;
bool MQTT_CLEAN_SESSION;
int MQTT_QOS;

// MQTT Topic info
#define MAX_MQTT_TOPIC_ID_LENGTH 	1024
#define MAX_MQTT_TOPIC_FILEPATH_LENGTH	256

char MQTT_TOPIC_ID_TRACKER[MAX_MQTT_TOPIC_ID_LENGTH];
char MQTT_TOPIC_ID_IRRADIANCES[MAX_MQTT_TOPIC_ID_LENGTH];
char MQTT_TOPIC_ID_STOW[MAX_MQTT_TOPIC_ID_LENGTH];
char MQTT_TOPIC_ID_STOW_WEBAPP[MAX_MQTT_TOPIC_ID_LENGTH];
char MQTT_TOPIC_ID_STOW_RULES_ENGINE[MAX_MQTT_TOPIC_ID_LENGTH];

unsigned long SERVO_PWM_PERIOD_mSecs;
float SERVO_PWM_AZIMUTH_MIN_DUTY_CYCLE_mSecs;
float SERVO_PWM_AZIMUTH_MAX_DUTY_CYCLE_mSecs;
float SERVO_PWM_AZIMUTH_STEP_SIZE_mSecs;
float SERVO_PWM_AZIMUTH_TOTAL_ANGLE_DEGREES;

float SERVO_PWM_ALTITUDE_MIN_DUTY_CYCLE_mSecs;
float SERVO_PWM_ALTITUDE_MAX_DUTY_CYCLE_mSecs;
float SERVO_PWM_ALTITUDE_STEP_SIZE_mSecs;
float SERVO_PWM_ALTITUDE_TOTAL_ANGLE_DEGREES;

bool SERVO_PWM_AZIMUTH_REVERSE_POLARITY;
bool SERVO_PWM_ALTITUDE_REVERSE_POLARITY;

// Specify the stow position.
float SERVO_AZIMUTH_START_POSITION_PWM_mSecs;
float SERVO_ALTITUDE_START_POSITION_PWM_mSecs;
float SERVO_ALTITUDE_STOW_POSITION_PWM_mSecs;

// A/D settings
unsigned int LIGHT_SENSOR_LEFT_TOP;
unsigned int LIGHT_SENSOR_RIGHT_TOP;
unsigned int LIGHT_SENSOR_LEFT_BOTTOM;
unsigned int LIGHT_SENSOR_RIGHT_BOTTOM;
float LIGHT_SENSOR_MIN_DIFFERENCE_TO_MOVE_TRACKER;

// Tuning parameters for Ambient light sensors
float lightSensorScalar[NUM_LIGHT_SENSORS];
float lightSensorOffset[NUM_LIGHT_SENSORS];

// Stow settings
float STOW_MONITOR_RATE_SECS;

// Update interval
float PWM_UPDATE_RATE_SECONDS;
float BROKER_REPORT_RATE_SECONDS;

ErrorCode_Type readAndAssignConfigurationDetails(char *configFile)
{
	// File pointer
	FILE *pConfigFile;
	ErrorCode_Type error = ERROR_SUCCESS;

	// Opening the file in read mode
	if ((pConfigFile = fopen(configFile, "r")) == NULL)
	{
		error = ERROR_COULD_NOT_OPEN_FILE;
	}
	else
	{
		// Parse the configuration parameters and save them
		char configLine[MAX_PARAMETER_TEXT_LENGTH];

		// For parsing config buffer
		#define MAX_PARSE_TOKENS 10
		jsmn_parser Parser;
		jsmntok_t Tokens[MAX_PARSE_TOKENS];

		while (fgets(configLine, MAX_PARAMETER_TEXT_LENGTH, pConfigFile) != NULL)
		{
			// We've read a line from the config file and parse using JSMN parser library

			// Initialize the JSON parser
			jsmn_init(&Parser);

			int NumTokens = jsmn_parse(&Parser, configLine, strlen(configLine), Tokens, MAX_PARSE_TOKENS);

			if (NumTokens > 0)
			{
				// JSMN parsed configLine successfully
				char ParameterValue[MAX_PARAMETER_TEXT_LENGTH];
				char ParameterID[MAX_PARAMETER_TEXT_LENGTH];
				jsmntok_t TokenValue;
				jsmntok_t TokenKey;

				TokenKey = Tokens[0];
				if (TokenKey.type == JSMN_STRING)
				{
					TokenValue = Tokens[1];
					if (TokenValue.type == JSMN_PRIMITIVE)
					{
						int ValueLength;
						ValueLength = TokenKey.end - TokenKey.start;
						strncpy(ParameterID, &configLine[TokenKey.start], ValueLength);
						ParameterID[ValueLength] = '\0';

						ValueLength = TokenValue.end - TokenValue.start;
						strncpy(ParameterValue, &configLine[TokenValue.start], ValueLength);
						ParameterValue[ValueLength] = '\0';

						if (strncmp(ParameterID, "MQTT_HOST_NAME", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	strncpy(MQTT_HOST_NAME, ParameterValue, ValueLength);	}

						else if (strncmp(ParameterID, "MQTT_PORT", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	MQTT_PORT = atoi(ParameterValue);	}

						else if (strncmp(ParameterID, "MQTT_CLIENT_ID", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	strncpy(MQTT_CLIENT_ID, ParameterValue, ValueLength);	}

						else if (strncmp(ParameterID, "MQTT_USERNAME", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	strncpy(MQTT_USERNAME, ParameterValue, ValueLength);	}

						else if (strncmp(ParameterID, "MQTT_PASSWORD", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	strncpy(MQTT_PASSWORD, ParameterValue, ValueLength);	}

						else if (strncmp(ParameterID, "MQTT_KEEP_ALIVE_INTERVAL", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	MQTT_KEEP_ALIVE_INTERVAL = atoi(ParameterValue);	}

						else if (strncmp(ParameterID, "MQTT_PROTOCOL_VERSION", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	MQTT_PROTOCOL_VERSION = atoi(ParameterValue);	}

						else if (strncmp(ParameterID, "MQTT_CLEAN_SESSION", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	MQTT_CLEAN_SESSION = ((strncmp("true", ParameterValue, ValueLength) == 0) ? true : false); }

						else if (strncmp(ParameterID, "MQTT_QOS", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	MQTT_QOS = atoi(ParameterValue);	}

						else if (strncmp(ParameterID, "MQTT_TOPIC_ID_TRACKER", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	strncpy(MQTT_TOPIC_ID_TRACKER, ParameterValue, ValueLength);	}

						else if (strncmp(ParameterID, "MQTT_TOPIC_ID_IRRADIANCES", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	strncpy(MQTT_TOPIC_ID_IRRADIANCES, ParameterValue, ValueLength);	}

						else if (strncmp(ParameterID, "MQTT_TOPIC_ID_STOW", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	strncpy(MQTT_TOPIC_ID_STOW, ParameterValue, ValueLength);	}

						else if (strncmp(ParameterID, "MQTT_TOPIC_ID_STOW_WEBAPP", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	strncpy(MQTT_TOPIC_ID_STOW_WEBAPP, ParameterValue, ValueLength);	}

						else if (strncmp(ParameterID, "MQTT_TOPIC_ID_STOW_RULES_ENGINE", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	strncpy(MQTT_TOPIC_ID_STOW_RULES_ENGINE, ParameterValue, ValueLength);	}

						else if (strncmp(ParameterID, "SERVO_PWM_PERIOD_mSecs", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	SERVO_PWM_PERIOD_mSecs = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "SERVO_PWM_AZIMUTH_MIN_DUTY_CYCLE_mSecs", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	SERVO_PWM_AZIMUTH_MIN_DUTY_CYCLE_mSecs = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "SERVO_PWM_AZIMUTH_MAX_DUTY_CYCLE_mSecs", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	SERVO_PWM_AZIMUTH_MAX_DUTY_CYCLE_mSecs = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "SERVO_PWM_AZIMUTH_STEP_SIZE_mSecs", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	SERVO_PWM_AZIMUTH_STEP_SIZE_mSecs = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "SERVO_PWM_AZIMUTH_TOTAL_ANGLE_DEGREES", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	SERVO_PWM_AZIMUTH_TOTAL_ANGLE_DEGREES = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "SERVO_PWM_ALTITUDE_MIN_DUTY_CYCLE_mSecs", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	SERVO_PWM_ALTITUDE_MIN_DUTY_CYCLE_mSecs = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "SERVO_PWM_ALTITUDE_MAX_DUTY_CYCLE_mSecs", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	SERVO_PWM_ALTITUDE_MAX_DUTY_CYCLE_mSecs = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "SERVO_PWM_ALTITUDE_STEP_SIZE_mSecs", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	SERVO_PWM_ALTITUDE_STEP_SIZE_mSecs = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "SERVO_PWM_ALTITUDE_TOTAL_ANGLE_DEGREES", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	SERVO_PWM_ALTITUDE_TOTAL_ANGLE_DEGREES = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "SERVO_PWM_AZIMUTH_REVERSE_POLARITY", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	SERVO_PWM_AZIMUTH_REVERSE_POLARITY = (strncmp(ParameterValue, "true", MAX_PARAMETER_TEXT_LENGTH) == 0) ? true : false; }

						else if (strncmp(ParameterID, "SERVO_PWM_ALTITUDE_REVERSE_POLARITY", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	SERVO_PWM_ALTITUDE_REVERSE_POLARITY = (strncmp(ParameterValue, "true", MAX_PARAMETER_TEXT_LENGTH) == 0) ? true : false; }

						else if (strncmp(ParameterID, "SERVO_AZIMUTH_START_POSITION_PWM_mSecs", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	SERVO_AZIMUTH_START_POSITION_PWM_mSecs = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "SERVO_ALTITUDE_START_POSITION_PWM_mSecs", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	SERVO_ALTITUDE_START_POSITION_PWM_mSecs = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "SERVO_ALTITUDE_STOW_POSITION_PWM_mSecs", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	SERVO_ALTITUDE_STOW_POSITION_PWM_mSecs = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "LIGHT_SENSOR_LEFT_TOP", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	LIGHT_SENSOR_LEFT_TOP = atoi(ParameterValue);	}

						else if (strncmp(ParameterID, "LIGHT_SENSOR_RIGHT_TOP", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	LIGHT_SENSOR_RIGHT_TOP = atoi(ParameterValue);	}

						else if (strncmp(ParameterID, "LIGHT_SENSOR_LEFT_BOTTOM", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	LIGHT_SENSOR_LEFT_BOTTOM = atoi(ParameterValue);	}

						else if (strncmp(ParameterID, "LIGHT_SENSOR_RIGHT_BOTTOM", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	LIGHT_SENSOR_RIGHT_BOTTOM = atoi(ParameterValue);	}

						else if (strncmp(ParameterID, "LIGHT_SENSOR_SCALAR_AZIMUTH_LEFT_ALTITUDE_TOP", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	lightSensorScalar[LIGHT_SENSOR_LEFT_TOP] = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "LIGHT_SENSOR_OFFSET_AZIMUTH_LEFT_ALTITUDE_TOP", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	lightSensorOffset[LIGHT_SENSOR_LEFT_TOP] = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "LIGHT_SENSOR_SCALAR_AZIMUTH_RIGHT_ALTITUDE_TOP", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	lightSensorScalar[LIGHT_SENSOR_RIGHT_TOP] = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "LIGHT_SENSOR_OFFSET_AZIMUTH_RIGHT_ALTITUDE_TOP", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	lightSensorOffset[LIGHT_SENSOR_RIGHT_TOP] = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "LIGHT_SENSOR_SCALAR_AZIMUTH_LEFT_ALTITUDE_BOTTOM", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	lightSensorScalar[LIGHT_SENSOR_LEFT_BOTTOM] = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "LIGHT_SENSOR_OFFSET_AZIMUTH_LEFT_ALTITUDE_BOTTOM", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	lightSensorOffset[LIGHT_SENSOR_LEFT_BOTTOM] = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "LIGHT_SENSOR_SCALAR_AZIMUTH_RIGHT_ALTITUDE_BOTTOM", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	lightSensorScalar[LIGHT_SENSOR_RIGHT_BOTTOM] = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "LIGHT_SENSOR_OFFSET_AZIMUTH_RIGHT_ALTITUDE_BOTTOM", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	lightSensorOffset[LIGHT_SENSOR_RIGHT_BOTTOM] = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "LIGHT_SENSOR_MIN_DIFFERENCE_TO_MOVE_TRACKER", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	LIGHT_SENSOR_MIN_DIFFERENCE_TO_MOVE_TRACKER = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "STOW_MONITOR_RATE_SECS", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	STOW_MONITOR_RATE_SECS = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "PWM_UPDATE_RATE_SECONDS", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	PWM_UPDATE_RATE_SECONDS = atof(ParameterValue);	}

						else if (strncmp(ParameterID, "BROKER_REPORT_RATE_SECONDS", MAX_PARAMETER_TEXT_LENGTH) == 0)
						{	BROKER_REPORT_RATE_SECONDS = atof(ParameterValue);	}
					}
				}
			}
			else
			{	// This line doesn't contain a configuration parameter

			}
		}
		// Close the file descriptor
		fclose(pConfigFile);
	}
	return error;
}

