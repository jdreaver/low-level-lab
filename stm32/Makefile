BUILD_CONFIGS ?= Release Debug
BUILD_DIRS ?= $(addprefix build/,$(BUILD_CONFIGS))

.PHONY: configure
configure: clean $(BUILD_DIRS) ## Set up CMake build directories. Limit to one config w/ e.g. BUILD_CONFIGS=Release

$(BUILD_DIRS): build/%:
	cmake -S . -B $@ -DCMAKE_BUILD_TYPE=$(@F)

BUILD_TARGETS := $(addprefix build_,$(BUILD_DIRS))

.PHONY: build
build: $(BUILD_TARGETS) ## Run cmake build (might require "make configure" first). Limit to one config w/ e.g. BUILD_CONFIGS=Release

$(BUILD_TARGETS): build_%: $(BUILD_DIRS)
	cmake --build $* ${BUILD_ARGS}

.PHONY: clean
clean: ## Remove CMake build directories
	rm -rf build/

.PHONY: size
size: ## Show size of all built .elf files
	@find build/ -type f -name '*.elf' -exec arm-none-eabi-size {} +

ifeq ($(MAKECMDGOALS),flash)
ifeq ($(FLASH_TARGET),)
  $(error "FLASH_TARGET is empty!")
endif
endif

.PHONY: flash
flash: $(FLASH_TARGET)
	sudo st-flash write $? 0x8000000
	sudo st-flash reset

.PHONY: erase
erase:
	sudo st-flash erase

# Taken from https://marmelab.com/blog/2016/02/29/auto-documented-makefile.html
.DEFAULT_GOAL := help
.PHONY: help
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
