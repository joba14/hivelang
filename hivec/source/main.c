
/**
 * @file main.c
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author jorisb
 *
 * @date 2022-09-26
 */

#include <types.h>
#include <lexer.h>
#include <parser.h>
#include <validator.h>
#include <translator.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef WIN32
// TODO: add equivalent of sys/stat.h!
#	error "TODO: need to add equivalent of sys/stat.h!"
#else
#	include <sys/stat.h>
#endif

static void usage(
	FILE* const stream,
	const char* argv0);

static const char* shift(
	int* const argc,
	char*** const argv);

static void flushLogs(
	struct Queue* const logs);

int main(
	int argc,
	char** argv)
{
	// [PHASE 1]
	// 
	// DESCRIPTION:
	//     Phase 1 consists of parsing the command-line arguments, configuring up flags,
	//     and setting up meta data.
	// 
	// STEPS:
	//     1. Setup flags and sources.
	//     2. Parse command-line arguments.
	//     3. Validate flags and sources and meta data.
	//     4. Validate source files.

	const char* arg0 = shift(&argc, &argv);

	if (argc <= 0)
	{
		fprintf(stderr, "[main]: error: no command-line arguments were provided!\n");
		usage(stderr, arg0);
		exit(1);
	}

	// [STEP 1] (Setup flags and sources).
	const char* outpuPath = NULL;
	struct List sources = List_create();

	// [STEP 2] (Parse command-line arguments).
	while (argc > 0)
	{
		const char* flag = shift(&argc, &argv);

		if (flag == NULL)
		{
			fprintf(stderr, "[main]: error: encountered flag was invalid (null)!\n");
			usage(stderr, arg0);
			exit(1);
		}

		if (strcmp(flag, "--output") == 0 || strcmp(flag, "-o") == 0)
		{
			if (outpuPath != NULL)
			{
				fprintf(stderr, "[main]: error: repeating --output | -o flag!\n");
				usage(stderr, arg0);
				exit(1);
			}

			if (argc <= 0)
			{
				fprintf(stderr, "[main]: error: no command-line value providded for flag `%s`!\n", flag);
				usage(stderr, arg0);
				exit(1);
			}

			flag = shift(&argc, &argv);

			if (flag == NULL)
			{
				fprintf(stderr, "[main]: error: encountered flag was invalid (null)!\n");
				usage(stderr, arg0);
				exit(1);
			}

			outpuPath = flag;
		}
		else if (strcmp(flag, "--help") == 0 || strcmp(flag, "-h") == 0)
		{
			usage(stdout, arg0);
			return 0;
		}
		else
		{
			List_push(&sources, (void*)flag);
		}
	}

	// [STEP 3] (Validate flags and sources and meta data).
	if (sources.count <= 0)
	{
		fprintf(stderr, "[main]: error: no source files were provided!\n");
		usage(stderr, arg0);
		exit(1);
	}

	if (outpuPath == NULL || (outpuPath != NULL && strlen(outpuPath) <= 0))
	{
		outpuPath = "target.asm";
	}

	// Global logs container
	struct Queue logs = Queue_create();

	// [STEP 4] (Validate source files).
	signed long long nonExistingFilesCount = 0;

	for (struct LNode* iterator = sources.front; iterator != NULL; iterator = iterator->next)
	{
		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The data of the iterator node, in the list must never be of value
		//        null.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(iterator->data != NULL);

		const char* source = (const char*)iterator->data;

		struct stat buffer = {0};
		signed int exist = stat(source, &buffer);

		if (exist != 0)
		{
			Queue_enqueue(&logs, Log_create("main", SEVERITY_ERROR, INVALID_LOCATION, "file `%s` does not exist!", source));
			++nonExistingFilesCount;
		}
	}

	if (nonExistingFilesCount > 0)
	{
		// NOTE: flushing logs AND destroying the logs queue!
		flushLogs(&logs);
		Queue_destroy(&logs);
		usage(stderr, arg0);
		exit(1);
	}

	// [PHASE 2]
	// 
	// DESCRIPTION:
	//     Phase 2 consists of preprocessing, lexing, parsing, validating, optimizing, and
	//     translating the source code.
	// 
	// STEPS:
	//     1. Running the preprocessor.
	//     2. Running the lexer and checking the tokens count.
	//     3. Running the parser and building procedures using tokens and checking the
	//        procedures count.
	//     4. Running the validator and type check procedures and control-flow.
	//     5. Running the optimizer.
	//     6. Running the translator.
	//     7. Print everything in the logs queue and destroy it.
	//     8. Cleanup tokens and procedures.
	// 
	// NOTES:
	//     1. Logs queue must be empty.
	//     2. Sources list must NOT be empty.

	for (struct LNode* sourcesIterator = sources.front; sourcesIterator != NULL; sourcesIterator = sourcesIterator->next)
	{
		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The data of the sources iterator node, in the list must never be
		//        of value null, since the sources were already checked.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(sourcesIterator->data != NULL);

		// Setup everything here
		const char* source = (const char*)sourcesIterator->data;
		struct List tokens = List_create();
		struct Globals globals = Globals_create();

		// [STEP 1] (Running the preprocessor).
		// TODO: run the preprocessor!

		// [STEP 2] (Running the lexer and checking the tokens count).

		// NOTE: in case tokens list will be empty, the lexer will go to cleanup and dump this source file.
		//       Having said that, there is no need to handle tokens list's count anywhere (maybe apart having)
		//       some asserts everywhere for bug-catching).
		if (!Lexer_lexFile(source, &tokens, &logs))
		{
			goto cleanup;
		}
		else
		{
			Queue_enqueue(&logs, Log_create("lexer", SEVERITY_SUCCESS, INVALID_LOCATION, "lexer finished successfully!"));
		}

		// [STEP 3] (Running the parser and building globals using tokens and checking
		//           the globals count).

		// NOTE: in case globals container will be empty, the parser will go to cleanup and dump this source file.
		//       Having said that, there is no need to handle tokens list's count anywhere (maybe apart having)
		//       some asserts everywhere for bug-catching).

		// TODO: implement memory and local
		// TODO: memory must only use numeric integer literal for the size!
		// TODO: implement with <identifiers> do <block <end>
		// TODO: procedures and memories are both globals, so create a Global wrapper for a union pf Procedure and Memory
		// TODO: all globals must be stored in a hash map
		// TODO: `with` needs to use hash map for the identifiers (i presume)
		// TODO: add warnings for global things that are not used: procedures, global memories
		// TODO: must implement check for multiple `main` procedures
		// TODO: must implement check overloaded procedures
		// TODO: must implement check overloaded local memories, and global memories
		// TODO: must implement check for repeating `with` identifiers
		if (!Parser_parseTokens(&globals, &tokens, &logs))
		{
			goto cleanup;
		}
		else
		{
			Queue_enqueue(&logs, Log_create("parser", SEVERITY_SUCCESS, INVALID_LOCATION, "parser finished successfully!"));
		}

		// [STEP 4] (Running the validator and type check globals and control-flow).
		// TODO:
		//     1. Refactor ALL log messages.
		if (!Validator_validateTokens(&globals, &logs))
		{
			goto cleanup;
		}
		else
		{
			Queue_enqueue(&logs, Log_create("validator", SEVERITY_SUCCESS, INVALID_LOCATION, "validator finished successfully!"));
		}

		// [STEP 5] (Running the optimizer).
		// TODO: Run the optimizer!

		// [STEP 6] (Running the translator).
		// TODO: translator should compile <original file>.asm and `outputPath` should be only final executable!
		if (!Translator_translateTokens(outpuPath, &globals, &logs))
		{
			goto cleanup;
		}
		else
		{
			Queue_enqueue(&logs, Log_create("translator", SEVERITY_SUCCESS, INVALID_LOCATION, "translator finished successfully!"));
		}

cleanup:
		// [STEP 7] (Print everything in the logs queue and destroy it).
		flushLogs(&logs);
		Queue_destroy(&logs);

		// [STEP 8] (Cleanup tokens and globals).
		Globals_destroy(&globals);

		for (struct LNode* tokensIterator = tokens.front; tokensIterator != NULL; tokensIterator = tokensIterator->next)
		{
			// NOTE: using `assert` and not `if`
			// REASONS:
			//     1. The data of the tokens iterator node, in the list must never be
			//        of value null.
			//     2. This assert will prevent developers infliced bugs and development
			//        and debug configuration.
			assert(tokensIterator->data != NULL);

			struct Token* token = (struct Token*)tokensIterator->data;
			Token_destroy(token);
		}

		List_destroy(&tokens);
	}

	List_destroy(&sources);

	return 0;
}

static void usage(
	FILE* const stream,
	const char* argv0)
{
	fprintf(stream,
		"Usage: %s [Options] sources...\n"
		"Options:\n"
		"    [ --output       | -o  ] <path>         Set output path for the target\n"
		"    [ --help         | -h  ]                Print usage message\n",
		argv0);
}

static const char* shift(
	int* const argc,
	char*** const argv)
{
	const char* current = NULL;

	if (*argc > 0 && *argv != NULL)
	{
		current = **argv;
		*argc -= 1;
		*argv += 1;
	}

	return current;
}

static void flushLogs(
	struct Queue* const logs)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The logs, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(logs != NULL);

	struct Log* log = NULL;

	while ((log = (struct Log*)Queue_dequeue(logs)) != NULL)
	{
		Log_print(log);
		Log_destroy(log);
	}
}
