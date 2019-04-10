EXEC = minishell
FILES = minishell.c commands.c
OBJECTS = $(FILES:.c=.o)
CFLAGS = -g -Wall -pedantic

all: $(EXEC)

$(EXEC): $(OBJECTS)
	gcc -o $(EXEC) $(OBJECTS)

%.o: %.c common.h commands.h
	gcc $(CFLAGS) -c $< -o $@
