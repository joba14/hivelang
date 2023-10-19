
/**
 * @file lexer.c
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author joba14
 *
 * @date 2022-09-26
 */

#include <lexer.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/**
 * @addtogroup lexer
 * 
 * @{
 */

struct LexerContext
{
	struct Location location;
	const char* begin;
	const char* current;
	const char* end;
	signed long long count;
};

static signed char Lexer_validateTokens(
	const char* filePath,
	struct List* const tokens,
	struct Queue* const logs);

static void Lexer_lexLine(
	struct LexerContext* const context,
	struct List* const tokens);

static struct Token* Lexer_createToken(
	void);

static void Lexer_setupToken(
	struct Token* const token,
	const signed long long id,
	const signed long long kind,
	const struct Location location);

static void Lexer_moveBy(
	struct LexerContext* const context,
	const signed long long amount);

static signed char Lexer_tryParseKeyword(
	struct LexerContext* const context,
	struct List* const tokens);

static signed char Lexer_tryParseIntrinsic(
	struct LexerContext* const context,
	struct List* const tokens);

static signed char Lexer_tryParseIdentifier(
	struct LexerContext* const context,
	struct List* const tokens);

static signed char Lexer_tryParseLiteral(
	struct LexerContext* const context,
	struct List* const tokens);

static signed char Lexer_isWhitespace(
	const char ch);

static void Lexer_skipWhitespace(
	struct LexerContext* const context);

signed char Lexer_lexFile(
	const char* filePath,
	struct List* const tokens,
	struct Queue* const logs)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The filePath, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(filePath != NULL);

	// NOTE: not marking as debug-only.
	// REASONS:
	//     1. Provided file path can theoretically be empty string, that, while
	//        is possible behaviour, will fail at file reading. Therefore, this
	//        check is necessary, to prevent compiler errors.
	if (strlen(filePath) <= 0)
	{
		Queue_enqueue(logs, Log_create("lexer", SEVERITY_ERROR, INVALID_LOCATION, "provided source file path was empty!"));

#if HIVEC_DEBUG
		Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
			(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
			"locator of the log above this meesage."));
#endif

		return 0;
	}

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The tokens, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(tokens != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The logs, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(logs != NULL);

	// Creating a lexer context
	struct LexerContext context = {0};
	// Setting up location
	context.location = (struct Location) {.file = filePath, .line = 1, .column = 1 };
	// Opening a provided file
	FILE* file = fopen(filePath, "r");

	// NOTE: not marking as debug-only.
	// REASONS:
	//     1. Opening file might actually fail (due to non-existing file path)
	//        and must be checked and reported accordingly.
	if (file == NULL)
	{
		Queue_enqueue(logs, Log_create("lexer", SEVERITY_ERROR, INVALID_LOCATION, "failed to open source file with path `%s`!", filePath));

#if HIVEC_DEBUG
		Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
			(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
			"locator of the log above this meesage."));
#endif

		return 0;
	}

	char* line = NULL;
	signed long long length = 0;
	signed long long read;

	while ((read = getline(&line, (long unsigned int*)&length, file)) != -1)
	{
		// Setting up line's pointers
		context.begin = line;
		context.current = line;
		context.end = line + read;
		// Lexing the file
		Lexer_lexLine(&context, tokens);
		// Updating location for the next line
		++context.location.line;
		context.location.column = 1;
	}

	if (line != NULL)
	{
		free(line);
	}

	fclose(file);
	return Lexer_validateTokens(filePath, tokens, logs);
}

signed char Lexer_validateTokens(
	const char* filePath,
	struct List* const tokens,
	struct Queue* const logs)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The filePath, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(filePath != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The tokens, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(tokens != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The logs, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(logs != NULL);

	// NOTES:
	//     1. Since tokens count is validated here, it is not required to be logged in any
	//        following subprocess. However, I advice to place asserts in all those places
	//        to catch any possible developer-inflicted bugs.
	//     2. In case the tokens list is empty (the source was empty of compilable code),
	//        all the following processes must be skipped and all resources deallocated.
	if (tokens->count <= 0)
	{
		Queue_enqueue(logs, Log_create("lexer", SEVERITY_ERROR, INVALID_LOCATION, "file `%s` was empty of compilable source which is forbidden!", filePath));

#if HIVEC_DEBUG
		Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
			(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
			"locator of the log above this meesage."));
#endif

		return 0;
	}

	signed char result = 1;

	for (struct LNode* tokensIterator = tokens->front; tokensIterator != NULL; tokensIterator = tokensIterator->next)
	{
		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The data of the tokens iterator node, in the list must never be
		//        of value null.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(tokensIterator->data != NULL);

		struct Token* token = (struct Token*)tokensIterator->data;

		if (token->kind == TOKEN_INVALID)
		{
			Queue_enqueue(logs,
				Log_create("lexer", SEVERITY_ERROR, token->location,
					"encountered an invalid token `%.*s`!",
					(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
			Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
				(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
				"locator of the log above this meesage."));
#endif

			result = 0;
		}
	}

	return result;
}

static void Lexer_lexLine(
	struct LexerContext* const context,
	struct List* const tokens)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The context, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(context != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The tokens, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(tokens != NULL);

	// Handlng empty line
	if (context->begin == context->end || context->begin == NULL)
	{
		return;
	}

	for (context->current = context->begin; context->current < context->end;)
	{
		// Skip white space
		Lexer_skipWhitespace(context);

		// Check if line was only white spaces and return if so
		if (context->current == context->end)
		{
			return;
		}

		// Skip comment
		if (context->current < context->end && *(context->current) == '/' && *(context->current + 1) == '/')
		{
			return;
		}

		// Trying to parse keyword
		if (Lexer_tryParseKeyword(context, tokens) != 0)
		{
			continue;
		}
		// Trying to parse intrinsic
		else if (Lexer_tryParseIntrinsic(context, tokens) != 0)
		{
			continue;
		}
		// Trying to parse identifier
		else if (Lexer_tryParseIdentifier(context, tokens) != 0)
		{
			continue;
		}
		// Trying to parse literal
		else if (Lexer_tryParseLiteral(context, tokens) != 0)
		{
			continue;
		}
		else
		{
			// Pushing invalid token, since all parsers failed
			struct Token* token = Lexer_createToken();
			Lexer_setupToken(token, context->count++, TOKEN_INVALID, context->location);

			const char* source = context->current;
			signed long long sourceLength = 0;
			for (; !Lexer_isWhitespace(*context->current); ++sourceLength) { Lexer_moveBy(context, 1); }

			token->source.buffer = (char*)malloc((sourceLength) * sizeof(char));

			// NOTE: using `assert` and not `if`
			// REASONS:
			//     1. The memory allocation errors can happen anytime, no matter build
			//        configuration being debug or release. However, since the compiler
			//        cannot prevent such bugs, I will leave it as assert. Worst case
			//        scenario - the compiler crashes, and user re-runs it.
			//     2. This assert will prevent developers infliced bugs and development
			//        and debug configuration.
			assert(token->source.buffer != NULL);

			memcpy(token->source.buffer, source, sourceLength);
			token->source.length = sourceLength;
			List_push(tokens, (void* const)token);
		}
	}
}

static struct Token* Lexer_createToken(
	void)
{
	struct Token* token = (struct Token*)malloc(sizeof(struct Token));

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The memory allocation errors can happen anytime, no matter build
	//        configuration being debug or release. However, since the compiler
	//        cannot prevent such bugs, I will leave it as assert. Worst case
	//        scenario - the compiler crashes, and user re-runs it.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(token != NULL);

	return token;
}

static void Lexer_setupToken(
	struct Token* const token,
	const signed long long id,
	const signed long long kind,
	const struct Location location)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The token, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(token != NULL);

	token->id = id;
	token->kind = kind;
	token->location = location;
	token->nextRef = NULL;
	token->previousRef = NULL;

	{
		char buffer[128 + 1];
		signed long long length = snprintf(buffer, 128, "%p_%lld", (void*)token, id);
		assert(length >= 0);
		buffer[length] = 0;
		token->hash = hash256(buffer, length);
	}
}

static void Lexer_moveBy(
	struct LexerContext* const context,
	const signed long long amount)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The context, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(context != NULL);

	for (signed long long i = 0; i < amount && context->current < context->end; ++i)
	{
		++context->current;
		++context->location.column;
	}
}

static signed char Lexer_tryParseKeyword(
	struct LexerContext* const context,
	struct List* const tokens)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The context, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(context != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The tokens, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(tokens != NULL);

	// NOTE: this `keywordsCount` define must be changed when modifying the `keywords` set!
	#define keywordsCount ((signed long long)11)
	const char* keywords[] =
	{
		[TOKEN_KEYWORD_MAIN] = "main",
		[TOKEN_KEYWORD_I64] = "i64",
		[TOKEN_KEYWORD_P64] = "p64",
		[TOKEN_KEYWORD_IF] = "if",
		[TOKEN_KEYWORD_ELSE] = "else",
		[TOKEN_KEYWORD_WHILE] = "while",
		[TOKEN_KEYWORD_PROCEDURE] = "procedure",
		[TOKEN_KEYWORD_REQUIRE] = "require",
		[TOKEN_KEYWORD_RETURN] = "return",
		[TOKEN_KEYWORD_DO] = "do",
		[TOKEN_KEYWORD_END] = "end"
	};
	static_assert((TOKEN_LAST_KEYWORD - TOKEN_FIRST_KEYWORD + 1) == keywordsCount,
		"The local `keywords` set is not synced with updated types enum!");

	signed long long wordLength = 0;
	for (const char* temp = context->current; !Lexer_isWhitespace(*temp) && temp != context->end; ++temp)
		++wordLength;

	for (signed long long type = TOKEN_FIRST_KEYWORD; type <= TOKEN_LAST_KEYWORD; ++type)
	{
		const char* keyword = keywords[type];
		const signed long long keywordLength = strlen(keyword);

		if ((wordLength == keywordLength) && (strncmp(context->current, keyword, keywordLength) == 0))
		{
			struct Token* token = Lexer_createToken();
			Lexer_setupToken(token, context->count++, type, context->location);

			token->source.buffer = (char*)malloc((wordLength) * sizeof(char));

			// NOTE: using `assert` and not `if`
			// REASONS:
			//     1. The memory allocation errors can happen anytime, no matter build
			//        configuration being debug or release. However, since the compiler
			//        cannot prevent such bugs, I will leave it as assert. Worst case
			//        scenario - the compiler crashes, and user re-runs it.
			//     2. This assert will prevent developers infliced bugs and development
			//        and debug configuration.
			assert(token->source.buffer != NULL);

			// Copying identifier name over to the token
			memcpy(token->source.buffer, context->current, wordLength);
			token->source.length = wordLength;

			List_push(tokens, (void* const)token);
			Lexer_moveBy(context, wordLength);
			return 1;
		}
	}

	return 0;
}

static signed char Lexer_tryParseIntrinsic(
	struct LexerContext* const context,
	struct List* const tokens)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The context, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(context != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The tokens, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(tokens != NULL);

	// NOTE: this `intrinsicsCount` define must be changed when modifying the `intrinsics` set!
#if HIVEC_DEBUG
// TODO: remove:
	#define intrinsicsCount ((signed long long)26)
#else
	#define intrinsicsCount ((signed long long)25)
#endif
	static const char* intrinsics[] =
	{
		[TOKEN_INTRINSIC_ADD] = "add",
		[TOKEN_INTRINSIC_SUBTRACT] = "subtract",
		[TOKEN_INTRINSIC_MULTIPLY] = "multiply",
		[TOKEN_INTRINSIC_DIVIDE] = "divide",
		[TOKEN_INTRINSIC_MODULUS] = "modulus",
		[TOKEN_INTRINSIC_EQUAL] = "equal",
		[TOKEN_INTRINSIC_NEQUAL] = "nequal",
		[TOKEN_INTRINSIC_GREATER] = "greater",
		[TOKEN_INTRINSIC_LESS] = "less",
		[TOKEN_INTRINSIC_BAND] = "band",
		[TOKEN_INTRINSIC_BOR] = "bor",
		[TOKEN_INTRINSIC_BNOT] = "bnot",
		[TOKEN_INTRINSIC_SHIFTL] = "shiftl",
		[TOKEN_INTRINSIC_SHIFTR] = "shiftr",
		[TOKEN_INTRINSIC_SYSCALL0] = "syscall0",
		[TOKEN_INTRINSIC_SYSCALL1] = "syscall1",
		[TOKEN_INTRINSIC_SYSCALL2] = "syscall2",
		[TOKEN_INTRINSIC_SYSCALL3] = "syscall3",
		[TOKEN_INTRINSIC_SYSCALL4] = "syscall4",
		[TOKEN_INTRINSIC_SYSCALL5] = "syscall5",
		[TOKEN_INTRINSIC_SYSCALL6] = "syscall6",
		[TOKEN_INTRINSIC_CLONE] = "clone",
		[TOKEN_INTRINSIC_DROP] = "drop",
		[TOKEN_INTRINSIC_OVER] = "over",
#if HIVEC_DEBUG
// TODO: remove:
		[TOKEN_INTRINSIC_PRINTN] = "printn",
#endif
		[TOKEN_INTRINSIC_SWAP] = "swap"
	};
	static_assert((TOKEN_LAST_INTRINSIC- TOKEN_FIRST_INTRINSIC + 1) == intrinsicsCount,
		"The local `intrinsics` set is not synced with updated types enum!");

	signed long long wordLength = 0;
	for (const char* temp = context->current; !Lexer_isWhitespace(*temp) && temp != context->end; ++temp)
		++wordLength;

	for (signed long long type = TOKEN_FIRST_INTRINSIC; type <= TOKEN_LAST_INTRINSIC; ++type)
	{
		const char* intrinsic = intrinsics[type];
		const signed long long intrensicLength = strlen(intrinsic);

		if ((wordLength == intrensicLength) && (strncmp(context->current, intrinsic, intrensicLength) == 0))
		{
			struct Token* token = Lexer_createToken();
			Lexer_setupToken(token, context->count++, type, context->location);

			token->source.buffer = (char*)malloc((wordLength) * sizeof(char));

			// NOTE: using `assert` and not `if`
			// REASONS:
			//     1. The memory allocation errors can happen anytime, no matter build
			//        configuration being debug or release. However, since the compiler
			//        cannot prevent such bugs, I will leave it as assert. Worst case
			//        scenario - the compiler crashes, and user re-runs it.
			//     2. This assert will prevent developers infliced bugs and development
			//        and debug configuration.
			assert(token->source.buffer != NULL);

			// Copying identifier name over to the token
			memcpy(token->source.buffer, context->current, wordLength);
			token->source.length = wordLength;

			List_push(tokens, token);
			Lexer_moveBy(context, wordLength);
			return 1;
		}
	}

	return 0;
}

static signed char Lexer_isIdentifierChar(
	const char ch)
{
	return ch == '_'
		|| ch == '?'
		|| isalpha(ch)
		|| isdigit(ch);
}

static signed char Lexer_tryParseIdentifier(
	struct LexerContext* const context,
	struct List* const tokens)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The context, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(context != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The tokens, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(tokens != NULL);

	// Checking for invalid first symbol
	if (!(Lexer_isIdentifierChar(*context->current) && !isdigit(*context->current)))
	{
		return 0;
	}

	signed long long wordLength = 0;
	for (const char* temp = context->current; !Lexer_isWhitespace(*temp) && Lexer_isIdentifierChar(*temp) && temp != context->end; ++temp)
		++wordLength;

	// Handle not an identifier word error
	if (wordLength <= 0)
	{
		return 0;
	}

	struct Token* token = Lexer_createToken();
	Lexer_setupToken(token, context->count++, TOKEN_IDENTIFIER, context->location);

	token->source.buffer = (char*)malloc((wordLength) * sizeof(char));

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The memory allocation errors can happen anytime, no matter build
	//        configuration being debug or release. However, since the compiler
	//        cannot prevent such bugs, I will leave it as assert. Worst case
	//        scenario - the compiler crashes, and user re-runs it.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(token->source.buffer != NULL);

	// Copying identifier name over to the token
	memcpy(token->source.buffer, context->current, wordLength);
	token->source.length = wordLength;

	List_push(tokens, token);
	Lexer_moveBy(context, wordLength);
	return 1;
}

static signed char Lexer_tryParseLiteral(
	struct LexerContext* const context,
	struct List* const tokens)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The context, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(context != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The tokens, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(tokens != NULL);

	// Try parse string literal
	if (*context->current == '\"')
	{
		const char* literal = context->current + 1;
		signed long long literalLength = 0;

		for (const char* iterator = context->current + 1; 1; ++iterator, ++literalLength)
		{
			if (*(iterator) == '\"' && (iterator - 1 >= context->begin) && *(iterator - 1) != '\"') break;
			if (*(iterator) == '\"' && (iterator - 2 >= context->begin) && *(iterator - 1) == '\"' && *(iterator - 2) == '\"') break;
		}

		signed long long bytesLength = 0;

		for (signed long long index = 0; index < literalLength; ++index, ++bytesLength)
		{
			if (literal[index] == '\\' && index + 1 < literalLength)
			{
				switch (literal[index + 1])
				{
					case 'a':
					{
						bytesLength -= 1;
					} break;

					case 'b':
					{
						bytesLength -= 1;
					} break;

					case 'f':
					{
						bytesLength -= 1;
					} break;

					case 'n':
					{
						bytesLength -= 1;
					} break;

					case 'r':
					{
						bytesLength -= 1;
					} break;

					case 't':
					{
						bytesLength -= 1;
					} break;

					case 'v':
					{
						bytesLength -= 1;
					} break;

					case '\\':
					{
						bytesLength -= 1;
					} break;

					case '\'':
					{
						bytesLength -= 0;
					} break;

					case '\"':
					{
						bytesLength -= 1;
					} break;

					case '\?':
					{
						bytesLength -= 1;
					} break;

					default:
					{
					} break;
				}
			}
		}

		unsigned char* bytes = (unsigned char*)malloc((bytesLength) * sizeof(unsigned char));

		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The memory allocation errors can happen anytime, no matter build
		//        configuration being debug or release. However, since the compiler
		//        cannot prevent such bugs, I will leave it as assert. Worst case
		//        scenario - the compiler crashes, and user re-runs it.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(bytes != NULL);

		for (signed long long index = 0, iter = 0; index < literalLength;)
		{
			if (literal[index] == '\\' && index + 2 <= literalLength)
			{
				switch (literal[index + 1])
				{
					case 'a':
					{
						memset(bytes + iter, '\a', 1);
						++iter;
						index += 2;
					} break;

					case 'b':
					{
						memset(bytes + iter, '\b', 1);
						++iter;
						index += 2;
					} break;

					case 'f':
					{
						memset(bytes + iter, '\f', 1);
						++iter;
						index += 2;
					} break;

					case 'n':
					{
						memset(bytes + iter, '\n', 1);
						++iter;
						index += 2;
					} break;

					case 'r':
					{
						memset(bytes + iter, '\r', 1);
						++iter;
						index += 2;
					} break;

					case 't':
					{
						memset(bytes + iter, '\t', 1);
						++iter;
						index += 2;
					} break;

					case 'v':
					{
						memset(bytes + iter, '\v', 1);
						++iter;
						index += 2;
					} break;

					case '\\':
					{
						memset(bytes + iter, '\\', 1);
						++iter;
						index += 2;
					} break;

					case '\'':
					{
						memset(bytes + iter, '\'', 1);
						++iter;
						index += 2;
					} break;

					case '\"':
					{
						memset(bytes + iter, '\"', 1);
						++iter;
						index += 2;
					} break;

					case '\?':
					{
						memset(bytes + iter, '\?', 1);
						++iter;
						index += 2;
					} break;

					default:
					{
						index += 1;
					} break;
				}
			}
			else
			{
				bytes[iter++] = literal[index];
				index += 1;
			}
		}

		struct Token* token = Lexer_createToken();
		Lexer_setupToken(token, context->count++, TOKEN_LITERAL_STRING, context->location);

		token->value.string.bytes = bytes;
		token->value.string.length = bytesLength;

		token->source.buffer = (char*)malloc((literalLength) * sizeof(char));

		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The memory allocation errors can happen anytime, no matter build
		//        configuration being debug or release. However, since the compiler
		//        cannot prevent such bugs, I will leave it as assert. Worst case
		//        scenario - the compiler crashes, and user re-runs it.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(token->source.buffer != NULL);

		// Copying identifier name over to the token
		memcpy(token->source.buffer, literal, literalLength);
		token->source.length = literalLength;

		List_push(tokens, token);
		Lexer_moveBy(context, literalLength + 2);
		return 1;
	}
	// Try parse numerical literal
	else
	{
		const char prefix = *context->current;
		if (!(isdigit(prefix) || prefix == '-' || prefix == '+'))
			return 0;

		const char* literal = context->current;
		signed long long literalLength = 1;
		for (const char* iterator = context->current + 1; isdigit(*iterator) && iterator < context->end; ++iterator, ++literalLength);

		if (literalLength <= 0)
		{
			return 0;
		}

		signed long long value = 0;

		if (sscanf(literal, "%lld", &value) <= 0)
		{
			return 0;
		}

		struct Token* token = Lexer_createToken();
		Lexer_setupToken(token, context->count++, TOKEN_LITERAL_I64, context->location);

		token->value.i64 = value;

		token->source.buffer = (char*)malloc((literalLength) * sizeof(char));

		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The memory allocation errors can happen anytime, no matter build
		//        configuration being debug or release. However, since the compiler
		//        cannot prevent such bugs, I will leave it as assert. Worst case
		//        scenario - the compiler crashes, and user re-runs it.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(token->source.buffer != NULL);

		// Copying identifier name over to the token
		memcpy(token->source.buffer, literal, literalLength);
		token->source.length = literalLength;

		List_push(tokens, token);
		Lexer_moveBy(context, literalLength);
		return 1;
	}

	return 0;
}

static signed char Lexer_isWhitespace(
	const char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' ? 1 : 0;
}

static void Lexer_skipWhitespace(
	struct LexerContext* const context)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The context, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(context != NULL);

	while (Lexer_isWhitespace(*(context->current)))
	{
		Lexer_moveBy(context, 1);
	}
}

/**
 * @}
 */
