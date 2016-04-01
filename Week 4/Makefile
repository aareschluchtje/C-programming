TARGET	= ipac

# Application source en include includes
SRC_DIR	= .
INC_DIR = ./lib/

# NutOS location (includes and libs)
NUT_INC = c:/ethernut-4.3.3/nut/include
NUT_BUILD_INC = c:/ethernut-4.3.3/build/gcc/atmega2561/lib/include
NUT_LIB_DIR = c:/ethernut-4.3.3/build/gcc/atmega2561/lib

# WinAvr includes
AVR_INC = c:/winavr/avr/include


# Compiler, assembler & linker (flags)
CC		= 	avr-gcc
CFLAGS	= 	-mmcu=atmega2561 -Os -Wall -Wstrict-prototypes -DNUT_CPU_FREQ=14745600 \
			-D__HARVARD_ARCH__ -DNUTOS_VERSION=433 \
			-Wa,-ahlms=$(SRC_DIR)/$*lst
ASFLAGS = 	-mmcu=atmega2561 -I. -x assembler-with-cpp -Wa,-ahlms=$(SRC_DIR)/$*lst,-gstabs 
LDFLAGS	=	-mmcu=atmega2561 -Wl,--defsym=main=0,-Map=TIStreamer.map,--cref

# Alle source files in de huidige directory (wildcard op .c)
SRCS =	$(wildcard *.c)
OBJS = 	$(SRCS:%.c=%.o)
NUT_LIBS = $(NUT_LIB_DIR)/nutinit.o -lnutpro -lnutnet -lnutpro -lnutfs -lnutos -lnutdev -lnutarch -lnutnet -lnutcrt -lnutdev


# Alle includes (header files) in de huidige directory
INCS =	$(wildcard *.h)

# Linking rule. All *.o to elf file. Then convert to *.hex
$(TARGET):	$(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -L$(NUT_LIB_DIR) $(NUT_LIBS) -o $@.elf
	avr-objcopy -O ihex $@.elf $@.hex
#	hex2bin -ebin $@.hex

# Compiling the objs's. avr-gcc autocalls assembler	
$(SRC_DIR)/%o:	$(SRC_DIR)/%c 
	$(CC) -c $< $(CFLAGS) -I$(INC_DIR) -I$(NUT_INC) -I$(AVR_INC) -o $@

	
all: $(TARGET)

debug:
	
	@echo $(OBJS)


.PHONY: clean
clean:
	-rm -f $(OBJS)
	-rm -f $(SRCS:.c=.lst)
	-rm -f *.hex *.elf *.map *.bin

