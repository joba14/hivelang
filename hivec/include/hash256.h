
/**
 * @file hash256.h
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author joba14
 *
 * @date 2022-09-15
 */

#ifndef _HASH256_H_
#define _HASH256_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @addtogroup hash256
 *
 * @{
 */

/**
 * 256-bit hash's stringified buffer length.
 */
#define HASH256_STRING_LENGTH ((uint64_t)64)

/**
 * 256-bit hash's blocks count.
 */
#define HASH256_BLOCKS_COUNT ((uint64_t)(HASH256_STRING_LENGTH / 8)) 

/**
 * Structure that holds 8 32-bit blocks, stringified version of the hash, and flag representing the
 * hash's status (if it was generated and is valid or not).
 */
struct Hash256
{
	uint32_t blocks[HASH256_BLOCKS_COUNT];
	char stringified[HASH256_STRING_LENGTH + 1];
	bool valid;
};

/**
 * Hash provided string value into 256-bit hash.
 */
struct Hash256 hash256(
	const char* const input,
	const uint64_t length);

/**
 * @}
 */

#endif
