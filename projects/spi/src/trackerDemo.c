/**********************************************************************************************************************
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
 Purpose: This code reads a data file, and sends it to the broker. It removes that file from the filesystem when
 its successfully published.
 ***********************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>

// Needed for BROKER configuration details and general functions
#include "GlobalConfig.h"
#include "BBGW/GPIO/GPIO.h"
//#include "mosquitto/mosquittoClient.h"
#include "log.h"
#include "utilities/utilities.h"
#include "project.h"
#include "ErrorCode.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variable declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// We'll use mutexes to protect the data we report to the broker.
pthread_mutex_t lockIrradianceData;
// Light sensor voltages are shared between threads
float irradianceV[4];

bool panelIsStowed = false;				// Overall stow status
bool stowButtonIsPressed = false;	// Stow hw button

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private function declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/**********************************************************************************************************************
 * A thread to periodically report to the broker.
 *
 * Inputs: 	pThread - Thread identifier.
 *
 * Outputs:	None
 *
 * Notes: 	Node
 **********************************************************************************************************************/
//void *ReportToBroker_thread(void *pThread);

/**********************************************************************************************************************
 * A thread to periodically read the stow input. If the pushbutton is pressed, we toggle the stow state.
 *
 * Inputs: 	pThread - Thread identifier.
 *
 * Outputs:	None
 *
 * Notes: 	Node
 **********************************************************************************************************************/
void *MonitorStowPushbutton_thread(void *pThread);

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		char configFile[MAX_MESSAGE_LENGTH];
		if ((strncmp(argv[1], "-v", strlen("-v")) == 0) ||
		    (strncmp(argv[1], "--version", strlen("--version")) == 0))
		{	// Display version
			char LogMsg[MAX_MESSAGE_LENGTH];
			snprintf(LogMsg, MAX_MESSAGE_LENGTH, "trackerDemo version = %s", FW_VERSION);
			LogMessage(LOG_INFO, LogMsg);
			exit(0);
		}
		else
		{	// Normal execution
			strncpy(configFile, argv[1], MAX_MESSAGE_LENGTH);
		}

		ErrorCode_Type error = readAndAssignConfigurationDetails(configFile);
		if (error != ERROR_SUCCESS)
		{
			char ErrorMessage[MAX_MESSAGE_LENGTH];
			snprintf(ErrorMessage, sizeof(ErrorMessage), "Failed to open configuration file %s", configFile);
			LogMessage(LOG_ERR, ErrorMessage);
			exit(0);
		}
		// else, we parsed the specified file
	}
	else
	{
		printf("No config file specified. It must be specified.\n");
		exit(0);
	}

	#if !defined(DESKTOP)
		DiscreteIO_init(GPIO_STOW_PIN, GPIO_INPUT);	// Configure stow input
		AtoD_init();		// configure A/D
	#endif

	// configure PWM EHRPWM1A for the SG90 micro servo.
	// See http://www.micropik.com/PDF/SG90Servo.pdf.
	PWM_init(EHRPWM1A, SERVO_PWM_PERIOD_mSecs, SERVO_PWM_AZIMUTH_MIN_DUTY_CYCLE_mSecs,
		SERVO_PWM_AZIMUTH_MAX_DUTY_CYCLE_mSecs, SERVO_PWM_AZIMUTH_STEP_SIZE_mSecs);

	PWM_init(EHRPWM1B, SERVO_PWM_PERIOD_mSecs, SERVO_PWM_ALTITUDE_MIN_DUTY_CYCLE_mSecs,
		SERVO_PWM_ALTITUDE_MAX_DUTY_CYCLE_mSecs, SERVO_PWM_ALTITUDE_STEP_SIZE_mSecs);

	// Delay to allow BBGW configurations to take affect. How long?
	sleep(1);

	/* Set PWM positions to known states. We need to enable/set the PWM to get a valid current duty cycle.
	 * Alternatively, we could initialize this as part of init, but I prefer to keep configuration and
	 * setting the position separate.
	 */
	PWM_setPosition(EHRPWM1A, SERVO_AZIMUTH_START_POSITION_PWM_mSecs);
	printf("Panel azimuth initialized at %f ms.\n", SERVO_AZIMUTH_START_POSITION_PWM_mSecs);
	PWM_setPosition(EHRPWM1B, SERVO_ALTITUDE_START_POSITION_PWM_mSecs);
	printf("Panel altitude initialized at %f ms.\n", SERVO_ALTITUDE_START_POSITION_PWM_mSecs);
	sleep(1);		// Let servo move to middle positions.
#if 0
	// Create a thread for reporting to the broker
	pthread_t ReportToBroker;
	if (pthread_create(&ReportToBroker, NULL, ReportToBroker_thread, NULL) != 0)
	{
		printf("Could not create thread ReportToBroker_thread\n");
	}
	// else, successfully created the thread
#endif
	// Create a thread for monitoring the stow pushbutton
	pthread_t MonitorStowPushbutton;
	if (pthread_create(&MonitorStowPushbutton, NULL, MonitorStowPushbutton_thread, NULL) != 0)
	{
		printf("Could not create thread MonitorStowPushbutton_thread\n");
	}
	// else

	while(1)
	{	// This loop runs periodically
		/* Get current time so we can measure how long it takes to execute this loop.
		 * We need this to make it execute at a more accurate rate.
		 */
		struct timeval startTime;
		gettimeofday(&startTime, NULL);

		// Read the light sensors
		pthread_mutex_lock(&lockIrradianceData);
		irradianceV[LIGHT_SENSOR_LEFT_TOP] = AtoD_read(LIGHT_SENSOR_LEFT_TOP);
		irradianceV[LIGHT_SENSOR_RIGHT_TOP] = AtoD_read(LIGHT_SENSOR_RIGHT_TOP);
		irradianceV[LIGHT_SENSOR_LEFT_BOTTOM] = AtoD_read(LIGHT_SENSOR_LEFT_BOTTOM);
		irradianceV[LIGHT_SENSOR_RIGHT_BOTTOM] = AtoD_read(LIGHT_SENSOR_RIGHT_BOTTOM);
		printf("\n");
		//printf("Initial A2Ds Left_Top:%1.2f, Right_Top:%1.2f, Left_Bottom:%1.2f, Right_Bottom:%1.2f\n",
		//	irradianceV[LIGHT_SENSOR_LEFT_TOP], irradianceV[LIGHT_SENSOR_RIGHT_TOP],
		//	irradianceV[LIGHT_SENSOR_LEFT_BOTTOM], irradianceV[LIGHT_SENSOR_RIGHT_BOTTOM]);

		// Scale light sensor values
		irradianceV[LIGHT_SENSOR_LEFT_TOP]     = lightSensorScalar[LIGHT_SENSOR_LEFT_TOP]     * irradianceV[LIGHT_SENSOR_LEFT_TOP]
		                                       + lightSensorOffset[LIGHT_SENSOR_LEFT_TOP];
		irradianceV[LIGHT_SENSOR_RIGHT_TOP]    = lightSensorScalar[LIGHT_SENSOR_RIGHT_TOP]    * irradianceV[LIGHT_SENSOR_RIGHT_TOP]
		                                       + lightSensorOffset[LIGHT_SENSOR_RIGHT_TOP];
		irradianceV[LIGHT_SENSOR_LEFT_BOTTOM]  = lightSensorScalar[LIGHT_SENSOR_LEFT_BOTTOM]  * irradianceV[LIGHT_SENSOR_LEFT_BOTTOM]
		                                       + lightSensorOffset[LIGHT_SENSOR_LEFT_BOTTOM];
		irradianceV[LIGHT_SENSOR_RIGHT_BOTTOM] = lightSensorScalar[LIGHT_SENSOR_RIGHT_BOTTOM] * irradianceV[LIGHT_SENSOR_RIGHT_BOTTOM]
		                                       + lightSensorOffset[LIGHT_SENSOR_RIGHT_BOTTOM];
		printf("Scaled A2Ds Left_Top:%1.2f, Right_Top:%1.2f, Left_Bottom:%1.2f, Right_Bottom:%1.2f\n",
			irradianceV[LIGHT_SENSOR_LEFT_TOP], irradianceV[LIGHT_SENSOR_RIGHT_TOP],
			irradianceV[LIGHT_SENSOR_LEFT_BOTTOM], irradianceV[LIGHT_SENSOR_RIGHT_BOTTOM]);
		pthread_mutex_unlock(&lockIrradianceData);

		// Determine how to drive the servo motor
		if (panelIsStowed)
		{
			PWM_setPosition(EHRPWM1B, SERVO_ALTITUDE_STOW_POSITION_PWM_mSecs);
			printf("Panel altitude stowed at %f ms.\n", SERVO_ALTITUDE_STOW_POSITION_PWM_mSecs);
			PWM_disable(EHRPWM1B);
		}
		else
		{	// Not stowed. Tracking the light sensors
			/** Azimuth Tracking */
			float horizontalDifferential =	// left side minus right side
				(irradianceV[LIGHT_SENSOR_LEFT_TOP]  + irradianceV[LIGHT_SENSOR_LEFT_BOTTOM]) -
				(irradianceV[LIGHT_SENSOR_RIGHT_TOP] + irradianceV[LIGHT_SENSOR_RIGHT_BOTTOM]);
			printf("Horiz diff: %f\n", horizontalDifferential);

			bool steppedAz = false;
			bool steppedRight = false;
			if (horizontalDifferential > LIGHT_SENSOR_MIN_DIFFERENCE_TO_MOVE_TRACKER)
			{	// Step servo to the right
				steppedAz = PWM_step(EHRPWM1A, !SERVO_PWM_AZIMUTH_REVERSE_POLARITY);
				steppedRight = true;
			}
			else if (horizontalDifferential < (-LIGHT_SENSOR_MIN_DIFFERENCE_TO_MOVE_TRACKER))
			{	// Step servo to the left
				steppedAz = PWM_step(EHRPWM1A, SERVO_PWM_AZIMUTH_REVERSE_POLARITY);
				steppedRight = false;
			}
			// else, differential is too small to change panel position

			printf("PWM Azimuth: ");
			if (steppedAz)
			{	// We're still stepping the servo. We're not at the limits yet.
				if (steppedRight)
				{
					printf("step right, ");
				}
				else
				{
					printf("step left, ");
				}
			}
			else
			{	// servo didn't step. At a range limit.
				printf("at limit or no position change, ");
			}
			printf("DutyCycle = %f ms.\n", PWM_getDutyCycle_mSecs(EHRPWM1A));

			/** Altitude Tracking */
			float altidudeDifferential =		// top side minus bottom side
				(irradianceV[LIGHT_SENSOR_LEFT_TOP]    + irradianceV[LIGHT_SENSOR_RIGHT_TOP]) -
				(irradianceV[LIGHT_SENSOR_LEFT_BOTTOM] + irradianceV[LIGHT_SENSOR_RIGHT_BOTTOM]);
			printf("Altitude diff: %f\n", altidudeDifferential);

			bool steppedAlt = false;
			bool steppedUp = false;
			if (altidudeDifferential > LIGHT_SENSOR_MIN_DIFFERENCE_TO_MOVE_TRACKER)
			{	// Step servo to the right
				steppedAlt = PWM_step(EHRPWM1B, !SERVO_PWM_ALTITUDE_REVERSE_POLARITY);
				steppedUp = false;
			}
			else if (altidudeDifferential < (-LIGHT_SENSOR_MIN_DIFFERENCE_TO_MOVE_TRACKER))
			{	// Step servo to the left
				steppedAlt = PWM_step(EHRPWM1B, SERVO_PWM_ALTITUDE_REVERSE_POLARITY);
				steppedUp = true;
			}
			// else, differential is too small to change panel position

			printf("PWM Altitude: ");
			if (steppedAlt)
			{	// We're still stepping the servo. We're not at the limits yet.
				if (steppedUp)
				{
					printf("step up, ");
				}
				else
				{
					printf("step down, ");
				}
			}
			else
			{	// servo didn't step. At a range limit.
				printf("at limit or no position change, ");
			}
			printf("DutyCycle = %f ms.\n", PWM_getDutyCycle_mSecs(EHRPWM1B));
		}

		struct timeval endTime;
		gettimeofday(&endTime, NULL);
		unsigned long diffTime_usec = ((endTime.tv_sec * 1e6) + endTime.tv_usec) -
				((startTime.tv_sec * 1e6) + startTime.tv_usec);
		usleep((unsigned int)(((unsigned long)(1e6 * PWM_UPDATE_RATE_SECONDS)) - diffTime_usec), NULL);
	}

        /*Sakthi:*/
	//shutdownMQTT();

	// Exit from Main
	return 1;	// some error
}
#if 0
void *ReportToBroker_thread(void *pThread)
{
	ErrorCode_Type error;
	char ErrorMessage[MAX_MESSAGE_LENGTH];

	error = MQTT_init();		// Initialize mosquitto client
	if (error != ERROR_SUCCESS)
	{
		printf("Failed to configure Mosquitto MQTT client: %d\n", error);
		snprintf(ErrorMessage, MAX_MESSAGE_LENGTH, "Failed to configure Mosquitto MQTT client: %d", error);
		LogMessage(LOG_ERR, ErrorMessage);
		exit(1);
	}

	error = MQTT_subscribe(&StowWebApp_MsgID, MQTT_TOPIC_ID_STOW_WEBAPP, 1);
	if (error != ERROR_SUCCESS)
	{
		printf("Failed to configure Mosquitto MQTT client: %d\n", error);
		snprintf(ErrorMessage, MAX_MESSAGE_LENGTH, "Failed to configure Mosquitto MQTT client: %d", error);
		LogMessage(LOG_ERR, ErrorMessage);
		exit(1);
	}

	error = MQTT_subscribe(&StowRulesEngine_MsgID, MQTT_TOPIC_ID_STOW_RULES_ENGINE, 1);
	if (error != ERROR_SUCCESS)
	{
		printf("Failed to configure Mosquitto MQTT client: %d\n", error);
		snprintf(ErrorMessage, MAX_MESSAGE_LENGTH, "Failed to configure Mosquitto MQTT client: %d", error);
		LogMessage(LOG_ERR, ErrorMessage);
		exit(1);
	}

	while (1)
	{
		struct timeval brokerThread_StartTime;
		gettimeofday(&brokerThread_StartTime, NULL);

		// Report voltage to the broker
		char JSON_Msg[MAX_JSON_MSG_LENGTH];
		// format: {"cl_irradiance_1":100, "cl_irradiance_2":100,"cl_irradiance_3":100,"cl_irradiance_4":100,"timestamp_utc":1493187991042}
		pthread_mutex_lock(&lockIrradianceData);
		snprintf(JSON_Msg, MAX_JSON_MSG_LENGTH,
			"{\"cl_irradiance_1\":%d, \"cl_irradiance_2\":%d, \"cl_irradiance_3\":%d, \"cl_irradiance_4\":%d, \"timestamp_utc\":%ld}",
			(unsigned int)(irradianceV[0] * A2D_BIT_RANGE), (unsigned int)(irradianceV[1] * A2D_BIT_RANGE),
			(unsigned int)(irradianceV[2] * A2D_BIT_RANGE), (unsigned int)(irradianceV[3] * A2D_BIT_RANGE),
			brokerThread_StartTime.tv_sec);
		pthread_mutex_unlock(&lockIrradianceData);
		MQTT_publish(MQTT_TOPIC_ID_IRRADIANCES, JSON_Msg);

		/* Report the stow status to the broker.
		 * Because this is a single variable, we don't protect it with a mutex.
		 * {"cl_stow_status":1,"cl_stow_button_status": 0, "timestamp_utc":1493187991042}
		 */
		snprintf(JSON_Msg, MAX_JSON_MSG_LENGTH,
			"{\"cl_stow_status\":%d, \"cl_stow_button_status\":%d, \"timestamp_utc\":%ld}",
			panelIsStowed, stowButtonIsPressed, brokerThread_StartTime.tv_sec);
		//printf("Publish: %s\n", JSON_Msg);
		MQTT_publish(MQTT_TOPIC_ID_STOW, JSON_Msg);

		/* Publish the servo position
		 * format: {"cl_position_oa":0,"cl_position_ob":5,"cl_position_og":70,"timestamp_utc":1493187991042}
		 */
		float ServoAzimuthAngle_degrees =
			( (PWM_getDutyCycle_mSecs(EHRPWM1A) - SERVO_PWM_AZIMUTH_MIN_DUTY_CYCLE_mSecs) /
			  (SERVO_PWM_AZIMUTH_MAX_DUTY_CYCLE_mSecs - SERVO_PWM_AZIMUTH_MIN_DUTY_CYCLE_mSecs))
				* SERVO_PWM_AZIMUTH_TOTAL_ANGLE_DEGREES;
		float ServoAltitudeAngle_degrees =
			( (PWM_getDutyCycle_mSecs(EHRPWM1B) - SERVO_PWM_ALTITUDE_MIN_DUTY_CYCLE_mSecs) /
			  (SERVO_PWM_ALTITUDE_MAX_DUTY_CYCLE_mSecs - SERVO_PWM_ALTITUDE_MIN_DUTY_CYCLE_mSecs))
				* SERVO_PWM_ALTITUDE_TOTAL_ANGLE_DEGREES;

		/* The web app seems to expect us to report the following calculations. Why we just don't report position, and do this
		 * in the web app, I don't know.
		 * 0a: ServoAltitudeAngle_degrees  ### 180 - ServoAzimuthAngle_degrees
		 * 0b: ServoAzimuthAngle_degrees	### arcsine(sin(ServoAltitudeAngle_degrees) * cos(ServoAzimuthAngle_degrees))
		 * 0g: arcsine(sin(ServoAltitudeAngle_degrees) * sin(ServoAzimuthAngle_degrees))
		 */
		float Oa = 110.0 + ServoAzimuthAngle_degrees; // 90.0 - ServoAzimuthAngle_degrees;
		float Ob = ServoAltitudeAngle_degrees;
			// ConvertRadiansToDegrees(asinf(
			//	sinf(ConvertDegreesToRadians(ServoAltitudeAngle_degrees)) * cosf(ConvertDegreesToRadians(ServoAzimuthAngle_degrees))));
		float Og = 0;
				// ConvertRadiansToDegrees(asinf(
				// sinf(ConvertDegreesToRadians(ServoAltitudeAngle_degrees)) * sinf(ConvertDegreesToRadians(ServoAzimuthAngle_degrees))));
		snprintf(JSON_Msg, MAX_JSON_MSG_LENGTH,
			"{\"cl_position_oa\":%f, \"cl_position_ob\":%f, \"cl_position_og\":%f, \"timestamp_utc\":%ld}",
			Oa, Ob, Og,
			brokerThread_StartTime.tv_sec);
		//printf("Publish: %s\n", JSON_Msg);
		MQTT_publish(MQTT_TOPIC_ID_TRACKER, JSON_Msg);


		// Sleep until it's time for the next report
		struct timeval brokerThread_EndTime;
		gettimeofday(&brokerThread_EndTime, NULL);
		unsigned long diffTime_usec = ((brokerThread_EndTime.tv_sec * 1e6) + brokerThread_EndTime.tv_usec) -
				((brokerThread_StartTime.tv_sec * 1e6) + brokerThread_StartTime.tv_usec);
		usleep((unsigned int)(((unsigned long)(1e6 * BROKER_REPORT_RATE_SECONDS)) - diffTime_usec), NULL);
	}
}
#endif

#define STOW_INPUT_DEBOUNCE		3
void *MonitorStowPushbutton_thread(void *pThread)
{
	GPIO_State currentStowInput, previousStowInput;	// debouncing variables
	unsigned int stowDebounceCount = 0;
	bool debouceInProgress = false;

	// Initialize previous stow input
	#if defined(DESKTOP)
		previousStowInput = GPIO_HIGH;
	#else
		previousStowInput = DiscreteIO_read(GPIO_STOW_PIN);
	#endif

	while (1)
	{
		#if defined(DESKTOP)
			currentStowInput = GPIO_HIGH;
		#else
			currentStowInput = DiscreteIO_read(GPIO_STOW_PIN);
		#endif

		if (currentStowInput != previousStowInput)
		{
			stowDebounceCount = 0;
			debouceInProgress = true;
			printf("Stow switch is %d\n", currentStowInput);
		}
		// else, no change in pushbutton state

		if (debouceInProgress)
		{
			if (stowDebounceCount < STOW_INPUT_DEBOUNCE)
			{	// still debouncing
				++stowDebounceCount;
			}
			else
			{	// Debouncing complete
				debouceInProgress = false;

				if (currentStowInput == GPIO_HIGH)
				{	// Pushbutton has been pressed
					stowButtonIsPressed = true;
				}
				else if (currentStowInput == GPIO_LOW)
				{	// Pushbutton has been released
					stowButtonIsPressed = false;
				}
				printf("Debounce complete. StowbuttonIsPressed = %d", stowButtonIsPressed);
			}
		}
		// else, we're done debouncing the input change
		previousStowInput = currentStowInput;
		/*Sakthi : panelIsStowed line changed, have enable when mosquitto is available*/
		panelIsStowed = stowButtonIsPressed;
		//panelIsStowed = stowButtonIsPressed || (StowCmd_WebApp == 1) || (StowCmd_RulesEngine == 1);

		/* Wait until the next time to check the input.
		 * We don't worry about counting for the time this thread takes, because:
		 * 1. It doesn't do much, and won't take much time.
		 * 2. This monitoring doesn't need to be that precise.
		 */
		usleep((unsigned int)((unsigned long)(1e6 * STOW_MONITOR_RATE_SECS)), NULL);
	}
}
