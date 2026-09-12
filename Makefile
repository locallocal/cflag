CLANG_FORMAT ?= clang-format
BUILD_SCRIPT := ./build.sh
EXAMPLE_BIN := build/bin/example
ARGS ?= --help
FLAG_FILE_ARGS ?=

SOURCE_DIRS := include example tests skills
SOURCES := $(shell find $(SOURCE_DIRS) -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cc' -o -name '*.cpp' \))

.PHONY: all release debug test cov clean example example-json example-yaml example-gflags format format-check help

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

example: release
	$(EXAMPLE_BIN) $(ARGS)

example-json: release
	build/bin/flag_file_json $(FLAG_FILE_ARGS)

example-yaml: release
	build/bin/flag_file_yaml $(FLAG_FILE_ARGS)

example-gflags: release
	build/bin/flag_file_gflags $(FLAG_FILE_ARGS)

format:
	@for f in $(SOURCES); do \
		echo "$(CLANG_FORMAT) -i --style=file $$f"; \
		$(CLANG_FORMAT) -i --style=file $$f || exit 1; \
	done

format-check:
	@status=0; \
	for f in $(SOURCES); do \
		echo "$(CLANG_FORMAT) --dry-run -Werror --style=file $$f"; \
		$(CLANG_FORMAT) --dry-run -Werror --style=file $$f || status=1; \
	done; \
	exit $$status

help:
	@echo "Targets:"
	@echo "  release       Build release (default)"
	@echo "  debug         Build debug"
	@echo "  test          Build and run tests"
	@echo "  cov           Build, run tests and generate coverage"
	@echo "  clean         Remove build and coverage directories"
	@echo "  example       Build and run the example (ARGS=\"--help\")"
	@echo "  example-json  Build and run the JSON flag file example (FLAG_FILE_ARGS=\"\")"
	@echo "  example-yaml  Build and run the YAML flag file example (FLAG_FILE_ARGS=\"\")"
	@echo "  example-gflags Build and run the gflags flag file example (FLAG_FILE_ARGS=\"\")"
	@echo "  format        Format sources in place with clang-format"
	@echo "  format-check  Fail if any source is not formatted"
