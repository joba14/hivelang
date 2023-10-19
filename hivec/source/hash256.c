
/**
 * @file hash256.c
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author joba14
 *
 * @date 2022-09-15
 */

#include <hash256.h>

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

/**
 * @addtogroup hash256
 * 
 * @{
 */

/**
 * Macro for creating 'invalid' hashes (not processed ones).
 */
#define INVALID_HASH256 ((struct Hash256) {.blocks = {0}, .stringified = {0}, .valid = 0})

/**
 * Primary value used for hashing no.1.
 */
#define PRIMARY1 ((const unsigned int)31)

/**
 * Primary value used for hashing no.2.
 */
#define PRIMARY2 ((const signed long long)2747636419)

/**
 * Primary value used for hashin no.3.
 */
#define PRIMARY3 ((const signed long long)2654435769)

// Note: these primary values were taken from the internet. I searched what constants are usually
// used in hashing algorithms, and learnt about prime numbers. First one I came up by myself, and
// last to I 'borrowed' from a forum. And they seem to be working with my algorithm just fine.

/**
 * [PRIVATE] Setup a block of the hash object using the state, made from the input, and bits
 * manipulation.
 */
static inline void setBlock(
	const signed int index,
	const signed int state,
	const unsigned int previous,
	struct Hash256* const hash)
{
	assert(hash != NULL);
	hash->blocks[index] += (state >> index) ^ previous;
	hash->blocks[index] ^= previous + index;
	hash->blocks[index] ^= PRIMARY2;
	hash->blocks[index] *= PRIMARY3 + index;
	hash->blocks[index] ^= hash->blocks[index] >> 16;
	hash->blocks[index] *= PRIMARY3;
	hash->blocks[index] ^= hash->blocks[index] >> 16;
	hash->blocks[index] *= PRIMARY3;
}

struct Hash256 hash256(
	const char* input,
	signed long long length)
{
	// Input validation and error handling
	if (input == NULL)
	{
		return INVALID_HASH256;
	}

	if (length < 0)
	{
		return INVALID_HASH256;
	}

	// Setting up hash blocks
	struct Hash256 hash = INVALID_HASH256;
	unsigned int state = 0;

	for (signed int i = 0; i < HASH256_BLOCKS_COUNT; ++i)
		setBlock(i, state, i == 0 ? state : hash.blocks[i - 1], &hash);

	state = 0;

	for (signed int i = 0; i < length; ++i)
	{
		const unsigned int value = *(unsigned int*)(input + i);
		state = PRIMARY1 * state - PRIMARY2 * state + (state ^ PRIMARY3) + value;

		for (signed int j = 0; j < HASH256_BLOCKS_COUNT; ++j)
			setBlock(j, state, j == 0 ? state : hash.blocks[j - 1], &hash);
	}

	// Setting up stringified hash
	for (signed int i = 0; i < HASH256_BLOCKS_COUNT; ++i)
	{
		if (snprintf(hash.stringified + (i * HASH256_BLOCKS_COUNT), HASH256_BLOCKS_COUNT + 1, "%08x", hash.blocks[i]) <= 0)
		{
			return INVALID_HASH256;
		}
	}

	hash.stringified[HASH256_STRING_LENGTH] = 0;
	hash.valid = 1;
	return hash;
}

/**
 * @}
 */
