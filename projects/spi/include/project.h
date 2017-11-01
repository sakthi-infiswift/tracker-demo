/*************************************************************************************************
 File: project.h
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
 *********************************************************************************************/

/* Purpose: Project-specific settings
 */

// Fw version. Change this if we send the fw anywhere.
#define FW_VERSION	"0.17"

/* GPIO for the Stow pushbutton
 * This pin is configured as an input with an internal pull-up.
 */
#define GPIO_STOW_PIN		66

// Define the BBGW I/O capabilities used by this project.
#define BBGW_USE_GPIO			1
#define BBGW_USE_A_TO_D		1
#define BBGW_USE_SERIAL		0
#define BBGW_USE_PWM			1

