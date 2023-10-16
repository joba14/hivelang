################################################################################
#### Variables and settings
################################################################################

# Executable name
EXEC = hivec

# Build, bin, assets, and install directories (bin and build root directories are kept for clean)
BUILD_DIR_ROOT = ./build
BIN_DIR_ROOT = ./bin
ASSETS_DIR = ./stdlib
ASSETS_OS_DIR := $(ASSETS_DIR)_os
INSTALL_DIR := ~/Desktop/$(EXEC)

# Sources (searches recursively inside the source directory)
SRC_DIR = ./source
SRCS := $(sort $(shell find $(SRC_DIR) -name '*.c'))

# Includes
INCLUDE_DIR = ./include
INCLUDES := -I$(INCLUDE_DIR)

# C preprocessor settings
CPPFLAGS = $(INCLUDES) -MMD -MP

# C compiler settings
CC = gcc
CFLAGS = -std=gnu11
WARNINGS = -Wall -Wextra -Wpedantic -Werror -Wshadow

# Linker flags
LDFLAGS =

# Libraries to link
LDLIBS =

# Target OS detection
ifeq ($(OS),Windows_NT) # OS is a preexisting environment variable on Windows
	OS = windows
else
	UNAME := $(shell uname -s)
	ifeq ($(UNAME),Darwin)
		OS = macos
	else ifeq ($(UNAME),Linux)
		OS = linux
	else
		$(error OS not supported by this Makefile)
	endif
endif

# OS-specific settings
ifeq ($(OS),windows)
	# Link libgcc and libstdc++ statically on Windows
	LDFLAGS += -static-libgcc -static-libstdc++

	# Disable console output on release builds
	ifeq ($(release),1)
		LDFLAGS += -mwindows
	endif

	# Windows 32- and 64-bit common settings
	INCLUDES +=
	LDFLAGS +=
	LDLIBS +=

	ifeq ($(win32),1)
		# Windows 32-bit settings
		INCLUDES +=
		LDFLAGS +=
		LDLIBS +=
	else
		# Windows 64-bit settings
		INCLUDES +=
		LDFLAGS +=
		LDLIBS +=
	endif
else ifeq ($(OS),macos)
	# Mac-specific settings
	INCLUDES +=
	LDFLAGS +=
	LDLIBS +=
else ifeq ($(OS),linux)
	# Linux-specific settings
	INCLUDES +=
	LDFLAGS +=
	LDLIBS +=
endif

################################################################################
#### Final setup
################################################################################

# Windows-specific default settings
ifeq ($(OS),windows)
	# Add .exe extension to executable
	EXEC := $(EXEC).exe

	ifeq ($(win32),1)
		# Compile for 32-bit
		CFLAGS += -m32
	else
		# Compile for 64-bit
		CFLAGS += -m64
	endif
endif

# OS-specific build, bin, and assets directories
BUILD_DIR := $(BUILD_DIR_ROOT)/$(OS)
BIN_DIR := $(BIN_DIR_ROOT)/$(OS)
ASSETS_OS_DIR := $(ASSETS_OS_DIR)/$(OS)

ifeq ($(OS),windows)
	# Windows 32-bit
	ifeq ($(win32),1)
		BUILD_DIR := $(BUILD_DIR)32
		BIN_DIR := $(BIN_DIR)32
		ASSETS_OS_DIR := $(ASSETS_OS_DIR)32
	# Windows 64-bit
	else
		BUILD_DIR := $(BUILD_DIR)64
		BIN_DIR := $(BIN_DIR)64
		ASSETS_OS_DIR := $(ASSETS_OS_DIR)64
	endif
endif

# Debug (default) and release modes settings
ifeq ($(release),1)
	BUILD_DIR := $(BUILD_DIR)/release
	BIN_DIR := $(BIN_DIR)/release
	CFLAGS += -O3 -DHIVEC_RELEASE=1
	CPPFLAGS += -DNDEBUG
else
	BUILD_DIR := $(BUILD_DIR)/debug
	BIN_DIR := $(BIN_DIR)/debug
	CFLAGS += -O0 -g -DHIVEC_DEBUG=1
endif

# Objects and dependencies
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

################################################################################
#### Targets
################################################################################

.PHONY: all
all: $(BIN_DIR)/$(EXEC)

# Build executable
$(BIN_DIR)/$(EXEC): $(OBJS)
	@echo "Building executable: $@"
	@mkdir -p $(@D)
	@$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compile C++ source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling: $<"
	@mkdir -p $(@D)
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(WARNINGS) -c $< -o $@

# Include automatically generated dependencies
-include $(DEPS)

# Install packaged program
.PHONY: install
install: all copyassets
	@echo "Packaging program to $(INSTALL_DIR)"
	@mkdir -p $(INSTALL_DIR); cp -r $(BIN_DIR)/. $(INSTALL_DIR)

# Build and run
.PHONY: run
run: all
	@echo "Starting program: $(BIN_DIR)/$(EXEC)"
	@echo "Provide arguments in args cmd flag: $(BIN_DIR)/$(EXEC) args=\"program args\""
	@cd ./$(BIN_DIR); ./$(EXEC) $(args)

# Run the compiler with --control-flow flag and render the .dot file
.PHONY: controlflow
controlflow:
	@echo "Building control flow"
	@echo "Starting program: $(BIN_DIR)/$(EXEC)"
	@cd ./$(BIN_DIR); ./$(EXEC) --control-flow ./cf_output.dot $(args)

# Copy assets to bin directory for selected platform
.PHONY: copyassets
copyassets:
	@echo "Copying assets from $(ASSETS_DIR) and $(ASSETS_OS_DIR) to $(BIN_DIR)"
	@mkdir -p $(BIN_DIR)
	@cp -r $(ASSETS_DIR)/. $(BIN_DIR)/
	@cp -r $(ASSETS_OS_DIR)/. $(BIN_DIR)/ 2> /dev/null || :

# Clean all assets from bin directories for all platforms
.PHONY: cleanassets
cleanassets:
	@echo "Cleaning assets for all platforms"
	@find $(BIN_DIR_ROOT) -mindepth 3 ! -name $(EXEC) -delete

# Clean build and bin directories for all platforms
.PHONY: clean
clean:
	@echo "Cleaning $(BUILD_DIR_ROOT) and $(BIN_DIR_ROOT) directories"
	@$(RM) -r $(BUILD_DIR_ROOT)
	@$(RM) -r $(BIN_DIR_ROOT)

# Generate documentation with Doxygen
.PHONY: docs
docs:
	@echo "Generating documentation"
	@doxygen ./Doxyfile

# Print help information
.PHONY: help
help:
	@printf "\
	Usage: make target... [options]...\n\
	\n\
	Targets:\n\
	    all                Build executable (debug mode by default) (default target)\n\
	    install            Install packaged program to desktop (debug mode by default)\n\
	    run                Build and run executable (debug mode by default)\n\
	    copyassets         Copy assets to executable directory for selected platform and configuration\n\
	    cleanassets        Clean assets from executable directories (all platforms)\n\
	    clean              Clean build and bin directories (all platforms)\n\
	    format             Run clang-format on source code\n\
	    doc                Generate documentation with Doxygen\n\
	    help               Print this information\n\
	    printvars          Print Makefile variables for debugging\n\
	\n\
	Options:\n\
	    release=1          Run target using release configuration rather than debug\n\
	    win32=1            Build for 32-bit Windows (valid when built on Windows only)\n\
	\n\
	Note: the above options affect all, install, run, copyassets, and printvars targets\n"

# Print Makefile variables
.PHONY: printvars
printvars:
	@printf "\
	OS: \"$(OS)\"\n\
	EXEC: \"$(EXEC)\"\n\
	BUILD_DIR: \"$(BUILD_DIR)\"\n\
	BIN_DIR: \"$(BIN_DIR)\"\n\
	ASSETS_DIR: \"$(ASSETS_DIR)\"\n\
	ASSETS_OS_DIR: \"$(ASSETS_OS_DIR)\"\n\
	INSTALL_DIR: \"$(INSTALL_DIR)\"\n\
	SRC_DIR: \"$(SRC_DIR)\"\n\
	SRCS: \"$(SRCS)\"\n\
	INCLUDE_DIR: \"$(INCLUDE_DIR)\"\n\
	INCLUDES: \"$(INCLUDES)\"\n\
	CC: \"$(CC)\"\n\
	CPPFLAGS: \"$(CPPFLAGS)\"\n\
	CFLAGS: \"$(CFLAGS)\"\n\
	WARNINGS: \"$(WARNINGS)\"\n\
	LDFLAGS: \"$(LDFLAGS)\"\n\
	LDLIBS: \"$(LDLIBS)\"\n"
