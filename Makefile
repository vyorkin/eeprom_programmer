USER_LIB_DIR   := ~/Arduino/libraries
ARDUINO_DIR    := $(shell nix-build --no-out-link "<nixpkgs>" -A arduino)/share/arduino
ARDUINO_MK_DIR := $(shell nix-build --no-out-link "<nixpkgs>" -A arduino-mk)
AVRDUDE_DIR    := $(shell nix-build --no-out-link "<nixpkgs>" -A avrdude)

ARDUINO_QUIET = 1

ARCHITECTURE  = avr
BOARD_TAG     = nano
BOARD_SUB     = atmega328old

.PHONY: my
my: upload monitor clean

include $(ARDUINO_MK_DIR)/Arduino.mk
