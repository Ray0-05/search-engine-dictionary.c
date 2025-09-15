CC      = gcc
CFLAGS  = -g -O1 -Iinclude

SRC = dict1.c \
      src/linked_list_dict.c \
      src/dict_common.c \
      src/read.c \
      src/record.c \
      src/bit.c \

OBJ = $(SRC:%.c=obj/%.o)
EXE = dict1

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@

obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj $(EXE)

