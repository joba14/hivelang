
/**
 * @file types.c
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author jorisb
 *
 * @date 2022-09-26
 */

#include <types.h>

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

struct Stack Stack_create(
	void)
{
	struct Stack stack = {0};
	stack.top = NULL;
	stack.count = 0;
	return stack;
}

void Stack_destroy(
	struct Stack* const stack)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The stack, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(stack != NULL);

	for (struct SNode* iterator = stack->top; iterator != NULL;)
	{
		struct SNode* current = iterator;

		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The current node, must never ever be null. The iterator, having
		//        value of null, should necer enter the loop.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(current != NULL);

		iterator = iterator->previous;
		free(current);
	}

	stack->count = 0;
}

void Stack_push(
	struct Stack* const stack,
	void* data)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The stack, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(stack != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The data, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(data != NULL);

	if (stack->top == NULL)
	{
		stack->top = (struct SNode*)malloc(sizeof(struct SNode));

		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The memory allocation errors can happen anytime, no matter build
		//        configuration being debug or release. However, since the compiler
		//        cannot prevent such bugs, I will leave it as assert. Worst case
		//        scenario - the compiler crashes, and user re-runs it.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(stack->top != NULL);

		struct SNode* node = stack->top;
		node->data = data;
		node->previous = NULL;
	}
	else
	{
		struct SNode* node = (struct SNode*)malloc(sizeof(struct SNode));

		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The memory allocation errors can happen anytime, no matter build
		//        configuration being debug or release. However, since the compiler
		//        cannot prevent such bugs, I will leave it as assert. Worst case
		//        scenario - the compiler crashes, and user re-runs it.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(node != NULL);

		node->data = data;
		node->previous = stack->top;
		stack->top = node;
	}

	++stack->count;
}

void* Stack_pop(
	struct Stack* const stack)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The stack, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(stack != NULL);

	if (stack->top == NULL)
	{
		return NULL;
	}

	struct SNode* node = stack->top;
	stack->top = node->previous;
	void* data = node->data;
	free(node);
	--stack->count;
	return data;
}

void* Stack_peek(
	struct Stack* const stack,
	signed long long offset)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The stack, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(stack != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. An element should never be popped from an empty stack.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(stack->top != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. Offset must be positive or 0.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(offset >= 0);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. Offset must not go over the count of the stack.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(stack->count - offset >= 0);

	struct SNode* node = stack->top;

	for (signed long long index = 0; index < offset; ++index)
	{
		node = node->previous;
	}

	void* data = node->data;
	return data;
}

struct Queue Queue_create(
	void)
{
	struct Queue queue = {0};
	queue.front = NULL;
	queue.back = NULL;
	return queue;
}

void Queue_destroy(
	struct Queue* const queue)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The queue, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(queue != NULL);

	for (struct QNode* iterator = queue->front; iterator != NULL;)
	{
		struct QNode* current = iterator;

		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The current node, must never ever be null. The iterator, having
		//        value of null, should necer enter the loop.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(current != NULL);

		iterator = iterator->next;
		free(current);
	}

	queue->count = 0;
}

void Queue_enqueue(
	struct Queue* const queue,
	void* data)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The queue, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(queue != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The data, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(data != NULL);

	if (queue->front == NULL)
	{
		queue->back = (struct QNode*)malloc(sizeof(struct QNode));

		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The memory allocation errors can happen anytime, no matter build
		//        configuration being debug or release. However, since the compiler
		//        cannot prevent such bugs, I will leave it as assert. Worst case
		//        scenario - the compiler crashes, and user re-runs it.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(queue->back != NULL);

		struct QNode* node = queue->back;
		node->data = data;
		node->next = NULL;
		queue->front = queue->back;
	}
	else
	{
		struct QNode* node = (struct QNode*)malloc(sizeof(struct QNode));

		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The memory allocation errors can happen anytime, no matter build
		//        configuration being debug or release. However, since the compiler
		//        cannot prevent such bugs, I will leave it as assert. Worst case
		//        scenario - the compiler crashes, and user re-runs it.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(node != NULL);

		node->data = data;
		node->next = NULL;
		queue->back->next = node;
		queue->back = node;
	}

	++queue->count;
}

void* Queue_dequeue(
	struct Queue* const queue)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The queue, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(queue != NULL);

	if (queue->front == NULL)
	{
		return NULL;
	}

	struct QNode* node = queue->front;
	queue->front = node->next;
	void* data = node->data;
	free(node);
	--queue->count;
	return data;
}

void* Queue_peek(
	struct Queue* const queue)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The queue, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(queue != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. An element should never be dequeued from an empty queue.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(queue->front != NULL);

	struct QNode* node = queue->front;
	void* data = node->data;
	return data;
}

struct List List_create(
	void)
{
	struct List list = {0};
	list.front = NULL;
	list.back = NULL;
	return list;
}

void List_destroy(
	struct List* const list)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The list, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(list != NULL);

	for (struct LNode* iterator = list->front; iterator != NULL;)
	{
		struct LNode* current = iterator;

		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The current node, must never ever be null. The iterator, having
		//        value of null, should necer enter the loop.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(current != NULL);

		iterator = iterator->next;
		free(current);
	}

	list->count = 0;
}

void List_push(
	struct List* const list,
	void* data)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The list, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(list != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The data, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(data != NULL);

	if (list->front == NULL)
	{
		list->back = (struct LNode*)malloc(sizeof(struct LNode));

		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The memory allocation errors can happen anytime, no matter build
		//        configuration being debug or release. However, since the compiler
		//        cannot prevent such bugs, I will leave it as assert. Worst case
		//        scenario - the compiler crashes, and user re-runs it.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(list->back != NULL);

		struct LNode* node = list->back;
		node->data = data;
		node->next = NULL;
		list->front = list->back;
	}
	else
	{
		struct LNode* node = (struct LNode*)malloc(sizeof(struct LNode));

		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The memory allocation errors can happen anytime, no matter build
		//        configuration being debug or release. However, since the compiler
		//        cannot prevent such bugs, I will leave it as assert. Worst case
		//        scenario - the compiler crashes, and user re-runs it.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(node != NULL);

		node->data = data;
		node->next = NULL;
		list->back->next = node;
		list->back = node;
	}

	++list->count;
}

signed char List_exists(
	struct List* const list,
	void* data)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The list, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(list != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The data, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(data != NULL);

	for (struct LNode* iterator = list->front; iterator != NULL; iterator = iterator->next)
	{
		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The data of the iterator in the list must never be of value null,
		//        since the sources were already checked.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(iterator->data != NULL);

		if (data == iterator->data)
		{
			return 1;
		}
	}

	return 0;
}

const char* Location_stringify(
	const struct Location location)
{
	#define bufferCapcity ((signed int)512)
	static char buffer[bufferCapcity];
	snprintf(buffer, bufferCapcity, "%s:%lld:%lld", location.file, location.line, location.column);
	#undef bufferCapcity
	return buffer;
}

signed char Location_isInvalid(
	const struct Location* const location)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The location, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(location != NULL);

	return location->file == NULL && location->line == -1 && location->column == -1;
}

void Token_destroy(
	struct Token* const token)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The token, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(token != NULL);

	switch (token->kind)
	{
		case TOKEN_LITERAL_STRING:
		{
			free(token->value.string.bytes);
		} break;
	
		default:
		{
		} break;
	}

	free(token->source.buffer);
	free(token);
}

const char* Token_stringify(
	const struct Token* const token)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The token, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(token != NULL);

	// NOTE: this `stringifiedTokensKindsCount` define must be changed when modifying the `stringifiedTokensKinds` set!
#if HIVEC_DEBUG
// TODO: remove:
	#define stringifiedTokensKindsCount ((signed long long)42)
#else
	#define stringifiedTokensKindsCount ((signed long long)41)
#endif
	static const char* stringifiedTokensKinds[] =
	{
		[TOKEN_INVALID] = "invalid",
		[TOKEN_IDENTIFIER] = "identifier",

		[TOKEN_KEYWORD_MAIN] = "keyword_main",
		[TOKEN_KEYWORD_I64] = "keyword_i64",
		[TOKEN_KEYWORD_P64] = "keyword_p64",
		[TOKEN_KEYWORD_IF] = "keyword_if",
		[TOKEN_KEYWORD_ELSE] = "keyword_else",
		[TOKEN_KEYWORD_WHILE] = "keyword_while",
		[TOKEN_KEYWORD_PROCEDURE] = "keyword_procedure",
		[TOKEN_KEYWORD_REQUIRE] = "keyword_require",
		[TOKEN_KEYWORD_RETURN] = "keyword_return",
		[TOKEN_KEYWORD_DO] = "keyword_do",
		[TOKEN_KEYWORD_END] = "keyword_end",

		[TOKEN_INTRINSIC_ADD] = "intrinsic_add",
		[TOKEN_INTRINSIC_SUBTRACT] = "intrinsic_subtract",
		[TOKEN_INTRINSIC_MULTIPLY] = "intrinsic_multiplly",
		[TOKEN_INTRINSIC_DIVIDE] = "intrinsic_divide",
		[TOKEN_INTRINSIC_MODULUS] = "intrinsic_modulus",
		[TOKEN_INTRINSIC_EQUAL] = "intrinsic_equal",
		[TOKEN_INTRINSIC_NEQUAL] = "intrinsic_nequal",
		[TOKEN_INTRINSIC_GREATER] = "intrinsic_greater",
		[TOKEN_INTRINSIC_LESS] = "intrinsic_less",
		[TOKEN_INTRINSIC_BAND] = "intrinsic_band",
		[TOKEN_INTRINSIC_BOR] = "intrinsic_bor",
		[TOKEN_INTRINSIC_BNOT] = "intrinsic_bnot",
		[TOKEN_INTRINSIC_SHIFTL] = "intrinsic_shiftl",
		[TOKEN_INTRINSIC_SHIFTR] = "intrinsic_shiftr",
		[TOKEN_INTRINSIC_SYSCALL0] = "intrinsic_syscall0",
		[TOKEN_INTRINSIC_SYSCALL1] = "intrinsic_syscall1",
		[TOKEN_INTRINSIC_SYSCALL2] = "intrinsic_syscall2",
		[TOKEN_INTRINSIC_SYSCALL3] = "intrinsic_syscall3",
		[TOKEN_INTRINSIC_SYSCALL4] = "intrinsic_syscall4",
		[TOKEN_INTRINSIC_SYSCALL5] = "intrinsic_syscall5",
		[TOKEN_INTRINSIC_SYSCALL6] = "intrinsic_syscall6",
		[TOKEN_INTRINSIC_CLONE] = "intrinsic_clone",
		[TOKEN_INTRINSIC_DROP] = "intrinsic_drop",
		[TOKEN_INTRINSIC_OVER] = "intrinsic_over",
#if HIVEC_DEBUG
// TODO: remove:
		[TOKEN_INTRINSIC_PRINTN] = "dev_intrinsic_printn",
#endif
		[TOKEN_INTRINSIC_SWAP] = "intrinsic_swap",

		[TOKEN_LITERAL_I64] = "literal_i64",
		[TOKEN_LITERAL_STRING] = "literal_string",

		[TOKEN_EOF] = "enf_of_file"
	};
	static_assert(TOKENS_COUNT == stringifiedTokensKindsCount,
		"Stringified token kinds are out of sync with the kinds enum!");

	assert((long unsigned int)(token->kind) < (sizeof(stringifiedTokensKinds) / sizeof(const char*)));
	#define bufferCapcity ((signed int)1024)
	static char buffer[bufferCapcity];

	snprintf(buffer, bufferCapcity, "id=`%lld` kind=`%s` source=`%.*s` location=`%s`",
		token->id,
		stringifiedTokensKinds[token->kind],
		(int)token->source.length, token->source.buffer,
		Location_stringify(token->location));

	#undef bufferCapcity
	return buffer;
}

struct Procedure* Procedure_create(
	void)
{
	struct Procedure* procedure = (struct Procedure*)malloc(sizeof(struct Procedure));

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The memory allocation errors can happen anytime, no matter build
	//        configuration being debug or release. However, since the compiler
	//        cannot prevent such bugs, I will leave it as assert. Worst case
	//        scenario - the compiler crashes, and user re-runs it.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(procedure != NULL);

	procedure->name = NULL;
	procedure->requiredTypes = List_create();
	procedure->returnedTypes = List_create();
	procedure->body = List_create();
	procedure->isMain = 0;
	return procedure;
}

void Procedure_destroy(
	struct Procedure* const procedure)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The procedure, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(procedure != NULL);

	List_destroy(&procedure->requiredTypes);
	List_destroy(&procedure->returnedTypes);
	List_destroy(&procedure->body);
	free(procedure);
}

void Procedure_print(
	const struct Procedure* const procedure)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The procedure, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(procedure != NULL);

	fprintf(stdout, "procedure %.*s\n", (signed int)procedure->name->source.length, procedure->name->source.buffer);

	fprintf(stdout, "required types:");

	if (procedure->requiredTypes.count > 0)
	{
		for (struct LNode* iterator = procedure->requiredTypes.front; iterator != NULL; iterator = iterator->next)
		{
			// NOTE: using `assert` and not `if`
			// REASONS:
			//     1. The iterator's data, in the list must never be of value null.
			//     2. This assert will prevent developers infliced bugs and development
			//        and debug configuration.
			assert(iterator->data != NULL);

			struct Token* token = (struct Token*)iterator->data;
			fprintf(stdout, " %.*s", (signed int)token->source.length, token->source.buffer);
		}
	}

	fprintf(stdout, "\n");

	fprintf(stdout, "returned types:");

	if (procedure->returnedTypes.count > 0)
	{
		for (struct LNode* iterator = procedure->returnedTypes.front; iterator != NULL; iterator = iterator->next)
		{
			// NOTE: using `assert` and not `if`
			// REASONS:
			//     1. The iterator's data, in the list must never be of value null.
			//     2. This assert will prevent developers infliced bugs and development
			//        and debug configuration.
			assert(iterator->data != NULL);

			struct Token* token = (struct Token*)iterator->data;
			fprintf(stdout, " %.*s", (signed int)token->source.length, token->source.buffer);
		}
	}

	fprintf(stdout, "\n");

	fprintf(stdout, "body:");

	if (procedure->body.count > 0)
	{
		for (struct LNode* iterator = procedure->body.front; iterator != NULL; iterator = iterator->next)
		{
			// NOTE: using `assert` and not `if`
			// REASONS:
			//     1. The iterator's data, in the list must never be of value null.
			//     2. This assert will prevent developers infliced bugs and development
			//        and debug configuration.
			assert(iterator->data != NULL);

			struct Token* token = (struct Token*)iterator->data;
			fprintf(stdout, " %.*s", (signed int)token->source.length, token->source.buffer);
		}
	}

	fprintf(stdout, "\n");

	fprintf(stdout, "is main: %d\n", procedure->isMain != 0);
}

struct Globals Globals_create(
	void)
{
	struct Globals globals = {0};
	globals.procedures = List_create();
	globals.stringLiterals = List_create();
	return globals;
}

void Globals_destroy(
	struct Globals* const globals)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The globals, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(globals != NULL);

	for (struct LNode* proceduresIterator = globals->procedures.front; proceduresIterator != NULL; proceduresIterator = proceduresIterator->next)
	{
		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The data of the procedure iterator node, in the list must never be
		//        of value null.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(proceduresIterator->data != NULL);

		struct Procedure* procedure = (struct Procedure*)proceduresIterator->data;

		Procedure_destroy(procedure);
	}

	List_destroy(&globals->procedures);

	List_destroy(&globals->stringLiterals);
}

const char* Severity_stringify(
	const enum Severity severity)
{
	#define GREEN(str) "\033[32m"str"\033[0m"
	#define YELLOW(str) "\033[33m"str"\033[0m"
	#define RED(str) "\033[31m"str"\033[0m"

	// NOTE: this `stringifiedSeveritiesCount` define must be changed when modifying the `stringifiedSeverities` set!
	#define stringifiedSeveritiesCount ((signed long long)4)
	const char* stringifiedSeverities[] =
	{
		[SEVERITY_SUCCESS] = GREEN("success"),
		[SEVERITY_INFO] = "info",
		[SEVERITY_WARNING] = YELLOW("warning"),
		[SEVERITY_ERROR] = RED("error")
	};
	static_assert(SEVERITIES_COUNT == stringifiedSeveritiesCount,
		"Stringified severity types are out of sync with the severities enum!");

	assert((long unsigned int)(severity) < (sizeof(stringifiedSeverities) / sizeof(const char*)));
	return stringifiedSeverities[severity];
}

struct Log* Log_create(
	const char* author,
	const enum Severity severity,
	const struct Location location,
	const char* format,
	...)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The author, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(author != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The format, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(format != NULL);

	struct Log* log = (struct Log*)malloc(sizeof(struct Log));

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The memory allocation errors can happen anytime, no matter build
	//        configuration being debug or release. However, since the compiler
	//        cannot prevent such bugs, I will leave it as assert. Worst case
	//        scenario - the compiler crashes, and user re-runs it.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(log != NULL);

	*log = (struct Log) { .author = author, .severity = severity, .location = location };
	va_list args;

	va_start(args, format);
	signed long long written = vsnprintf(log->content, logCapacity * sizeof(char), format, args);
	log->content[written] = 0;

	va_end(args);
	return log;
}

void Log_destroy(
	struct Log* const log)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The log, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(log != NULL);

	free(log);
}

void Log_print(
	const struct Log* const log)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The log, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(log != NULL);

	FILE* stream = NULL;

	switch (log->severity)
	{
		case SEVERITY_SUCCESS:
		{
			stream = stdout;
		} break;

		case SEVERITY_INFO:
		{
			stream = stdout;
		} break;

		case SEVERITY_WARNING:
		{
			stream = stderr;
		} break;

		case SEVERITY_ERROR:
		{
			stream = stderr;
		} break;

		default:
		{
			// NOTE: using `assert` and not `if`
			// REASONS:
			//     1. The log, cannot have unknown severity type.
			//     2. This assert will prevent developers infliced bugs and development
			//        and debug configuration.
			assert(0);
		} break;
	}

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The stream cannot ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(stream != NULL);

	if (Location_isInvalid(&log->location))
	{
		fprintf(stream, "[%s]: %s: %s: %s\n", log->author, "[no location]", Severity_stringify(log->severity), log->content);
	}
	else
	{
		fprintf(stream, "[%s]: %s: %s: %s\n", log->author, Location_stringify(log->location), Severity_stringify(log->severity), log->content);
	}
}
