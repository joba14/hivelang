
/**
 * @file types.h
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author joba14
 *
 * @date 2022-09-26
 */

#ifndef _TYPES_H_
#define _TYPES_H_

#include <hash256.h>

/**
 * @addtogroup types
 * 
 * @{
 */

struct SNode
{
	void* data;
	struct SNode* previous;
};

struct Stack
{
	struct SNode* top;
	int64_t count;
};

struct Stack Stack_create(
	void);

void Stack_destroy(
	struct Stack* const stack);

void Stack_push(
	struct Stack* const stack,
	void* data);

void* Stack_pop(
	struct Stack* const stack);

void* Stack_peek(
	struct Stack* const stack,
	int64_t offset);

struct QNode
{
	void* data;
	struct QNode* next;
};

struct Queue
{
	struct QNode* front;
	struct QNode* back;
	int64_t count;
};

struct Queue Queue_create(
	void);

void Queue_destroy(
	struct Queue* const queue);

void Queue_enqueue(
	struct Queue* const queue,
	void* data);

void* Queue_dequeue(
	struct Queue* const queue);

void* Queue_peek(
	struct Queue* const queue);

struct LNode
{
	void* data;
	struct LNode* next;
};

struct List
{
	struct LNode* front;
	struct LNode* back;
	int64_t count;
};

struct List List_create(
	void);

void List_destroy(
	struct List* const list);

void List_push(
	struct List* const list,
	void* data);

signed char List_exists(
	struct List* const list,
	void* data);

struct Location
{
	const char* file;
	int64_t line;
	int64_t column;
};

#define INVALID_LOCATION ((struct Location) { .file = NULL, .line = -1, .column = -1 })

signed char Location_isInvalid(
	const struct Location* const location);

const char* Location_stringify(
	const struct Location location);

struct Token
{
	int64_t id;

	enum
	{
		TOKEN_INVALID = 0,
		TOKEN_IDENTIFIER,

		TOKEN_FIRST_KEYWORD,
		TOKEN_KEYWORD_MAIN = TOKEN_FIRST_KEYWORD,
		TOKEN_KEYWORD_I64,
		TOKEN_KEYWORD_P64,
		TOKEN_KEYWORD_IF,
		TOKEN_KEYWORD_ELSE,
		TOKEN_KEYWORD_WHILE,
		TOKEN_KEYWORD_PROCEDURE,
		TOKEN_KEYWORD_REQUIRE,
		TOKEN_KEYWORD_RETURN,
		TOKEN_KEYWORD_DO,
		TOKEN_KEYWORD_END,
		TOKEN_LAST_KEYWORD = TOKEN_KEYWORD_END,

		TOKEN_FIRST_INTRINSIC,
		TOKEN_INTRINSIC_ADD = TOKEN_FIRST_INTRINSIC,
		TOKEN_INTRINSIC_SUBTRACT,
		TOKEN_INTRINSIC_MULTIPLY,
		TOKEN_INTRINSIC_DIVIDE,
		TOKEN_INTRINSIC_MODULUS,
		TOKEN_INTRINSIC_EQUAL,
		TOKEN_INTRINSIC_NEQUAL,
		TOKEN_INTRINSIC_GREATER,
		TOKEN_INTRINSIC_LESS,
		TOKEN_INTRINSIC_BAND,
		TOKEN_INTRINSIC_BOR,
		TOKEN_INTRINSIC_BNOT,
		TOKEN_INTRINSIC_SHIFTL,
		TOKEN_INTRINSIC_SHIFTR,
		TOKEN_INTRINSIC_SYSCALL0,
		TOKEN_INTRINSIC_SYSCALL1,
		TOKEN_INTRINSIC_SYSCALL2,
		TOKEN_INTRINSIC_SYSCALL3,
		TOKEN_INTRINSIC_SYSCALL4,
		TOKEN_INTRINSIC_SYSCALL5,
		TOKEN_INTRINSIC_SYSCALL6,
		TOKEN_INTRINSIC_CLONE,
		TOKEN_INTRINSIC_DROP,
		TOKEN_INTRINSIC_OVER,
#if HIVEC_DEBUG
// TODO: remove all development keywords:
		TOKEN_INTRINSIC_PRINTN,
#endif
		TOKEN_INTRINSIC_SWAP, // NOTE: might add more
		TOKEN_LAST_INTRINSIC = TOKEN_INTRINSIC_SWAP,

		TOKEN_FIRST_LITERAL,
		TOKEN_LITERAL_I64 = TOKEN_FIRST_LITERAL,
		TOKEN_LITERAL_STRING, // NOTE: might add char literal
		TOKEN_LAST_LITERAL = TOKEN_LITERAL_STRING,

		TOKEN_EOF,

		TOKENS_COUNT,
	} kind;

	union
	{
		int64_t i64;
		struct
		{
			unsigned char* bytes;
			int64_t length;
		} string;
	} value;

	struct
	{
		char* buffer;
		int64_t length;
	} source;

	struct Location location;
	struct Token* nextRef;
	struct Token* previousRef;
	struct Hash256 hash;
};

void Token_destroy(
	struct Token* const token);

const char* Token_stringify(
	const struct Token* const token);

struct Procedure
{
	struct Token* name;
	struct List requiredTypes;
	struct List returnedTypes;
	struct List body; // excluding `do` and `end`
	signed char isMain;
};

struct Procedure* Procedure_create(
	void);

void Procedure_destroy(
	struct Procedure* const procedure);

void Procedure_print(
	const struct Procedure* const procedure);

struct Globals
{
	struct List procedures;
	struct List stringLiterals;
};

struct Globals Globals_create(
	void);

void Globals_destroy(
	struct Globals* const globals);

enum Severity
{
	SEVERITY_SUCCESS = 0,
	SEVERITY_INFO,
	SEVERITY_WARNING,
	SEVERITY_ERROR,

	SEVERITIES_COUNT
};

const char* Severity_stringify(
	const enum Severity severity);

struct Log
{
	const char* author;
	enum Severity severity;
	struct Location location;
	#define logCapacity ((signed int)256)
	char content[logCapacity + 1];
};

struct Log* Log_create(
	const char* author,
	const enum Severity severity,
	const struct Location location,
	const char* format,
	...);

void Log_destroy(
	struct Log* const log);

void Log_print(
	const struct Log* const log);

/**
 * @}
 */

#endif
