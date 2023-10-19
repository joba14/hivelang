
/**
 * @file translator.h
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author joba14
 *
 * @date 2022-09-26
 */

#ifndef _TRANSLATOR_H_
#define _TRANSLATOR_H_

#include <types.h>

/**
 * @addtogroup translator
 * 
 * @{
 */

signed char Translator_translateTokens(
	const char* outputPath,
	struct Globals* const globals,
	struct Queue* const logs);

/**
 * @}
 */

#endif
