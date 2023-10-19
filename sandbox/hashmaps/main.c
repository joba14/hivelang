
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct DNode
{
	const char* key;
	void* value;
	struct DNode* next;
};

struct Dict
{
	int64_t capacity;
	struct DNode** list;
};

struct Dict Dict_create(
	int64_t capacity)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The capacity, provided to this function, must never ever have a
	//        negative value.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(capacity >= 0);

	struct Dict dict = {0};
	dict.capacity = capacity;

	dict.list = (struct DNode**)malloc(capacity * sizeof(struct DNode*));

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The memory allocation errors can happen anytime, no matter build
	//        configuration being debug or release. However, since the compiler
	//        cannot prevent such bugs, I will leave it as assert. Worst case
	//        scenario - the compiler crashes, and user re-runs it.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(dict.list != NULL);

	for (int64_t i = 0; i < capacity; ++i)
	{
		dict.list[i] = NULL;
	}

	return dict;
}

uint64_t Dict_hash(
	struct Dict* const dict,
	const char* key,
	const int64_t length)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The dict, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(dict != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The key, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(key != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The length, provided to this function, must never ever be negative.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(length >= 0);

	uint64_t hash = 5381;

	for (int64_t i = 0; i < length; ++i)
		hash = ((hash << 5) + hash) + key[i];

	printf("POST-HASH\n");
	getchar();

	return hash;
}

void Dict_insert(
	struct Dict* const dict,
	const char* key,
	const int64_t length,
	void* value)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The dict, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(dict != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The key, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(key != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The length, provided to this function, must never ever be negative.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(length >= 0);

	printf("PRE-HASH\n");
	getchar();

	const uint64_t index = Dict_hash(dict, key, length);

	printf("PRE-INDEX\n");
	getchar();

	struct DNode* list = dict->list[index];
	struct DNode* temp = list;

	printf("PRE-MALLOC\n");
	getchar();

	struct DNode* newNode = (struct DNode*)malloc(sizeof(struct DNode));

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The memory allocation errors can happen anytime, no matter build
	//        configuration being debug or release. However, since the compiler
	//        cannot prevent such bugs, I will leave it as assert. Worst case
	//        scenario - the compiler crashes, and user re-runs it.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(newNode != NULL);

	printf("PRE-LOOP\n");
	getchar();

	while (temp)
	{
		if (strncmp(temp->key, key, length) == 0)
		{
			temp->value = value;
			return;
		}

		temp = temp->next;
	}

	printf("POST-LOOP\n");
	getchar();

	newNode->key = key;
	newNode->value = value;
	newNode->next = list;
	dict->list[index] = newNode;
}

signed char Dict_exists(
	struct Dict* const dict,
	signed char* key,
	const int64_t length)
{
	const uint64_t index = Dict_hash(dict, key, length);

	struct DNode* list = dict->list[index];
	struct DNode* temp = list;

	while (temp)
	{
		if (strncmp(temp->key, key, length) == 0)
		{
			return 1;
		}

		temp = temp->next;
	}

	return 0;
}

int main()
{
	static int a = 4, b = 5;
	struct Dict dict = Dict_create(1000000000);
	printf("1\n");
	Dict_insert(&dict, "1", 1, &a);
	printf("2\n");
	Dict_insert(&dict, "2", 1, &b);
	printf("3\n");

	printf("%d\n", (signed int)Dict_exists(&dict, "1", 1));
	return 0;
}
