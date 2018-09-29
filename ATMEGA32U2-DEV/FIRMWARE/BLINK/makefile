TARGET = BLINK
SOURCE = BLINK.c

DEVICE = atmega32u2
CC      = avr-gcc
OBJCOPY = avr-objcopy
DFUPROG = sudo dfu-programmer $(DEVICE)
CFLAGS  = -mmcu=$(DEVICE) -Os -Wall

all:
	$(CC) -mmcu=$(DEVICE) $(SOURCE) -o $(TARGET).o
	avr-objcopy -j .text -j .data -O ihex $(TARGET).o $(TARGET).hex

dfu: $(TARGET).hex
	$(DFUPROG) erase
	$(DFUPROG) flash $(DEVICE).hex
	$(DFUPROG) reset

clean:
	@rm src/*.o $(TARGET).o $(TARGET).hex

