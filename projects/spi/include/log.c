/**********************************************************************************************************************
  File: log.c
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

#include <stdio.h>
#include <syslog.h>

#include "log.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION IMPLEMENTATIONS
// Function descriptions are in the header file.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogMessage(int messagePriority, const char* message)
{
	#if defined(PRINT_LOG_MSG)
		printf("%s\n", message);
	#endif
	#if !defined(DESKTOP)
		openlog(NULL, LOG_PID | LOG_CONS, LOG_USER);
		syslog(messagePriority, "%s", message);
		closelog();
	#endif
}

