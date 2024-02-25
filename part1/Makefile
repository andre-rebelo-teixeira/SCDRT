.PHONY: clean flash

RPI_STORAGE :=  /home/andret/mnt/RPI-RP2/

SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build
SRCS := $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/*.c)
HEADERS := $(wildcard $(INCLUDE_DIR)/*.hpp $(INCLUDE_DIR)/*.h)


TARGET := scdtr_part1
TYPE := uf2

all: build

clean:
	rm -rf ./$(BUILD_DIR)

build: CMakeLists.txt $(SRCS) $(HEADERS)
	mkdir -p $(BUILD_DIR)
	@echo "Entering build directory"
	cd $(BUILD_DIR) && cmake -DTARGET=$(TARGET) -DSRCS="$(SRCS)" -DINCLUDE_DIR=$(INCLUDE_DIR) ..
	make -C $(BUILD_DIR)
	@echo "Leaving build directory"

flash: build
	@if [ -d $(RPI_STORAGE) ]; then \
		echo "Flashing rpi pico..."; \
		cp $(BUILD_DIR)/$(TARGET).$(TYPE) $(RPI_STORAGE) && echo "Done" || echo "Failed to flash"; \
	else \
		echo "Flash directory not found"; \
	fi
