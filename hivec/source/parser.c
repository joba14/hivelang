
/**
 * @file parser.c
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author joba14
 *
 * @date 2022-10-04
 */

#include <parser.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * @addtogroup parser
 * 
 * @{
 */

struct ParserContext
{
	struct LNode* iterator;
};

static void Parser_collectStringLiterals(
	struct List* const stringLiterals,
	const struct List* const tokens,
	struct Queue* const logs);

static signed char Parser_parseProcedure(
	struct Procedure* const procedure,
	struct ParserContext* const context,
	struct Queue* const logs);

static signed char Parser_crossReference(
	struct Globals* const globals,
	struct Queue* const logs);

signed char Parser_parseTokens(
	struct Globals* const globals,
	const struct List* const tokens,
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
	//     1. The tokens, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(tokens != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The logs, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	// ./optimizer.c
	assert(logs != NULL);

	// NOTE:
	//     This case is being checked in parser and must not be logged here. It should never
	//     fail here too. This check is helpful for debugging purposes in development
	//     process.
	assert(tokens->count > 0);

	Parser_collectStringLiterals(&globals->stringLiterals, tokens, logs);
	signed char parsedMain = 0;

	for (struct LNode* iterator = tokens->front; iterator != NULL;)
	{
		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The iterator's data, in the list must never be of value null.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(iterator->data != NULL);

		struct Token* token = (struct Token*)iterator->data;

		switch (token->kind)
		{
			case TOKEN_KEYWORD_PROCEDURE:
			{
				struct ParserContext context = (struct ParserContext) { .iterator = iterator };
				struct Procedure* procedure = Procedure_create();

				if (!Parser_parseProcedure(procedure, &context, logs))
				{
					return 0;
				}

				if (procedure->name->kind == TOKEN_KEYWORD_MAIN)
				{
					parsedMain = 1;
				}

				for (struct LNode* proceduresIterator = globals->procedures.front; proceduresIterator != NULL; proceduresIterator = proceduresIterator->next)
				{
					// NOTE: using `assert` and not `if`
					// REASONS:
					//     1. The procedures iterator's data, in the list must never be of value
					//        null.
					//     2. This assert will prevent developers infliced bugs and development
					//        and debug configuration.
					assert(proceduresIterator->data != NULL);

					struct Procedure* existing = (struct Procedure*)proceduresIterator->data;

					if (procedure->name->source.length == existing->name->source.length
					 && strncmp(procedure->name->source.buffer, existing->name->source.buffer, procedure->name->source.length) == 0)
					{
						Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, existing->name->location, "encountered an already defined procedure `%.*s`!",
							(signed int)existing->name->source.length, existing->name->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						return 0;
					}
				}

				List_push(&globals->procedures, procedure);

				iterator = context.iterator;

				if (iterator != NULL)
				{
					iterator = iterator->next;
				}
			} break;

			default:
			{
				Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "encountered an invalid global token `%.*s`!", (signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
				Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
					(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
					"locator of the log above this meesage."));
#endif

				return 0;
			} break;
		}
	}

	if (!parsedMain)
	{
		Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, INVALID_LOCATION, "missing `main` procedure!"));

#if HIVEC_DEBUG
		Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
			(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
			"locator of the log above this meesage."));
#endif

		return 0;
	}

	return Parser_crossReference(globals, logs);
}

static void Parser_collectStringLiterals(
	struct List* const stringLiterals,
	const struct List* const tokens,
	struct Queue* const logs)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The string literls list, provided to this function, must never
	//        ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(stringLiterals != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The tokens list, provided to this function, must never ever be
	//        null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(tokens != NULL);

	if (tokens->count <= 0)
	{
		return;
	}

	(void)logs;

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The logs, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(logs != NULL);

	for (struct LNode* iterator = tokens->front; iterator != NULL; iterator = iterator->next)
	{
		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The iterator's data, in the list must never be of value null.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(iterator->data != NULL);

		struct Token* token = (struct Token*)iterator->data;

		switch (token->kind)
		{
			case TOKEN_LITERAL_STRING:
			{
				signed char stringLiteralAlreadyExists = 0;

				for (struct LNode* stringLiteralsIterator = stringLiterals->front; stringLiteralsIterator != NULL; stringLiteralsIterator = stringLiteralsIterator->next)
				{
					// NOTE: using `assert` and not `if`
					// REASONS:
					//     1. The string literal's iterator's data, in the list must never be
					//        of value null.
					//     2. This assert will prevent developers infliced bugs and development
					//        and debug configuration.
					assert(stringLiteralsIterator->data != NULL);

					struct Token* stringLiteral = (struct Token*)stringLiteralsIterator->data;

					if (token->source.length == stringLiteral->source.length
					 && strncmp(token->source.buffer, stringLiteral->source.buffer, token->source.length) == 0)
					{
						stringLiteralAlreadyExists = 1;
						break;
					}
				}

				if (!stringLiteralAlreadyExists)
				{
					List_push(stringLiterals, token);
				}
			} break;

			default:
			{
			} break;
		}
	}
}

static signed char Parser_parseProcedure(
	struct Procedure* const procedure,
	struct ParserContext* const context,
	struct Queue* const logs)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The procedure, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(procedure != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The context, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(context != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The context's iterator, provided to this function, must never ever
	//        be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(context->iterator != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The logs, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(logs != NULL);

	// STEPS:
	//     1. Validate that current token kind is `procedure` and parse optional specifiers:
	//        inline and unsafe.
	//     2. Validate that the token, following the `procedure` token, is an identifier
	//        token.
	//     3. Try to collect type specifiers until reaching `do` token.
	//     4. Parse the body of a procedure.
	// 
	// NOTES:
	//     1. The procedure's body CAN contain invalid tokens. The tokens will be validated
	//        by the validator later.

	// [STEP 1] (Validate that current token kind is `procedure`).
	struct Token* token = (struct Token*)context->iterator->data; // Iterator, at this point, should never be NULL!

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The token in iterator's node must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(token != NULL);

	if (token->kind != TOKEN_KEYWORD_PROCEDURE) 
	{
		if (token->kind == TOKEN_INVALID)
		{
			Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "encountered an invalid token `%.*s`!", (signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
			Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
				(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
				"locator of the log above this meesage."));
#endif

			return 0;
		}
		else
		{
			Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "expected a `procedure`, `unsafe`, or `inline` keyword, but found `%.*s`!", (signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
			Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
				(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
				"locator of the log above this meesage."));
#endif

			return 0;
		}
	}

	if (token->kind == TOKEN_KEYWORD_PROCEDURE)
	{
		if (context->iterator->next == NULL)
		{
			Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "missing an identifier token after the `procedure` keyword!"));

#if HIVEC_DEBUG
			Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
				(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
				"locator of the log above this meesage."));
#endif

			return 0;
		}

		context->iterator = context->iterator->next;
		token = (struct Token*)context->iterator->data;

		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The token in iterator's node must never ever be null.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(token != NULL);
	}
	else
	{
		Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "encountered an nivalid token `%.*s` instead of the `procedure` keyword!", (signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
		Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
			(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
			"locator of the log above this meesage."));
#endif

		return 0;
	}

	// [STEP 2] (Validate that the token, following the `procedure` token, is an identifier
	//           token).

	if (token->kind != TOKEN_IDENTIFIER
	 && token->kind != TOKEN_KEYWORD_MAIN) 
	{
		if (token->kind == TOKEN_INVALID)
		{
			Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "encountered an invalid token `%.*s`!", (signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
			Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
				(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
				"locator of the log above this meesage."));
#endif

			return 0;
		}
		else
		{
			Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "encountered an invalid token for procedure's name: `%.*s`!", (signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
			Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
				(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
				"locator of the log above this meesage."));
#endif

			return 0;
		}
	}

	procedure->name = token;
	procedure->isMain = token->kind == TOKEN_KEYWORD_MAIN;

	// [STEP 3] (Try to collect type specifiers until reaching `do` token).
	if (context->iterator->next == NULL)
	{
		Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "expected a `do`, `require`, or `return` keyword to follow the procedure's name!"));

#if HIVEC_DEBUG
		Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
			(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
			"locator of the log above this meesage."));
#endif

		return 0;
	}

	context->iterator = context->iterator->next;
	token = (struct Token*)context->iterator->data; // Iterator, at this point, should never be NULL!

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The token in iterator's node must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(token != NULL);

	if (token->kind == TOKEN_KEYWORD_REQUIRE)
	{
		for (context->iterator = context->iterator->next; context->iterator != NULL;)
		{
			// NOTE: using `assert` and not `if`
			// REASONS:
			//     1. The iterator's data, in the list must never be of value null.
			//     2. This assert will prevent developers infliced bugs and development
			//        and debug configuration.
			assert(context->iterator->data != NULL);

			token = (struct Token*)context->iterator->data;

			if (token->kind == TOKEN_KEYWORD_I64
			 || token->kind == TOKEN_KEYWORD_P64)
			{
				List_push(&procedure->requiredTypes, token);
				context->iterator = context->iterator->next;
			}
			else if (token->kind == TOKEN_KEYWORD_RETURN
				  || token->kind == TOKEN_KEYWORD_DO)
			{
				break;
			}
			else
			{
				Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "encountered invalid keyword `%.*s` in the place of `require` types specifiers!", (signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
				Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
					(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
					"locator of the log above this meesage."));
#endif

				return 0;
			}
		}
	}

	if (token->kind == TOKEN_KEYWORD_RETURN)
	{
		for (context->iterator = context->iterator->next; context->iterator != NULL;)
		{
			// NOTE: using `assert` and not `if`
			// REASONS:
			//     1. The iterator's data, in the list must never be of value null.
			//     2. This assert will prevent developers infliced bugs and development
			//        and debug configuration.
			assert(context->iterator->data != NULL);

			token = (struct Token*)context->iterator->data;

			if (token->kind == TOKEN_KEYWORD_I64
			 || token->kind == TOKEN_KEYWORD_P64)
			{
				List_push(&procedure->returnedTypes, token);
				context->iterator = context->iterator->next;
			}
			else if (token->kind == TOKEN_KEYWORD_DO)
			{
				break;
			}
			else
			{
				Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "encountered invalid keyword `%.*s` in the place of `return` types specifiers!", (signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
				Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
					(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
					"locator of the log above this meesage."));
#endif

				return 0;
			}
		}
	}

	// [STEP 4] (Parse the body of a procedure).
	if (token->kind == TOKEN_KEYWORD_DO)
	{
		struct Stack stack = Stack_create();

		if (context->iterator->next == NULL)
		{
			Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "failed to parse procedure's body!"));

#if HIVEC_DEBUG
			Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
				(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
				"locator of the log above this meesage."));
#endif

			Stack_destroy(&stack);
			return 0;
		}

		Stack_push(&stack, token); // Pushing `do` onto the stack for scoping.
		context->iterator = context->iterator->next;

		for (; context->iterator != NULL;)
		{
			// NOTE: using `assert` and not `if`
			// REASONS:
			//     1. The iterator's data, in the list must never be of value null.
			//     2. This assert will prevent developers infliced bugs and development
			//        and debug configuration.
			assert(context->iterator->data != NULL);

			token = (struct Token*)context->iterator->data;

			if (token->kind == TOKEN_INVALID)
			{
				Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "encountered an invalid token `%.*s`!", (signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
				Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
					(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
					"locator of the log above this meesage."));
#endif

				Stack_destroy(&stack);
				return 0;
			}

			switch (token->kind)
			{
				case TOKEN_KEYWORD_DO:
				{
					Stack_push(&stack, token);
					List_push(&procedure->body, token);
				} break;

				case TOKEN_KEYWORD_END:
				{
					if (stack.count <= 0)
					{
						// TODO: after implementing `with` remove [UNIMPLEMENTED mark for the `with ... ` sequence.
						Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "`%.*s` keyword must follow `if <condition> do <block>`, `if <condition> do <block> else <block>`, `while <condition> do <block>`, or `[UNIMPLEMENTED] with <identifiers> do <block>` sequences!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						Stack_destroy(&stack);
						return 0;
					}

					Stack_pop(&stack);

					if (stack.count <= 0)
					{
						Stack_destroy(&stack);
						return 1;
					}
					else
					{
						List_push(&procedure->body, token);
					}
				} break;

				case TOKEN_KEYWORD_PROCEDURE:
				case TOKEN_KEYWORD_REQUIRE:
				case TOKEN_KEYWORD_RETURN:
				case TOKEN_KEYWORD_I64:
				case TOKEN_KEYWORD_P64:
				{
					Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "encountered invalid token `%.*s` in procedure's body!",
						(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
					Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
						(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
						"locator of the log above this meesage."));
#endif

					Stack_destroy(&stack);
					return 0;
				} break;

				default:
				{
					List_push(&procedure->body, token);
				} break;
			}

			context->iterator = context->iterator->next;
		}

		if (stack.count > 0)
		{
			struct Token* unhandled = NULL;

			while ((unhandled = Stack_pop(&stack)) != NULL)
			{
				Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, unhandled->location, "encountered an unhandled token `%.*s`!", (signed int)unhandled->source.length, unhandled->source.buffer));

#if HIVEC_DEBUG
				Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
					(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
					"locator of the log above this meesage."));
#endif
			}

			Stack_destroy(&stack);
			return 0;
		}

		Stack_destroy(&stack);
	}
	else
	{
		Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "encountered an invalid keyword `%.*s` instead of `do` keyword after procedure's definition!", (signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
		Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
			(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
			"locator of the log above this meesage."));
#endif

		return 0;
	}

	return 1;
}

static signed char Parser_crossReference(
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
	// ./optimizer.c
	assert(logs != NULL);

	// NOTES:
	//     1. Since procedures count is validated here, it is not required to be logged in
	//        any following subprocess. However, I advice to place asserts in all those
	//        places to catch any possible developer-inflicted bugs.
	//     2. In case the tokens list is empty (the source was empty of compilable code),
	//        all the following processes must be skipped and all resources deallocated.
	if (globals->procedures.count <= 0)
	{
		Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, INVALID_LOCATION, "failed to parse any procedures!"));

#if HIVEC_DEBUG
		Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
			(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
			"locator of the log above this meesage."));
#endif

		return 0;
	}

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

		if (procedure->body.count <= 0)
		{
			continue;
		}

		struct Stack stack = Stack_create();

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
				case TOKEN_KEYWORD_IF:
				case TOKEN_KEYWORD_WHILE:
				{
					if (stack.count > 0)
					{
						struct Token* peeked = (struct Token*)Stack_peek(&stack, 0);

						// NOTE: using `assert` and not `if`
						// REASONS:
						//     1. The peeked token, in the stack must never be of value null.
						//     2. This assert will prevent developers infliced bugs and development
						//        and debug configuration.
						assert(peeked != NULL);

						if (peeked->kind == TOKEN_KEYWORD_IF
						 || peeked->kind == TOKEN_KEYWORD_WHILE)
						{
							// TODO: after implementing `with` remove [UNIMPLEMENTED mark for the `with ... ` sequence.
							Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, peeked->location, "encountered an invalid keyword `%.*s`, following the `%.*s` keyword!",
								(signed int)token->source.length, token->source.buffer,
								(signed int)peeked->source.length, peeked->source.buffer));

#if HIVEC_DEBUG
							Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
								(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
								"locator of the log above this meesage."));
#endif

							Stack_destroy(&stack);
							return 0;
						}
					}

					Stack_push(&stack, token);
				} break;

				case TOKEN_KEYWORD_ELSE:
				{
					if (stack.count <= 0)
					{
						Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "`%.*s` keyword must follow `if <condition> do <block>` sequence!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						Stack_destroy(&stack);
						return 0;
					}

					struct Token* popped = Stack_pop(&stack);

					// NOTE: using `assert` and not `if`
					// REASONS:
					//     1. The popped token, in the stack must never be of value null.
					//     2. This assert will prevent developers infliced bugs and development
					//        and debug configuration.
					assert(popped != NULL);

					if (popped->previousRef == NULL)
					{
						Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, popped->location, "missing `if` keyword in `if <condition> do` sequence!"));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						Stack_destroy(&stack);
						return 0;
					}

					if (popped->kind == TOKEN_KEYWORD_DO
					 && popped->previousRef->kind == TOKEN_KEYWORD_IF)
					{
						popped->nextRef = token;
						token->previousRef = popped;
						Stack_push(&stack, token);
					}
					else
					{
						Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "`%.*s` keyword must follow `if <condition> do <block>` sequence!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						Stack_destroy(&stack);
						return 0;
					}
				} break;

				case TOKEN_KEYWORD_DO:
				{
					if (stack.count <= 0)
					{
						// TODO: after implementing `with` remove [UNIMPLEMENTED mark for the `with ... ` sequence.
						Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "`%.*s` keyword must follow `if <condition>`, `while <condition>`, or `[UNIMPLEMENTED] with <identifiers>` sequence!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						Stack_destroy(&stack);
						return 0;
					}

					struct Token* popped = Stack_pop(&stack);

					// NOTE: using `assert` and not `if`
					// REASONS:
					//     1. The popped token, in the stack must never be of value null.
					//     2. This assert will prevent developers infliced bugs and development
					//        and debug configuration.
					assert(popped != NULL);

					if (popped->kind == TOKEN_KEYWORD_IF
					 || popped->kind == TOKEN_KEYWORD_WHILE)
					{
						popped->nextRef = token;
						token->previousRef = popped;
						Stack_push(&stack, token);
					}
					else
					{
						// TODO: after implementing `with` remove [UNIMPLEMENTED mark for the `with ... ` sequence.
						Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "`%.*s` keyword must follow `if <condition>`, `while <condition>`, or `[UNIMPLEMENTED] with <identifiers>` sequence!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						Stack_destroy(&stack);
						return 0;
					}
				} break;

				case TOKEN_KEYWORD_END:
				{
					if (stack.count <= 0)
					{
						// TODO: after implementing `with` remove [UNIMPLEMENTED mark for the `with ... ` sequence.
						Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "`%.*s` keyword must follow `if <condition> do <block>`, `if <condition> do <block> else <block>`, `while <condition> do <block>`, or `[UNIMPLEMENTED] with <identifiers> do <block>` sequence!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						Stack_destroy(&stack);
						return 0;
					}

					struct Token* popped = Stack_pop(&stack);

					// NOTE: using `assert` and not `if`
					// REASONS:
					//     1. The popped token, in the stack must never be of value null.
					//     2. This assert will prevent developers infliced bugs and development
					//        and debug configuration.
					assert(popped != NULL);

					if (popped->kind == TOKEN_KEYWORD_DO)
					{
						if (popped->previousRef->kind == TOKEN_KEYWORD_IF)
						{
							popped->nextRef = token;
							token->previousRef = popped;
						}
						else if (popped->previousRef->kind == TOKEN_KEYWORD_WHILE)
						{
							popped->nextRef = token;
							token->previousRef = popped;
							token->nextRef = popped->previousRef;
						}
						else
						{
							Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "encountered invalid keyword `%.*s` before the `do` keyword!",
								(signed int)popped->source.length, popped->source.buffer));

#if HIVEC_DEBUG
							Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
								(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
								"locator of the log above this meesage."));
#endif

							Stack_destroy(&stack);
							return 0;
						}
					}
					else if (popped->kind == TOKEN_KEYWORD_ELSE)
					{
						popped->nextRef = token;
						token->previousRef = popped;
					}
					else
					{
						// TODO: after implementing `with` remove [UNIMPLEMENTED mark for the `with ... ` sequence.
						Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, token->location, "`%.*s` keyword must follow `if <condition> do <block>`, `if <condition> do <block> else <block>`, `while <condition> do <block>`, or `[UNIMPLEMENTED] with <identifiers> do <block>` sequence!",
							(signed int)token->source.length, token->source.buffer));

#if HIVEC_DEBUG
						Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
							(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
							"locator of the log above this meesage."));
#endif

						Stack_destroy(&stack);
						return 0;
					}
				} break;

				default:
				{
				} break;
			}
		}

		if (stack.count > 0)
		{
			struct Token* unhandled = NULL;

			while ((unhandled = Stack_pop(&stack)) != NULL)
			{
				Queue_enqueue(logs, Log_create("parser", SEVERITY_ERROR, unhandled->location, "encountered an unhandled token `%.*s`!", (signed int)unhandled->source.length, unhandled->source.buffer));

#if HIVEC_DEBUG
				Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
					(struct Location) { .file = (const char*)__FILE__, .line = (int64_t)__LINE__, .column = 0 },
					"locator of the log above this meesage."));
#endif
			}

			Stack_destroy(&stack);
			return 0;
		}

		Stack_destroy(&stack);
	}

	return 1;
}

/**
 * @}
 */
