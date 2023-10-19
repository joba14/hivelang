
/**
 * @file translator.c
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author joba14
 *
 * @date 2022-09-26
 */

#include <translator.h>
#include <hash256.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * @addtogroup translator
 * 
 * @{
 */

static void Translator_translateProcedure(
	FILE* const file,
	const struct Procedure* const procedure);

signed char Translator_translateTokens(
	const char* filePath,
	struct Globals* const globals,
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
		Queue_enqueue(logs, Log_create("translator", SEVERITY_ERROR, INVALID_LOCATION, "provided IR file path was empty!"));

#if HIVEC_DEBUG
		Queue_enqueue(logs, Log_create("debug", SEVERITY_WARNING,
			(struct Location) { .file = (const char*)__FILE__, .line = (signed long long)__LINE__, .column = 0 },
			"locator of the log above this meesage."));
#endif

		return 0;
	}

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

	// NOTE: this error is being logged in parser function before entering
	//       this function.
	assert(globals->procedures.count > 0);

	FILE* file = fopen(filePath, "w");

	fprintf(file, "\n");
	fprintf(file, "BITS 64\n");
	fprintf(file, "\n");
	fprintf(file, "segment .text\n");
	fprintf(file, "\n");

#if HIVEC_DEBUG
	// TODO: remove this block.
	fprintf(file, "printn:\n");
	fprintf(file, "\tmov r9, -3689348814741910323\n");
	fprintf(file, "\tsub rsp, 40\n");
	fprintf(file, "\tmov BYTE [rsp + 31], 10\n");
	fprintf(file, "\tlea rcx, [rsp + 30]\n");
	fprintf(file, "\n");

	fprintf(file, ".L2:\n");
	fprintf(file, "\tmov rax, rdi\n");
	fprintf(file, "\tlea r8, [rsp + 32]\n");
	fprintf(file, "\tmul r9\n");
	fprintf(file, "\tmov rax, rdi\n");
	fprintf(file, "\tsub r8, rcx\n");
	fprintf(file, "\tshr rdx, 3\n");
	fprintf(file, "\tlea rsi, [rdx + rdx * 4]\n");
	fprintf(file, "\tadd rsi, rsi\n");
	fprintf(file, "\tsub rax, rsi\n");
	fprintf(file, "\tadd eax, 48\n");
	fprintf(file, "\tmov BYTE [rcx], al\n");
	fprintf(file, "\tmov rax, rdi\n");
	fprintf(file, "\tmov rdi, rdx\n");
	fprintf(file, "\tmov rdx, rcx\n");
	fprintf(file, "\tsub rcx, 1\n");
	fprintf(file, "\tcmp rax, 9\n");
	fprintf(file, "\tja .L2\n");
	fprintf(file, "\tlea rax, [rsp + 32]\n");
	fprintf(file, "\tmov edi, 1\n");
	fprintf(file, "\tsub rdx, rax\n");
	fprintf(file, "\txor eax, eax\n");
	fprintf(file, "\tlea rsi, [rsp + 32 + rdx]\n");
	fprintf(file, "\tmov rdx, r8\n");
	fprintf(file, "\tmov rax, 1\n");
	fprintf(file, "\tsyscall\n");
	fprintf(file, "\tadd rsp, 40\n");
	fprintf(file, "\tret\n");
#endif

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

		Translator_translateProcedure(file, procedure);
	}

	fprintf(file, "\n");
	fprintf(file, "segment .data\n");

	for (struct LNode* stringsIterator = globals->stringLiterals.front; stringsIterator != NULL; stringsIterator = stringsIterator->next)
	{
		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The strings iterator's data, in the list must never be of value
		//        null.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(stringsIterator->data != NULL);

		struct Token* token = (struct Token*)stringsIterator->data;

		fprintf(file, "\tstr_%s: db", hash256(token->source.buffer, token->source.length).stringified);

		for (signed long long i = 0; i < token->value.string.length; ++i)
		{
			fprintf(file, " %#02x", token->value.string.bytes[i]);

			if (i < token->value.string.length - 1)
			{
				fprintf(file, ",");
			}
		}

		fprintf(file, "\n");

		/*
		signed char duplicate = 0;
		struct Token* token = (struct Token*)stringsIterator->data;

		for (struct LNode* checkIterator = strings.front; checkIterator != stringsIterator; checkIterator = checkIterator->next)
		{
			// NOTE: using `assert` and not `if`
			// REASONS:
			//     1. The check iterator's data, in the list must never be of value
			//        null.
			//     2. This assert will prevent developers infliced bugs and development
			//        and debug configuration.
			assert(checkIterator->data != NULL);

			struct Token* temp = (struct Token*)checkIterator->data;

			if (memcmp(token->source.buffer, temp->source.buffer, temp->source.length) == 0)
			{
				duplicate = 1;
			}
		}
		*/

		/*
		if (!duplicate)
		{
			fprintf(file, "\tstr_%s: db", hash256(token->source.buffer, token->source.length).stringified);

			for (signed long long i = 0; i < token->value.string.length; ++i)
			{
				fprintf(file, " %#02x", token->value.string.bytes[i]);

				if (i < token->value.string.length - 1)
				{
					fprintf(file, ",");
				}
			}

			fprintf(file, "\n");
		}
		*/
	}

	fprintf(file, "\n");
	fprintf(file, "segment .bss\n");
	fprintf(file, "\targs_ptr: resq 1\n");
	#define RET_STACK_CAP ((signed long long)4096)
	fprintf(file, "\tret_stack_rsp: resq 1\n");
	fprintf(file, "\tret_stack: resb %lld\n", RET_STACK_CAP);
	fprintf(file, "\tret_stack_end:\n");

	return 1;
}

static void Translator_translateProcedure(
	FILE* const file,
	const struct Procedure* const procedure)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The file, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(file != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The procedure, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(procedure != NULL);

	if (procedure->isMain)
	{
		fprintf(file, ";; -- %.*s -- \n", (signed int)procedure->name->source.length, procedure->name->source.buffer);
		fprintf(file, "global _start\n");
		fprintf(file, "_start:\n");
		fprintf(file, "\tmov [args_ptr], rsp\n");
		fprintf(file, "\tmov rax, ret_stack_end\n");
		fprintf(file, "\tmov [ret_stack_rsp], rax\n");
	}
	else
	{
		fprintf(file, ";; -- %.*s -- \n", (signed int)procedure->name->source.length, procedure->name->source.buffer);
		fprintf(file, "proc_%s:\n", hash256(procedure->name->source.buffer, procedure->name->source.length).stringified);
		fprintf(file, "\tmov [ret_stack_rsp], rsp\n");
		fprintf(file, "\tmov rsp, rax\n");
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
			case TOKEN_IDENTIFIER:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "\tmov rax, rsp\n");
				fprintf(file, "\tmov rsp, [ret_stack_rsp]\n");
				fprintf(file, "\tcall proc_%s\n", hash256(token->source.buffer, token->source.length).stringified);
				fprintf(file, "\tmov [ret_stack_rsp], rsp\n");
				fprintf(file, "\tmov rsp, rax\n");
			} break;

			case TOKEN_KEYWORD_IF:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
			} break;
			
			case TOKEN_KEYWORD_ELSE:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);

				// NOTES:
				//     1. This should never ever be NULL. The cross reference is handled in parser and
				//        must fail in case errors like this and neverr reach the translator!
				//     2. Leaving the assert for development bugs.
				assert(token->nextRef != NULL);

				fprintf(file, "\tjmp addr_%s\n", token->nextRef->hash.stringified);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
			} break;
			
			case TOKEN_KEYWORD_WHILE:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
			} break;
			
			case TOKEN_KEYWORD_DO:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);

				fprintf(file, "\tpop rax\n");
				fprintf(file, "\ttest rax, rax\n");

				// NOTES:
				//     1. This should never ever be NULL. The cross reference is handled in parser and
				//        must fail in case errors like this and neverr reach the translator!
				//     2. Leaving the assert for development bugs.
				assert(token->nextRef != NULL);

				fprintf(file, "\tjz addr_%s\n", token->nextRef->hash.stringified);
			} break;

			case TOKEN_KEYWORD_END:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);

				if (token->nextRef != NULL) // while
				{
					fprintf(file, "\tjmp addr_%s\n", token->nextRef->hash.stringified);
				}

				fprintf(file, "addr_%s:\n", token->hash.stringified);
			} break;

			case TOKEN_INTRINSIC_ADD:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tpop rbx\n");
				fprintf(file, "\tadd rax, rbx\n");
				fprintf(file, "\tpush rax\n");
			} break;

			case TOKEN_INTRINSIC_SUBTRACT:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tpop rbx\n");
				fprintf(file, "\tsub rbx, rax\n");
				fprintf(file, "\tpush rbx\n");
			} break;

			case TOKEN_INTRINSIC_MULTIPLY:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tpop rbx\n");
				fprintf(file, "\tmul rbx\n");
				fprintf(file, "\tpush rax\n");
			} break;

			case TOKEN_INTRINSIC_DIVIDE:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rcx\n");
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tmov rdx, 0\n");
				fprintf(file, "\tdiv rcx\n");
				fprintf(file, "\tpush rax\n");
			} break;

			case TOKEN_INTRINSIC_MODULUS:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rcx\n");
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tmov rdx, 0\n");
				fprintf(file, "\tdiv rcx\n");
				fprintf(file, "\tpush rdx\n");
			} break;

			case TOKEN_INTRINSIC_EQUAL:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tmov rcx, 0\n");
				fprintf(file, "\tmov rdx, 1\n");
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tpop rbx\n");
				fprintf(file, "\tcmp rax, rbx\n");
				fprintf(file, "\tcmove rcx, rdx\n");
				fprintf(file, "\tpush rcx\n");
			} break;

			case TOKEN_INTRINSIC_NEQUAL:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tmov rcx, 1\n");
				fprintf(file, "\tmov rdx, 0\n");
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tpop rbx\n");
				fprintf(file, "\tcmp rax, rbx\n");
				fprintf(file, "\tcmove rcx, rdx\n");
				fprintf(file, "\tpush rcx\n");
			} break;

			case TOKEN_INTRINSIC_GREATER:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tmov rcx, 0\n");
				fprintf(file, "\tmov rdx, 1\n");
				fprintf(file, "\tpop rbx\n");
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tcmp rax, rbx\n");
				fprintf(file, "\tcmovg rcx, rdx\n");
				fprintf(file, "\tpush rcx\n");
			} break;

			case TOKEN_INTRINSIC_LESS:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tmov rcx, 0\n");
				fprintf(file, "\tmov rdx, 1\n");
				fprintf(file, "\tpop rbx\n");
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tcmp rax, rbx\n");
				fprintf(file, "\tcmovl rcx, rdx\n");
				fprintf(file, "\tpush rcx\n");
			} break;

			case TOKEN_INTRINSIC_BAND:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tpop rbx\n");
				fprintf(file, "\tand rbx, rax\n");
				fprintf(file, "\tpush rbx\n");
			} break;

			case TOKEN_INTRINSIC_BOR:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tpop rbx\n");
				fprintf(file, "\tor rbx, rax\n");
				fprintf(file, "\tpush rbx\n");
			} break;

			case TOKEN_INTRINSIC_BNOT:
			{
				// TODO: test it out!
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tmov eax, edi\n");
				fprintf(file, "\tnot eax\n");
				assert(!"FIXME: does not work!");
			} break;

			case TOKEN_INTRINSIC_SHIFTL:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rcx\n");
				fprintf(file, "\tpop rbx\n");
				fprintf(file, "\tshl rbx, cl\n");
				fprintf(file, "\tpush rbx\n");
			} break;

			case TOKEN_INTRINSIC_SHIFTR:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rcx\n");
				fprintf(file, "\tpop rbx\n");
				fprintf(file, "\tshr rbx, cl\n");
				fprintf(file, "\tpush rbx\n");
			} break;

			case TOKEN_INTRINSIC_SYSCALL0:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tsyscall\n");
				fprintf(file, "\tpush rax\n");
			} break;

			case TOKEN_INTRINSIC_SYSCALL1:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tpop rdi\n");
				fprintf(file, "\tsyscall\n");
				fprintf(file, "\tpush rax\n");
			} break;

			case TOKEN_INTRINSIC_SYSCALL2:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tpop rdi\n");
				fprintf(file, "\tpop rsi\n");
				fprintf(file, "\tsyscall\n");
				fprintf(file, "\tpush rax\n");
			} break;

			case TOKEN_INTRINSIC_SYSCALL3:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tpop rdi\n");
				fprintf(file, "\tpop rsi\n");
				fprintf(file, "\tpop rdx\n");
				fprintf(file, "\tsyscall\n");
				fprintf(file, "\tpush rax\n");
			} break;

			case TOKEN_INTRINSIC_SYSCALL4:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tpop rdi\n");
				fprintf(file, "\tpop rsi\n");
				fprintf(file, "\tpop rdx\n");
				fprintf(file, "\tpop r10\n");
				fprintf(file, "\tsyscall\n");
				fprintf(file, "\tpush rax\n");
			} break;

			case TOKEN_INTRINSIC_SYSCALL5:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tpop rdi\n");
				fprintf(file, "\tpop rsi\n");
				fprintf(file, "\tpop rdx\n");
				fprintf(file, "\tpop r10\n");
				fprintf(file, "\tpop r5\n");
				fprintf(file, "\tsyscall\n");
				fprintf(file, "\tpush rax\n");
			} break;

			case TOKEN_INTRINSIC_SYSCALL6:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tpop rdi\n");
				fprintf(file, "\tpop rsi\n");
				fprintf(file, "\tpop rdx\n");
				fprintf(file, "\tpop r10\n");
				fprintf(file, "\tpop r5\n");
				fprintf(file, "\tpop r9\n");
				fprintf(file, "\tsyscall\n");
				fprintf(file, "\tpush rax\n");
			} break;

			case TOKEN_INTRINSIC_CLONE:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tpush rax\n");
				fprintf(file, "\tpush rax\n");
			} break;

			case TOKEN_INTRINSIC_DROP:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rax\n");
			} break;

			case TOKEN_INTRINSIC_OVER:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tpop rbx\n");
				fprintf(file, "\tpush rbx\n");
				fprintf(file, "\tpush rax\n");
				fprintf(file, "\tpush rbx\n");
			} break;

#if HIVEC_DEBUG
// TODO: remove:
			case TOKEN_INTRINSIC_PRINTN:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rdi\n");
				fprintf(file, "\tcall printn\n");
			} break;
#endif

			case TOKEN_INTRINSIC_SWAP:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tpop rax\n");
				fprintf(file, "\tpop rbx\n");
				fprintf(file, "\tpush rax\n");
				fprintf(file, "\tpush rbx\n");
			} break;

			case TOKEN_LITERAL_I64:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);
				fprintf(file, "\tmov rax, %lld\n", token->value.i64);
				fprintf(file, "\tpush rax\n");
			} break;

			case TOKEN_LITERAL_STRING:
			{
				fprintf(file, ";; -- %.*s -- \n", (signed int)token->source.length, token->source.buffer);
				fprintf(file, "addr_%s:\n", token->hash.stringified);

				// Pushing string's length
				fprintf(file, "\tmov rax, %lld\n", token->value.string.length);
				fprintf(file, "\tpush rax\n");

				// Pushing pointer to the string
				fprintf(file, "\tpush str_%s\n", hash256(token->source.buffer, token->source.length).stringified);
			} break;

			default:
			{
				// NOTE: SHOULD NEVER BE REACHED, BECAUSE ALL ERRORS MUST BE HANDLED IN
				//       PROCESSES HAPPENED BEFORE THE TRANSLATOR!!!
				// TODO: add crash here and log!:
				// TODO: remove(1):
				assert(0);
			} break;
		}
	}

	if (procedure->isMain)
	{
		fprintf(file, ";; -- end -- \n");
		fprintf(file, "\tmov rax, 60\n");
		fprintf(file, "\tmov rdi, 0\n");
		fprintf(file, "\tsyscall\n");
	}
	else
	{
		fprintf(file, "\tmov rax, rsp\n");
		fprintf(file, "\tmov rsp, [ret_stack_rsp]\n");
		fprintf(file, "\tret\n");
	}
}

/**
 * @}
 */
