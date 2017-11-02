/**********************************************************************************************************************
 File: mosquittoClient.c
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
#include <mosquitto.h>
#include <stdbool.h>
#include <string.h>
#include <jsmn.h>

#include "GlobalConfig.h"
#include "ErrorCode.h"
#include "mosquittoClient.h"

struct mosquitto *pMosq;

int StowWebApp_MsgID;
int StowRulesEngine_MsgID;
int StowCmd_WebApp = 0;
int StowCmd_RulesEngine = 0;

/**********************************************************************************************************************
 * Callback function from the MQTT broker on a connection request
 *
 * Inputs:	mosq		- Pointer to the mosquitto client instance.
 * 					obj			- Pointer to something(?)
 * 					result	- numerical result
 *
 * Outputs: None
 *
 * Returns:	None
 *
 * Notes:		None
 **********************************************************************************************************************/
void MQTT_callback_connect(struct mosquitto *mosq, void *obj, int returnCode);

/**********************************************************************************************************************
 * Callback function from the MQTT broker on a publish request
 *
 * Inputs:	mosq		- Pointer to the mosquitto client instance.
 * 					obj			- Pointer to something(?)
 * 					msgID		- numerical result
 *
 * Outputs: None
 *
 * Returns:	None
 *
 * Notes:		None
 **********************************************************************************************************************/
void MQTT_callback_publish(struct mosquitto *mosq, void *obj, int msgID);


/**********************************************************************************************************************
 * Callback function from the MQTT broker on a publish request
 *
 * Inputs:	mosq		- Pointer to the mosquitto client instance.
 * 					obj			- Pointer to object we passed in mqtt_new. We don't use this, so it's NULL.
 * 					msgID		- The ID of the client sending this message.
 * 					QoS_count	- The # of granted subscriptions
 * 					granted_QoS	- An array of the QoS level for each granted subscription.
 *
 * Outputs: None
 *
 * Returns:	None
 *
 * Notes:		None
 **********************************************************************************************************************/
void MQTT_callback_subscribe(struct mosquitto *mosq, void *obj, int msgID, int QoS_count, const int *granted_QoS);

/**********************************************************************************************************************
 * Callback function for when a message is received from the broker. Theses would be messages we subscribed to.
 *
 * Inputs:	mosq		- Pointer to the mosquitto client instance.
 * 					obj			- Pointer to object we passed in mqtt_new. We don't use this, so it's NULL.
 * 					message	- A structure that has the topic, message, and other data related to the message.
 *
 * Outputs: None
 *
 * Returns:	None
 *
 * Notes:		None
 **********************************************************************************************************************/
void MQTT_callback_message(struct mosquitto *mosq, void *obj,  const struct mosquitto_message *message);

ErrorCode_Type MQTT_init()
{
	int result;
	ErrorCode_Type error = ERROR_SUCCESS;

	mosquitto_lib_init();
	// Display library version
	int major, minor, revision;
	mosquitto_lib_version(&major, &minor, &revision);

	// Get instance of mosquitto client
	pMosq = mosquitto_new(MQTT_CLIENT_ID, true, 0);

	// Specify login credentials
	result = mosquitto_username_pw_set(pMosq, MQTT_USERNAME, MQTT_PASSWORD);
	if (result == MOSQ_ERR_SUCCESS)
	{
		// Set protocol version to 3.1.1
		unsigned int MQTTProtocol = 4;
		mosquitto_opts_set(pMosq, MOSQ_OPT_PROTOCOL_VERSION, &MQTTProtocol);

		// Register callback functions
		mosquitto_connect_callback_set(pMosq, MQTT_callback_connect);
		mosquitto_publish_callback_set(pMosq, MQTT_callback_publish);
		mosquitto_subscribe_callback_set(pMosq, MQTT_callback_subscribe);
		mosquitto_message_callback_set(pMosq, MQTT_callback_message);

		// Connect to the broker
		//printf("host: %s, port: %d, Ka: %d\n", MQTT_HOST_NAME, MQTT_PORT, MQTT_KEEP_ALIVE_INTERVAL);
		result = mosquitto_connect(pMosq, MQTT_HOST_NAME, MQTT_PORT, MQTT_KEEP_ALIVE_INTERVAL);
		if (result == MOSQ_ERR_SUCCESS)
		{
		}
		else
		{
			char errorMsg[1024];
			strerror_r(result, errorMsg, 1024);
			printf("Mosq connect error: %s\n", errorMsg);
			error = ERROR_MQTT_CONNECTING;
		}

		// Start the mosquitto message processing loop
		result =  mosquitto_loop_start(pMosq);
		if (result == MOSQ_ERR_SUCCESS)
		{
		}
		else
		{
			char errorMsg[1024];
			strerror_r(result, errorMsg, 1024);
			printf("Mosq threaded loop error: %s\n", errorMsg);
			error = ERROR_MQTT_LOOPING;
		}
	}
	else
	{
		error = ERROR_MQTT_SETTING_CREDENTIALS;
	}

	return error;
}

ErrorCode_Type MQTT_publish(char *topic, char *msg)
{
	ErrorCode_Type error = ERROR_SUCCESS;
	//printf("Publishing Topic: \"%s\", msg: \"%s\"\n", topic, msg);

	int result = mosquitto_publish(pMosq, NULL, topic, strlen(msg), msg, MQTT_QOS, false);
	if (result != MOSQ_ERR_SUCCESS)
	{
		error = ERROR_MQTT_PUBLISHING;
	}
	return error;
}

ErrorCode_Type MQTT_loop()
{
	int result;
	ErrorCode_Type error = ERROR_SUCCESS;
	/* Call the mosquitto loop process
	 * Inputs are, in order:
	 * mosq:				mosquitto instance
	 * timeout:			How many milliseconds we'll look for packets. Because I'm reporting to the broker every 1
	 * 							second, I set this to half of that, to allow time for other tasks in this thread to be completed.
	 * max_packets:	unused. Spec says to set to 1.
	 */
	result = mosquitto_loop(pMosq, 500, 1);
	if (result != MOSQ_ERR_SUCCESS)
	{
		error = ERROR_MQTT_LOOPING;
	}
	return error;
}


ErrorCode_Type MQTT_subscribe(int *msgID, char *msgPattern, int QoS)
{
	int result;
	ErrorCode_Type error = ERROR_SUCCESS;
	result = mosquitto_subscribe(pMosq, msgID, msgPattern, QoS);
	if (result != MOSQ_ERR_SUCCESS)
	{
		error = ERROR_MQTT_SUBSCRIBE;
	}

	printf("Id=%d for pattern %s\n", *msgID, msgPattern);
	return error;
}

void shutdownMQTT()
{
	mosquitto_disconnect(pMosq);
	mosquitto_loop_stop(pMosq,false);
	mosquitto_destroy(pMosq);
	mosquitto_lib_cleanup();
}

void MQTT_callback_publish(struct mosquitto *mosq, void *obj, int msgID)
{
	// printf("publish callback, msdID=%d\n", msgID);
}

void MQTT_callback_connect(struct mosquitto *mosq, void *obj, int returnCode)
{
	// printf("connect callback, return code=%d\n", returnCode);
}

void MQTT_callback_subscribe(struct mosquitto *mosq, void *obj, int msgID, int QoS_count, const int *granted_qos)
{
	// printf("subscribe callback, ID=%d\n", msgID);
}

void MQTT_callback_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
	// For parsing JSON message
	#define MAX_JSON_ELEMENT_LENGTH	100
	jsmn_parser parser;
	jsmntok_t tokens[MAX_JSON_ELEMENT_LENGTH];

	// Initialize the JSON parser
	jsmn_init(&parser);

	printf("Subscribed msg id: %d, Topic: %s, Payload: %s\n", message->mid, message->topic, (char*) message->payload);
	char JSONMsg[message->payloadlen];
	strncpy(JSONMsg, (char*) message->payload, message->payloadlen);

	//if (message->mid == StowMsgID)
	//{
			int numTokens = jsmn_parse(&parser, JSONMsg, strlen(JSONMsg), tokens, 10);
			if (numTokens >= 0)
			{	// Successfully parsed the JSON string
				char valueStr[MAX_JSON_MSG_LENGTH];
				char keyStr[MAX_JSON_MSG_LENGTH];
				jsmntok_t value;
				jsmntok_t key;

				/* Check the tokens to see if we have the stow command.
				 * Skip the first token pair, which is the entire JSON msg.
				 * Since we're looking at 2 items on each pass, a token and value,
				 * we index by 2.
				 */
				for (int i = 1; i < numTokens; i += 2)
				{
					key = tokens[i];
					if (key.type == JSMN_STRING)
					{	// This can be a key/value pair
						// check the value
						value = tokens[i+1];
						if (value.type == JSMN_PRIMITIVE)
						{	// This is possibly a value we need
							int length;
							length = key.end - key.start;
							strncpy(keyStr, &JSONMsg[key.start], length);
							keyStr[length] = '\0';

							length = value.end - value.start;
							strncpy(valueStr, &JSONMsg[value.start], length);
							valueStr[length] = '\0';

							if (strcmp(keyStr, "stow_status") == 0)
							{
								StowCmd_WebApp = atoi(valueStr);
								printf("WebApp stow cmd = %d\n", StowCmd_WebApp);
							}

							if (strcmp(keyStr, "rules_engine_stow_status") == 0)
							{
								StowCmd_RulesEngine = atoi(valueStr);
								printf("Rules Engine stow cmd = %d\n", StowCmd_RulesEngine);
							}
						}
						// else, the potential value type is not a primitive. Cannot be a valid value.
					}	// else, key is not a string. Cannot be a valid key.
				}	// for
			}
			else
			{	// Error parsing the JSON string
				printf("Could not parse JSON msg: %d\n", numTokens);
			}
	//}
}
