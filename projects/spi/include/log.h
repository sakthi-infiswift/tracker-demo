/***********************************************************************************************************************
  File: log.h
  Copyright © 2016  Infiswift Inc. All rights reserved.

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

/* Purpose:This file contains the code to log error and Information.Building the log messages and send it to queue.
 */

#ifndef LOG_H_
#define LOG_H_

#include <syslog.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC DEFINITIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_MESSAGE_LENGTH	1024

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES AND DESCRIPTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**********************************************************************************************************************
* This function logs a message to the syslog system.
* If PRINT_LOG_MSG is 1, it also writes the message to the console.
*
* Inputs: priority	- The message priority (emergency, alert, crticial, etc.)
* 										See syslog.h for a definition of the log priorities.
* 		  	message		- The message to log.
*
*	Return: none
* Outputs: Logged message.
*
* Notes:
*
**********************************************************************************************************************/
void LogMessage(int priority, const char* message);

#endif /* LOG_H_ */
