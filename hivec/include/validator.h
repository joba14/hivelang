
/**
 * @file validator.h
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author jorisb
 *
 * @date 2022-09-26
 */

#ifndef _VALIDATOR_H_
#define _VALIDATOR_H_

#include <types.h>

/**
 * @addtogroup validator
 * 
 * @{
 */

signed char Validator_validateTokens(
	struct Globals* const globals,
	struct Queue* const logs);

/**
 * @}
 */

#endif
