##
## Author......: Matt Weir
## Note........: Parts of this Makefile were copied from Hashcat and other sources
## License.....: GPL-2
##

DEBUG                   := 0
PRODUCTION              := 0

##
## Detect Operating System
##

UNAME                   := $(shell uname -s)

# we need to strip the windows version number to be able to build hashcat on cygwin hosts
UNAME                   := $(patsubst CYGWIN_NT-%,CYGWIN,$(UNAME))

# same for msys
UNAME                   := $(patsubst MSYS_NT-%,MSYS2,$(UNAME))
UNAME                   := $(patsubst MINGW32_NT-%,MSYS2,$(UNAME))
UNAME                   := $(patsubst MINGW64_NT-%,MSYS2,$(UNAME))

ifeq (,$(filter $(UNAME),Linux FreeBSD Darwin CYGWIN MSYS2))
$(error "! Your Operating System ($(UNAME)) is not supported by this Makefile")
endif

ifeq ($(DEBUG),1)
$(warning "## Detected Operating System : $(UNAME)")
endif


##
## Native compiler paths
##
CC                      := gcc

ifeq ($(UNAME),Darwin)
CC                      := clang
endif

ifeq ($(UNAME),FreeBSD)
CC                      := cc
endif

ifeq ($(DEBUG),0)
CFLAGS                  += -O2
ifneq ($(UNAME),Darwin)
LFLAGS                  += -s
endif

else
ifeq ($(DEBUG),1)
ifneq ($(UNAME),Darwin)
CFLAGS                  += -DDEBUG -Og -ggdb
else
CFLAGS                  += -DDEBUG -O0 -ggdb
endif
else
ifeq ($(DEBUG),2)
ifneq ($(UNAME),Darwin)
CFLAGS                  += -DDEBUG -Og -ggdb
else
CFLAGS                  += -DDEBUG -O0 -ggdb
endif
CFLAGS                  += -fsanitize=address -fno-omit-frame-pointer
endif
endif
endif

CFLAGS                  += -pipe -std=gnu99 -Iinclude/ -IOpenCL/

##
## Native compilation target
##

ifeq ($(UNAME),Linux)
CFLAGS_NATIVE           := $(CFLAGS)
CFLAGS_NATIVE           += -DWITH_HWMON
LFLAGS_NATIVE           := $(LFLAGS)
LFLAGS_NATIVE           += -lpthread
LFLAGS_NATIVE           += -ldl
LFLAGS_NATIVE           += -lrt
LFLAGS_NATIVE           += -lm
endif # Linux

ifeq ($(UNAME),FreeBSD)
ifndef PORTNAME
CFLAGS_NATIVE           := $(CFLAGS)
LFLAGS_NATIVE           := $(LFLAGS)
LFLAGS_NATIVE           += -lpthread
LFLAGS_NATIVE           += -lm
endif
endif # FreeBSD

ifeq ($(UNAME),Darwin)
export MACOSX_DEPLOYMENT_TARGET=10.9
CFLAGS_NATIVE           := $(CFLAGS)

ifeq ($(shell test $(PROD_VERS) -le 11; echo $$?), 0)
CFLAGS_NATIVE           += -DMISSING_CLOCK_GETTIME
endif

LFLAGS_NATIVE           := $(LFLAGS)
LFLAGS_NATIVE           += -framework OpenCL
LFLAGS_NATIVE           += -lpthread
LFLAGS_NATIVE           += -liconv
endif # Darwin

ifeq ($(UNAME),CYGWIN)
CFLAGS_NATIVE           := $(CFLAGS)
CFLAGS_NATIVE           += -DWITH_HWMON
LFLAGS_NATIVE           := $(LFLAGS)
LFLAGS_NATIVE           += -Wl,--dynamicbase
LFLAGS_NATIVE           += -Wl,--nxcompat
LFLAGS_NATIVE           += -lpsapi
LFLAGS_NATIVE           += -liconv
endif # CYGWIN

ifeq ($(UNAME),MSYS2)
CFLAGS_NATIVE           := $(CFLAGS)
CFLAGS_NATIVE           += -DWITH_HWMON
LFLAGS_NATIVE           := $(LFLAGS)
LFLAGS_NATIVE           += -Wl,--dynamicbase
LFLAGS_NATIVE           += -Wl,--nxcompat
LFLAGS_NATIVE           += -lpsapi
LFLAGS_NATIVE           += -liconv
LFLAGS_NATIVE           += -lws2_32
endif # MSYS2


pcfg_guesser: src/pcfg_guesser.o src/command_line.o src/tty.o src/banner_info.o src/grammar_io.a
	$(CC) $(CFLAGS_NATIVE) src/pcfg_guesser.o src/tty.o src/command_line.o src/banner_info.o src/grammar_io.a -O3 -o pcfg_guesser
	
pcfg_guesser.o: src/pcfg_guesser.c src/pcfg_guesser.h
	$(CC) $(CFLAGS_NATIVE) -c src/pcfg_guesser.c
	
tty.o: src/tty.c src/tty.h
	$(CC) $(CFLAGS_NATIVE) -c src/tty.c
	
command_line.o: src/command_line.c src/command_line.h
	$(CC) $(CFLAGS_NATIVE) -c src/command_line.c
    
banner_info.o: src/banner_info.c src/bannar_info.h
	$(CC) $(CFLAGS_NATIVE) -c src/banner_info.c

src/grammar_io.a: src/grammar_io.o src/config_parser.o
	ar rcs src/grammar_io.a src/grammar_io.o src/config_parser.o

grammar_io.o: src/grammar_io.c src/grammar_io.h
	$(CC) $(CFLAGS_NATIVE) -c src/grammar_io.c 

config_parser.o: src/config_parser.c src/config_parser.h
	$(CC) $(CFLAGS_NATIVE) -c src/config_parser.c

main: pcfg_guesser

clean:
	rm -f pcfg_guesser 
	rm -f src/*.o
	rm -f src/*.a
