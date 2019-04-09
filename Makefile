EXEC = miniminishell
FILES = miniminishell.c commands.c
OBJECTS = $(FILES:.c=.o)
HEADERS = $(FILES:.c=.h)

all: $(EXEC)

$(EXEC): $(OBJECTS) $(HEADERS)
	gcc -o $(EXEC) $(OBJECTS)

%.h: ;
