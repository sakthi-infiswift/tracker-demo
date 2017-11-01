/**********************************************************************************************************************
 File: utilities.h
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

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <math.h>

#include "ErrorCode.h"

// Pi is not defined in standard C.
# define M_PI		3.14159265358979323846

#define RoundFloatToInt(a)		((int)((a) + 0.5))
#define ConvertDegreesToRadians(degrees)	((((float)(degrees)) * M_PI) / 180.0)
#define ConvertRadiansToDegrees(radians)	((((float)(radians)) * 180.0) / M_PI)

/**********************************************************************************************************************
 * This function takes an input string, and modifies it by removing leading and trailing whitespace.
 *
 * Inputs:	str 		- string to modify
 *
 * Return:	A pointer to the input string, advanced past the white space.
 *
 * Outputs: str is modified.
 *
 * Notes: From https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way.
 *
 * If the given string was allocated dynamically, the caller must not overwrite that pointer with the returned value,
 * since the original pointer must be deallocated using the same allocator with which it was allocated.  The return
 * value must NOT be deallocated using free() etc.
 *
 **********************************************************************************************************************/
char *trimWhiteSpace(char *str);

/**********************************************************************************************************************
 * This function to return substring of inputString starting at position index and of length subStringLength
 *
 * Inputs:	inputString 		- input string
 * 		  		subString 			- sub string
 * 		  		index						- starting offset into inputString
 * 		  		subStringLength - substring length
 *
 * Return:	ErrorCode_Type - an error if the input string doesn't contain the complete substring.
 *
 * Outputs: subString buffer is updated with specific length of substring
 *
 * Notes:
 *
 **********************************************************************************************************************/
ErrorCode_Type getSubString(char* inputString, char* subString, unsigned int index, unsigned int subStringLength);


/**********************************************************************************************************************
 * This call is a replacement for the "system" call. If this is a desktop build, meaning "DESKTOP" has been defined,
 * then the system message is output to the console. If not, it's written to the system as expected in an embedded
 * system. This allows us to test embedded fw on a desktop.
 *
 * Inputs: 	systemMsg - System message
 *
 * Return:	The return code from the call to system. -1 indicates an error occurred.
 *
 * Outputs:	Writes to the file system if DESKTOP is not defined. Otherwise, prints to the console.
 *
 * Notes: 	Node
 **********************************************************************************************************************/
int system_desktopSafe(char *systemMsg);

#endif		// prevent multiple includes
