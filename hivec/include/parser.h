
/**
 * @file parser.h
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author joba14
 *
 * @date 2022-10-04
 */

#ifndef _PARSER_H_
#define _PARSER_H_

#include <types.h>

/**
 * @addtogroup parser
 * 
 * @{
 */

signed char Parser_parseTokens(
	struct Globals* const globals,
	const struct List* const tokens,
	struct Queue* const logs);

/**
 * @}
 */

#endif
