
/**
 * @file lexer.h
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author jorisb
 *
 * @date 2022-09-10
 */

#ifndef _LEXER_H_
#define _LEXER_H_

#include <types.h>

/**
 * @addtogroup lexer
 * 
 * @{
 */

signed char Lexer_lexFile(
	const char* filePath,
	struct List* const tokens,
	struct Queue* const logs);

/**
 * @}
 */

#endif
