CC = gcc
CFLAGS = -Wall -Wextra -Werror -g3
LIBS = -lncurses
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
EXEC = my_htop

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

test: all
	@valgrind ./my_htop

clean:
	rm -f $(OBJ) $(EXEC)

re: clean all
