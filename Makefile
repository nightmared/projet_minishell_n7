EXEC = minishell
FILES = minishell.c commands.c list.c process.c builtins.c signals.c
OBJECTS = $(FILES:.c=.o)
CFLAGS = -g -Wall -pedantic

all: $(EXEC)

$(EXEC): $(OBJECTS)
	gcc -o $(EXEC) $(OBJECTS)

%.o: %.c common.h commands.h list.h process.h signals.h
	gcc $(CFLAGS) -c $< -o $@
