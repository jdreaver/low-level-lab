.PHONY: configure
configure: ## Set up CMake build directories
	cmake -S . -B build

.PHONY: build
build: ## Run cmake build (requires configuration first)
	cmake --build build/

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
