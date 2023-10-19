
# hivec


## Build status

**Linux (x86_64)** <br>
![#](https://img.shields.io/badge/Native-Unknown-lightgrey)<br>
**Windows 10** <br>
![#](https://img.shields.io/badge/Native-Unknown-lightgrey)<br>
**MacOS** <br>
![#](https://img.shields.io/badge/Native-Unknown-lightgrey)<br>


## Building

```console
> cd <hivec-root>
> make
```

Optionally, for changing the build configuration to release, or switch to win32, see:

```console
> make help
```

To check overall environment setup for make, see:

```console
> make printvars
```

NOTE: the compiler executable will be stored in \<hivec-root\>/bin directory

## Testing

To run tests, make sure you have python3 interpreter in your machine and use [tests script](./.run_tests.sh). To change tester settings modify [seetings.json](./tests/.settings.json)

Example:
```console
> cd <hivec-root>
> ./.run_tests.sh
```

## Using the compiler

If it is your first time using the hivec compiler, run:

```console
hivec --help
```

The overall usage of the compiler will be displayed like this:

```console
Usage: hivec [Options] sources...
Options:
    [ --output       | -o  ] <path>         Set output path for the target
    [ --help         | -h  ]                Print usage message
```

Note, to actually compile the source to binary executable, you will also need a [nasm](https://nasm.us/) compiler. The hivec compiler generates assembly code which by itself is not an executable. But, with the power of [nasm](https://nasm.us/) you will be able to compile it and have a native program built from scratch with ONLY two compilers :D..

## The hivelang syntax

NOTE: everything must be inside a procedure (function)!

### Procedures:
 - `procedure`
	```
	procedure func1 do
		// ...
	end
	```
 - `require` `return` `i64` `p64`
	```
	// NOTE: type specifiers must be listed as they are in the stack (from the top)

	procedure func1 require i64 do
		// ...
	end

	procedure func2 return p64 do
		// ...
	end

	procedure func3 require p64 return i64 do
		// ...
	end
	```

### Literals:
 - `i64`
	```hlang
	// stack: <empty>
	60
	-10
	+20
	// stack: 60 -10 20
	```
 - `p64`
	```hlang
	// stack: <empty>
	"Hello\n" // Supports escape sequences!
	// stack: 6 <pointer_to_string (p64)>
	```

### Arithmetic:
 - `add`
	```hlang
	// stack: 10 20
	10 20 add
	// stack: 30
	```
 - `subtract`
	```hlang
	// stack: 20 5
	20 5 subtract
	// stack: 15
	```
 - `multiply`
	```hlang
	// stack: 20 10
	20 10 multiply
	// stack: 200
	```
 - `divide`
	```hlang
	// stack: 30 10
	30 10 divide
	// stack: 3
	```
 - `modulus`
	```hlang
	// stack: 5 2
	5 2 modulus
	// stack: 1
	```

### Bitwise:
 - `band`
	```hlang
	// stack: 1 2
	1 2 band
	// stack: 0
	```
 - `bor`
	```hlang
	// stack: 1 2
	1 2 bor
	// stack: 3
	```
 - `bnot`
	```hlang
	// stack: -3
	-3 bnot
	// stack: 2
	```
 - `shiftl`
	```hlang
	// stack: 1 2
	1 2 shiftl
	// stack: 4
	```
 - `shiftr`
	```hlang
	// stack: 32 3
	32 3 shiftr
	// stack: 4
	```

### Binary logic:
 - `equal`
	```hlang
	// stack: 50 2
	50 2 equal
	// stack: 0
	```
 - `nequal`
	```hlang
	// stack: 50 2
	50 2 nequal
	// stack: 1
	```
 - `greater`
	```hlang
	// stack: 50 2
	50 2 greater
	// stack: 1
	```
 - `less`
	```hlang
	// stack: 50 2
	50 2 less
	// stack: 0
	```

### Intrinsics
 - `clone`
	```hlang
	// stack: 2
	2 clone
	// stack: 2 2
	```
 - `drop`
	```hlang
	// stack: 2 1
	2 1 drop
	// stack: 2
	```
 - `swap`
	```hlang
	// stack: 2 3
	2 3 swap
	// stack: 3 2
	```
 - `over`
	```hlang
	// stack: 2 3
	2 3 over
	// stack: 2 3 2
	```

### System (OS):
 - `syscall0`
	```hlang
	<id> syscall0
	```
 - `syscall1`
	```hlang
	<arg> <id> syscall1
	```
 - `syscall2`
	```hlang
	<arg2> <arg1> <id> syscall2
	```
 - `syscall3`
	```hlang
	<arg3> <arg2> <arg1> <id> syscall3
	```
 - `syscall4`
	```hlang
	<arg4> <arg3> <arg2> <arg1> <id> syscall4
	```
 - `syscall5`
	```hlang
	<arg5> <arg4> <arg3> <arg2> <arg1> <id> syscall5
	```
 - `syscall6`
	```hlang
	<arg6> <arg5> <arg4> <arg3> <arg2> <arg1> <id> syscall6
	```

### Structural keywords:
 - `if <condition> do <block> end`
	```hlang
	if <condition> do
		<block>
	end
	```
 - `if <condition> do <block> else <block> end`
	```hlang
	if <condition> do
		<block>
	else
		<block>
	end
	```
 - `while <condition> do <block> end`
	```hlang
	while <condition> do
		<block>
	end
	```
