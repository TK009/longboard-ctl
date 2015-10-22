# Uses this project as the makefile base https://github.com/sudar/Arduino-Makefile
# It must be installed before using this makefile



# Arduino settings
BOARD_TAG    = nano328
MONITOR_PORT = /dev/ttyUSB0
# MONITOR_BAUDRATE  = 9600 # 115200

# Libraries
ARDUINO_LIBS = Wire Arduino-MMA8452


### Do not touch - used for binaries path
CURRENT_DIR       = $(shell basename $(CURDIR))
PROJECT_DIR       = $(CURRENT_DIR)

### Path to the Arduino-Makefile directory.
ARDMK_DIR         = /usr/share/arduino

### Path to the Arduino application and resources directory.
#ARDUINO_DIR       = /usr/share/arduino

### Path to where the your project's libraries are stored.
USER_LIB_PATH    :=  $(CURDIR)/lib



### Path to the AVR tools directory such as avr-gcc, avr-g++, etc. if other than arduino IDE shipped are preferred
AVR_TOOLS_DIR     = /usr

### Path to avrdude directory if other than arduino ide version is preferred
#AVRDUDE          = /usr/bin/avrdude

### Set the C standard to be used during compilation.
CFLAGS_STD        = -std=gnu11

### Set the C++ standard to be used during compilation.
CXXFLAGS_STD      = -std=gnu++11

### Flags you might want to set for debugging purpose. Comment to stop.
CXXFLAGS         += -pedantic -Wall -Wextra -I$(USER_LIB_PATH)

### The port your board is connected to. Using an '*' tries all the ports and finds the right one.
#MONITOR_PORT      = /dev/tty.usbmodem*

### OBJDIR
### This is where you put the binaries you just compile using 'make'
OBJDIR            = $(PROJECT_DIR)/bin/$(BOARD_TAG)/$(CURRENT_DIR)

### Do not touch - the path to Arduino.mk, inside the ARDMK_DIR
include $(ARDMK_DIR)/Arduino.mk

