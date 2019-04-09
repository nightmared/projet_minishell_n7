EXEC = miniminishell
FILES = miniminishell.c commands.c
OBJECTS = $(FILES:.c=.o)
HEADERS = $(FILES:.c=.h)
CFLAGS = -Wall -pedantic

all: $(EXEC)

$(EXEC): $(OBJECTS)
	gcc -o $(EXEC) $(OBJECTS)

%.o: %.c common.h commands.h
	gcc $(CFLAGS) -c $< -o $@
