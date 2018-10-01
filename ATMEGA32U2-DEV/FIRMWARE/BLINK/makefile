# Makefile for the ATmega32U2 Development

TARGET = BLINK
SOURCE = BLINK.c

DEVICE  = atmega32u2
CC      = avr-gcc
OBJCOPY = avr-objcopy
DFUPROG = sudo dfu-programmer $(DEVICE)

all:
	$(CC) -Os -mmcu=$(DEVICE) $(SOURCE) -o $(TARGET).o
	avr-objcopy -j .text -j .data -O ihex $(TARGET).o $(TARGET).hex

dfu:    
	$(CC) -Os -mmcu=$(DEVICE) $(SOURCE) -o $(TARGET).o
	avr-objcopy -j .text -j .data -O ihex $(TARGET).o $(TARGET).hex
	$(DFUPROG) erase
	$(DFUPROG) flash $(DEVICE).hex
	$(DFUPROG) reset

clean:
	@rm src/*.o $(TARGET).o $(TARGET).hex

