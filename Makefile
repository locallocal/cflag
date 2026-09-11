CLANG_FORMAT ?= clang-format
BUILD_SCRIPT := ./build.sh

SOURCE_DIRS := include example tests
SOURCES := $(shell find $(SOURCE_DIRS) -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cc' -o -name '*.cpp' \))

.PHONY: all release debug test cov clean format format-check help

all: release

release:
	$(BUILD_SCRIPT) --release

debug:
	$(BUILD_SCRIPT) --debug

test:
	$(BUILD_SCRIPT) --test

cov:
	$(BUILD_SCRIPT) --cov

clean:
	$(BUILD_SCRIPT) --clean

format:
	$(CLANG_FORMAT) -i --style=file $(SOURCES)

format-check:
	$(CLANG_FORMAT) --dry-run -Werror --style=file $(SOURCES)

help:
	@echo "Targets:"
	@echo "  release       Build release (default)"
	@echo "  debug         Build debug"
	@echo "  test          Build and run tests"
	@echo "  cov           Build, run tests and generate coverage"
	@echo "  clean         Remove build and coverage directories"
	@echo "  format        Format sources in place with clang-format"
	@echo "  format-check  Fail if any source is not formatted"
