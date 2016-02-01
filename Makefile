#!Makefile

C_SRC = $(shell find -name *.c | sort)
C_OBJ = $(patsubst %.c, %.o, $(C_SRC))
S_SRC = $(shell find -name *.S | sort)
S_OBJ = $(patsubst %.S, %.o, $(S_SRC))

BIN = bitch

CC = gcc
LD = ld

C_FLAGS = -O -c -Wall -m32 -nostdinc -fno-builtin -fno-stack-protector -I include
LD_FLAGS = -T arch/x86/x86.ld -m elf_i386 -nostdlib

QEMU = qemu-system-x86_64

//C_FLAGS += -DDEBUG

all: link

.S.o:
	$(CC) $(C_FLAGS) $< -o $@

.c.o:
	$(CC) $(C_FLAGS) $< -o $@

link: $(S_OBJ) $(C_OBJ)
	$(LD) $(LD_FLAGS) $(S_OBJ) $(C_OBJ) -o $(BIN)

clean:
	$(RM) $(S_OBJ) $(C_OBJ) $(BIN)

qemu: link
	$(QEMU) --kernel $(BIN)
