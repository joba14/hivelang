
/**
 * @file validator.c
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author jorisb
 *
 * @date 2022-09-26
 */

#include <validator.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * @addtogroup validator
 * 
 * @{
 */

signed char Validator_validateTokens(
	struct Globals* const globals,
	struct Queue* const logs)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The globals, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(globals != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The logs, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(logs != NULL);

	// NOTE:
	//     This case is being checked in lexer and must not be logged here. It should never
	//     fail here too. This check is helpful for debugging purposes in development
	//     process.
	assert(globals->procedures.count > 0);

	enum { VALUE_TYPE_I64 = 0, VALUE_TYPE_P64 };
	static signed long long I64_VALUE_TYPE = (signed long long)VALUE_TYPE_I64;
	static signed long long P64_VALUE_TYPE = (signed long long)VALUE_TYPE_P64;

	for (struct LNode* proceduresIterator = globals->procedures.front; proceduresIterator != NULL; proceduresIterator = proceduresIterator->next)
	{
		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The procedures iterator's data, in the list must never be of value
		//        null.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(proceduresIterator->data != NULL);

		struct Procedure* procedure = (struct Procedure*)proceduresIterator->data;
		struct Stack stack = Stack_create();

		for (struct LNode* requiredTypesIterator = procedure->requiredTypes.front; requiredTypesIterator != NULL; requiredTypesIterator = requiredTypesIterator->next)
		{
			// NOTE: using `assert` and not `if`
			// REASONS:
			//     1. The required types iterator's data, in the list must never be of
			//        value null.
			//     2. This assert will prevent developers infliced bugs and development
			//        and debug configuration.
			assert(requiredTypesIterator->data != NULL);

			struct Token* token = (struct Token*)requiredTypesIterator->data;

			// NOTE: using `assert` and not `if`
			// REASONS:
			//     1. All required types tokens must be of correct type. I am leaving the
			//        asserts for developers-inflicted erors and bugs in parser.
			//     2. This assert will prevent developers infliced bugs and development
			//        and debug configuration.
			assert(token->kind == TOKEN_KEYWORD_I64 || token->kind == TOKEN_KEYWORD_P64);

			if (token->kind == TOKEN_KEYWORD_I64)
			{
				Stack_push(&stack, &I64_VALUE_TYPE);
			}
			else if (token->kind == TOKEN_KEYWORD_P64)
			{
				Stack_push(&stack, &P64_VALUE_TYPE);
			}
		}

		for (struct LNode* bodyIterator = procedure->body.front; bodyIterator != NULL; bodyIterator = bodyIterator->next)
		{
			// NOTE: using `assert` and not `if`
			// REASONS:
			//     1. The procedures iterator's data, in the list must never be of value
			//        null.
			//     2. This assert will prevent developers infliced bugs and development
			//        and debug configuration.
			assert(bodyIterator->data != NULL);

			struct Token* token = (struct Token*)bodyIterator->data;

			switch (token->kind)
			{
				case TOKEN_KEYWORD_DO:
				{
					if (token->previousRef != NULL && token->previousRef->kind == TOKEN_KEYWORD_PROCEDURE)
					{
						break;
					}

					if (stack.count <= 0)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` keyword requires 1 argument to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long a = *(signed long long*)Stack_pop(&stack);

					if (a != I64_VALUE_TYPE)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` keyword only supports (i64) sets of arguments!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}
				} break;

				case TOKEN_INTRINSIC_ADD:
				{
					if (stack.count <= 1)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 2 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long b = *(signed long long*)Stack_pop(&stack);
					signed long long a = *(signed long long*)Stack_pop(&stack);

					if (a == I64_VALUE_TYPE && a == b)
					{
						Stack_push(&stack, &I64_VALUE_TYPE);
					}
					else if ((a == I64_VALUE_TYPE && b == P64_VALUE_TYPE) || (a == P64_VALUE_TYPE && b == I64_VALUE_TYPE))
					{
						Stack_push(&stack, &P64_VALUE_TYPE);
					}
					else
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic only supports (i64 i64), (i64 p64), or (p64 i64) sets of arguments!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}
				} break;

				case TOKEN_INTRINSIC_SUBTRACT:
				{
					if (stack.count <= 1)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 2 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long b = *(signed long long*)Stack_pop(&stack);
					signed long long a = *(signed long long*)Stack_pop(&stack);

					if (a == I64_VALUE_TYPE && a == b)
					{
						Stack_push(&stack, &I64_VALUE_TYPE);
					}
					else if (a == P64_VALUE_TYPE && b == I64_VALUE_TYPE)
					{
						Stack_push(&stack, &P64_VALUE_TYPE);
					}
					else
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic only supports (i64 i64) or (p64 i64) sets of arguments!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}
				} break;

				case TOKEN_INTRINSIC_MULTIPLY:
				{
					if (stack.count <= 1)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 2 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long b = *(signed long long*)Stack_pop(&stack);
					signed long long a = *(signed long long*)Stack_pop(&stack);

					if (a == I64_VALUE_TYPE && a == b)
					{
						Stack_push(&stack, &I64_VALUE_TYPE);
					}
					else
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic only supports (i64 i64) set of arguments!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}
				} break;

				case TOKEN_INTRINSIC_DIVIDE:
				{
					if (stack.count <= 1)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 2 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long b = *(signed long long*)Stack_pop(&stack);
					signed long long a = *(signed long long*)Stack_pop(&stack);

					if (a == I64_VALUE_TYPE && a == b)
					{
						Stack_push(&stack, &I64_VALUE_TYPE);
					}
					else
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic only supports (i64 i64) set of arguments!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}
				} break;

				case TOKEN_INTRINSIC_MODULUS:
				{
					if (stack.count <= 1)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 2 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long b = *(signed long long*)Stack_pop(&stack);
					signed long long a = *(signed long long*)Stack_pop(&stack);

					if (a == I64_VALUE_TYPE && a == b)
					{
						Stack_push(&stack, &I64_VALUE_TYPE);
					}
					else
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic only supports (i64 i64) set of arguments!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}
				} break;

				case TOKEN_INTRINSIC_EQUAL:
				{
					if (stack.count <= 1)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 2 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long b = *(signed long long*)Stack_pop(&stack);
					signed long long a = *(signed long long*)Stack_pop(&stack);

					if (a == b)
					{
						Stack_push(&stack, &a);
					}
					else
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic only supports (i64 i64), (p64 p64), or (a64 a64) sets of arguments!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}
				} break;

				case TOKEN_INTRINSIC_NEQUAL:
				{
					if (stack.count <= 1)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 2 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long b = *(signed long long*)Stack_pop(&stack);
					signed long long a = *(signed long long*)Stack_pop(&stack);

					if (a == b)
					{
						Stack_push(&stack, &a);
					}
					else
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic only supports (i64 i64), (p64 p64), or (a64 a64) sets of arguments!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}
				} break;

				case TOKEN_INTRINSIC_GREATER:
				{
					if (stack.count <= 1)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 2 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long b = *(signed long long*)Stack_pop(&stack);
					signed long long a = *(signed long long*)Stack_pop(&stack);

					if (a == b)
					{
						Stack_push(&stack, &a);
					}
					else
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic only supports (i64 i64), (p64 p64), or (a64 a64) sets of arguments!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}
				} break;

				case TOKEN_INTRINSIC_LESS:
				{
					if (stack.count <= 1)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 2 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long b = *(signed long long*)Stack_pop(&stack);
					signed long long a = *(signed long long*)Stack_pop(&stack);

					if (a == b)
					{
						Stack_push(&stack, &a);
					}
					else
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic only supports (i64 i64), (p64 p64), or (a64 a64) sets of arguments!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}
				} break;

				case TOKEN_INTRINSIC_BAND:
				{
					if (stack.count <= 1)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 2 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long b = *(signed long long*)Stack_pop(&stack);
					signed long long a = *(signed long long*)Stack_pop(&stack);

					if (a == I64_VALUE_TYPE && a == b)
					{
						Stack_push(&stack, &I64_VALUE_TYPE);
					}
					else
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic only supports (i64 i64) set of arguments!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}
				} break;

				case TOKEN_INTRINSIC_BOR:
				{
					if (stack.count <= 1)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 2 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long b = *(signed long long*)Stack_pop(&stack);
					signed long long a = *(signed long long*)Stack_pop(&stack);

					if (a == I64_VALUE_TYPE && a == b)
					{
						Stack_push(&stack, &I64_VALUE_TYPE);
					}
					else
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic only supports (i64 i64) set of arguments!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}
				} break;

				case TOKEN_INTRINSIC_BNOT:
				{
					if (stack.count <= 0)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 1 argument to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long a = *(signed long long*)Stack_pop(&stack);

					if (a == I64_VALUE_TYPE)
					{
						Stack_push(&stack, &I64_VALUE_TYPE);
					}
					else
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic only supports (i64) set of arguments!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}
				} break;

				case TOKEN_INTRINSIC_SHIFTL:
				{
					if (stack.count <= 1)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 2 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long b = *(signed long long*)Stack_pop(&stack);
					signed long long a = *(signed long long*)Stack_pop(&stack);

					if (a == I64_VALUE_TYPE && a == b)
					{
						Stack_push(&stack, &I64_VALUE_TYPE);
					}
					else
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic only supports (i64 i64) set of arguments!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}
				} break;

				case TOKEN_INTRINSIC_SHIFTR:
				{
					if (stack.count <= 1)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 2 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long b = *(signed long long*)Stack_pop(&stack);
					signed long long a = *(signed long long*)Stack_pop(&stack);

					if (a == I64_VALUE_TYPE && a == b)
					{
						Stack_push(&stack, &I64_VALUE_TYPE);
					}
					else
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic only supports (i64 i64) set of arguments!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}
				} break;

				case TOKEN_INTRINSIC_SYSCALL0:
				{
					if (stack.count <= 0)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 1 argument to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					Stack_pop(&stack);
					Stack_push(&stack, &I64_VALUE_TYPE);
				} break;

				case TOKEN_INTRINSIC_SYSCALL1:
				{
					if (stack.count <= 1)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 2 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					for (signed int i = 0; i < 2; ++i)
					{
						Stack_pop(&stack);
					}

					Stack_push(&stack, &I64_VALUE_TYPE);
				} break;

				case TOKEN_INTRINSIC_SYSCALL2:
				{
					if (stack.count <= 2)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 3 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					for (signed int i = 0; i < 3; ++i)
					{
						Stack_pop(&stack);
					}

					Stack_push(&stack, &I64_VALUE_TYPE);
				} break;

				case TOKEN_INTRINSIC_SYSCALL3:
				{
					if (stack.count <= 3)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 4 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					for (signed int i = 0; i < 4; ++i)
					{
						Stack_pop(&stack);
					}

					Stack_push(&stack, &I64_VALUE_TYPE);
				} break;

				case TOKEN_INTRINSIC_SYSCALL4:
				{
					if (stack.count <= 4)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 5 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					for (signed int i = 0; i < 5; ++i)
					{
						Stack_pop(&stack);
					}

					Stack_push(&stack, &I64_VALUE_TYPE);
				} break;

				case TOKEN_INTRINSIC_SYSCALL5:
				{
					if (stack.count <= 5)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 6 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					for (signed int i = 0; i < 6; ++i)
					{
						Stack_pop(&stack);
					}

					Stack_push(&stack, &I64_VALUE_TYPE);
				} break;

				case TOKEN_INTRINSIC_SYSCALL6:
				{
					if (stack.count <= 6)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 7 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					for (signed int i = 0; i < 7; ++i)
					{
						Stack_pop(&stack);
					}

					Stack_push(&stack, &I64_VALUE_TYPE);
				} break;

				case TOKEN_INTRINSIC_CLONE:
				{
					if (stack.count <= 0)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 1 argument to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long a = *(signed long long*)Stack_pop(&stack);
					Stack_push(&stack, &a);
					Stack_push(&stack, &a);
				} break;

				case TOKEN_INTRINSIC_DROP:
				{
					if (stack.count <= 0)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 1 argument to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					Stack_pop(&stack);
				} break;

				case TOKEN_INTRINSIC_OVER:
				{
					if (stack.count <= 1)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 2 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long b = *(signed long long*)Stack_pop(&stack);
					signed long long a = *(signed long long*)Stack_pop(&stack);

					Stack_push(&stack, &a);
					Stack_push(&stack, &b);
					Stack_push(&stack, &a);
				} break;

#if HIVEC_DEBUG
// TODO: remove:
				case TOKEN_INTRINSIC_PRINTN:
				{
					if (stack.count <= 0)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` dev-intrinsic requires 1 argument to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					Stack_pop(&stack);
				} break;
#endif

				case TOKEN_INTRINSIC_SWAP:
				{
					if (stack.count <= 1)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "`%.*s` intrinsic requires 2 arguments to be on the stack!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					signed long long b = *(signed long long*)Stack_pop(&stack);
					signed long long a = *(signed long long*)Stack_pop(&stack);

					Stack_push(&stack, &b);
					Stack_push(&stack, &a);
				} break;

				case TOKEN_LITERAL_I64:
				{
					Stack_push(&stack, &I64_VALUE_TYPE);
				} break;

				case TOKEN_LITERAL_STRING:
				{
					Stack_push(&stack, &I64_VALUE_TYPE);
					Stack_push(&stack, &P64_VALUE_TYPE);
				} break;

				case TOKEN_IDENTIFIER:
				{
					struct Procedure* calledProcedure = NULL;

					for (struct LNode* proceduresIterator2 = globals->procedures.front; proceduresIterator2 != NULL; proceduresIterator2 = proceduresIterator2->next)
					{
						// NOTE: using `assert` and not `if`
						// REASONS:
						//     1. The procedures second iterator's data, in the list must never be
						//        of value null.
						//     2. This assert will prevent developers infliced bugs and development
						//        and debug configuration.
						assert(proceduresIterator2->data != NULL);

						struct Procedure* temp = (struct Procedure*)proceduresIterator2->data;

						if (token->source.length == temp->name->source.length
						 && strncmp(token->source.buffer, temp->name->source.buffer, token->source.length) == 0)
						{
							calledProcedure = temp;
							break;
						}
					}

					if (calledProcedure == NULL)
					{
						Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "encountered undefined procedure `%.*s`!", (signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}

					for (struct LNode* requiredTypesIterator = calledProcedure->requiredTypes.front; requiredTypesIterator != NULL; requiredTypesIterator = requiredTypesIterator->next)
					{
						// NOTE: using `assert` and not `if`
						// REASONS:
						//     1. The required types iterator's data, in the list must never be
						//        of value null.
						//     2. This assert will prevent developers infliced bugs and development
						//        and debug configuration.
						assert(requiredTypesIterator->data != NULL);

						struct Token* temp = (struct Token*)requiredTypesIterator->data;

						if (stack.count <= 0)
						{
							Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, temp->location, "expected a `%.*s` type to be on the stack, but the stack was empty!", (signed int)temp->source.length, temp->source.buffer));

#if HIVEC_DEBUG
							Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
								(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
								"locator of the log above this meesage."));
#endif

							return 0;
						}

						signed long long type = *(signed long long*)Stack_pop(&stack);

						if (temp->kind == TOKEN_KEYWORD_I64)
						{
							if (type != I64_VALUE_TYPE)
							{
								Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, temp->location, "encountered an invalid type! Expected an `%.*s` type to be on the stack!", (signed int)temp->source.length, temp->source.buffer));

#if HIVEC_DEBUG
								Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
									(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
									"locator of the log above this meesage."));
#endif

								return 0;
							}
						}
						else if (temp->kind == TOKEN_KEYWORD_P64)
						{
							if (type != P64_VALUE_TYPE)
							{
								Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, temp->location, "encountered an invalid type! Expected a `%.*s` type to be on the stack!", (signed int)temp->source.length, temp->source.buffer));

#if HIVEC_DEBUG
								Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
									(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
									"locator of the log above this meesage."));
#endif

								return 0;
							}
						}
						else
						{
							Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, temp->location, "encountered an invalid token `%.*s` in place of a type specifier!", (signed int)temp->source.length, temp->source.buffer));

#if HIVEC_DEBUG
							Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
								(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
								"locator of the log above this meesage."));
#endif

							return 0;
						}
					}

					for (struct LNode* returnedTypesIterator = calledProcedure->returnedTypes.front; returnedTypesIterator != NULL; returnedTypesIterator = returnedTypesIterator->next)
					{
						// NOTE: using `assert` and not `if`
						// REASONS:
						//     1. The returned types iterator's data, in the list must never be
						//        of value null.
						//     2. This assert will prevent developers infliced bugs and development
						//        and debug configuration.
						assert(returnedTypesIterator->data != NULL);

						struct Token* temp = (struct Token*)returnedTypesIterator->data;

						if (temp->kind == TOKEN_KEYWORD_I64)
						{
							Stack_push(&stack, &I64_VALUE_TYPE);
						}
						else if (temp->kind == TOKEN_KEYWORD_P64)
						{
							Stack_push(&stack, &P64_VALUE_TYPE);
						}
						else
						{
							Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, temp->location, "encountered an invalid token `%.*s` in place of a type specifier!", (signed int)temp->source.length, temp->source.buffer));

#if HIVEC_DEBUG
							Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
								(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
								"locator of the log above this meesage."));
#endif

							return 0;
						}
					}
				} break;

				default:
				{
				} break;
			}
		}

		for (struct LNode* returnedTypesIterator = procedure->returnedTypes.front; returnedTypesIterator != NULL; returnedTypesIterator = returnedTypesIterator->next)
		{
			// NOTE: using `assert` and not `if`
			// REASONS:
			//     1. The returned types iterator's data, in the list must never be of
			//        value null.
			//     2. This assert will prevent developers infliced bugs and development
			//        and debug configuration.
			assert(returnedTypesIterator->data != NULL);

			struct Token* token = (struct Token*)returnedTypesIterator->data;

			if (stack.count <= 0)
			{
				Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "expected `%.*s` type to be on the stack, but stack is empty!", (signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
				Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
					(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
					"locator of the log above this meesage."));
#endif

				return 0;
			}

			signed long long a = *(signed long long*)Stack_peek(&stack, 0);

			if (a == I64_VALUE_TYPE && token->kind == TOKEN_KEYWORD_I64)
			{
				Stack_pop(&stack);
			}
			else if (a == P64_VALUE_TYPE && token->kind == TOKEN_KEYWORD_P64)
			{
				Stack_pop(&stack);
			}
			else
			{
				Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, token->location, "expected `%.*s` type to be on the stack, but stack is empty!", (signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
				Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
					(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
					"locator of the log above this meesage."));
#endif

				return 0;
			}
		}

		if (stack.count > 0)
		{
			Queue_enqueue(logs, Log_create("validator", SEVERITY_ERROR, procedure->name->location, "procedure's `%.*s` stack has an overflow!", (signed int)procedure->name->source.length, procedure->name->source.buffer));

#if HIVEC_DEBUG
			Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
				(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
				"locator of the log above this meesage."));
#endif

			return 0;
		}
	}

	return 1;
}

/**
 * @}
 */
