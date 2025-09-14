CC      = gcc
CFLAGS  = -g -O1 -Iinclude

SRC = driver.c \
      src/dict_linkedlist.c \
      src/list.c \
      src/parser.c \
      src/record.c \
      src/bit.c \
      src/io.c

OBJ = $(SRC:%.c=obj/%.o)
EXE = dict1

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@

obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj $(EXE)

