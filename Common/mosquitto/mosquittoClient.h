/**********************************************************************************************************************
  File: mosquittoClient.h
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

/* Purpose: An MQTT client that publishes messages to a broker.
 */

#include "ErrorCode.h"

// Stow parameters. Todo: These shouldn't be in a non-project file.
// Message ID for the stow command
extern int StowWebApp_MsgID;
extern int StowRulesEngine_MsgID;
extern int StowCmd_WebApp;
extern int StowCmd_RulesEngine;

/**********************************************************************************************************************
 * Performs initialization for the MQTT client. Must be called before publishing any messages to the broker.
 *
 * Inputs:	None
 *
 * Outputs: None
 *
 * Returns:	ERROR_SUCCESS on success.
 * 					ERROR_MQTT_SETTING_CREDENTIALS if we cannot configure username and password.
 * 					ERROR_MQTT_CONNECTING if we could not connect to the broker.
 *
 *
 * Notes:		None
 **********************************************************************************************************************/
ErrorCode_Type MQTT_init(void);

/**********************************************************************************************************************
 * Mosquitto needs this to be called periodically to process communications. Published messages are typically sent
 * immediately, but receiving messages are not.
 *
 *
 * Inputs:	topic	- The topic to publish
 * 					msg		- The JSON message to publish
 *
 * Outputs: None
 *
 * Returns:	ERROR_SUCCESS on success.
 * 					ERROR_MQTT_LOOPING if we get an error.
 *
 * Notes:		The code currently sends messages to the broker every 1 second. We'll call this at that rate. This will
 * 					introduce some delay in receiving messages we've subscribed to.
 **********************************************************************************************************************/
ErrorCode_Type MQTT_loop(void);

/**********************************************************************************************************************
 * Publishes the topic and message to the MQTT broker.
 *
 * Inputs:	topic	- The topic to publish
 * 					msg		- The JSON message to publish
 *
 * Outputs: None
 *
 * Returns:	ERROR_SUCCESS on success.
 * 					ERROR_MQTT_PUBLISHING if we couldn't publish.
 *
 * Notes:		Note that the broker URL, port, username and password are configured in a config file, and are globally
 * 					available.
 **********************************************************************************************************************/
ErrorCode_Type MQTT_publish(char *topic, char *msg);

/**********************************************************************************************************************
 * Callback function from the MQTT broker for a subscription
 *
 * Inputs:	msgID			- An ID returned as part of the subscription.
 * 					msgPattern- The ID of the topic to subscribe to
 * 					Qos				- Quality of service level
 *
 * Outputs: None
 *
 * Returns:	ERROR_SUCCESS on success.
 * 					ERROR_MQTT_SUBSCRIBE if we couldn't subscribe.
 *
 * Notes:		None
 **********************************************************************************************************************/
ErrorCode_Type MQTT_subscribe(int *msgID, char *msgPattern, int QoS);

/**********************************************************************************************************************
 * Shuts down the MQTT connection, and frees up the client instance.
 *
 * Inputs:	None
 *
 * Outputs: None
 *
 * Returns:	None
 *
 * Notes:		None
 **********************************************************************************************************************/
void shutdownMQTT(void);
