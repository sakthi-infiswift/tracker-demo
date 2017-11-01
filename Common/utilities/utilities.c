/***********************************************************************************************************************
 File: utilities.c
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

/* Purpose: Utility macros, functions, definitions
 */

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "ErrorCode.h"
#include "utilities.h"

char *trimWhiteSpace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

ErrorCode_Type getSubString(char* inputString, char* subString, unsigned int index,	unsigned int subStringLength)
{
	ErrorCode_Type error = ERROR_SUCCESS;
	int counter;
	unsigned int inputStringLength = strlen(inputString);

	/* Input validation
	 * range of sub-string must be in [0, strlen(inputString)]
	 */
	if ((index > inputStringLength) ||
			((index + subStringLength) > inputStringLength))
	{	// input string isn't long enough to return the specified string
		error = ERROR_STR_INDEX_LENGTH_ERROR;
	}
	else
	{	// input string contains the substring
		for (counter = 0; counter < subStringLength; counter++)
		{
			subString[counter] = inputString[index++];
		}

		subString[counter] = '\0';	// terminate the string
	}

	return error;
}

int system_desktopSafe(char *systemMsg)
{
	int status = 0;
	#if defined(DESKTOP)
		// Print any system commands
		printf("%s\n", systemMsg);
	#else
		status = system(systemMsg);
		status = WEXITSTATUS(status);
		//printf("%s, status=%d\n", systemMsg, status);	// TEST
	#endif

	return status;
}
