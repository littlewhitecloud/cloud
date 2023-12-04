CFLAGS += -Wall -Wextra -Wpedantic -Werror=switch -Werror=incompatible-pointer-types -std=c11 -O3
CFLAGS += -std=c11
CFLAGS += -g

SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=obj/%.o)

ifneq (,$(findstring Windows,$(OS)))
	include Makefile_windows
else
	include Makefile_posix
endif