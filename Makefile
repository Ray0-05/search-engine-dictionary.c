CC      = gcc
CFLAGS  = -g -O1 -Iinclude

SRC_COMMON = src/dict_common.c \
             src/read.c \
             src/record.c \
             src/bit.c

# -------- dict1 --------
SRC1 = dict1.c src/linked_list_dict.c $(SRC_COMMON)
OBJ1 = $(SRC1:%.c=obj/%.o)
EXE1 = dict1

# -------- dict2 --------
SRC2 = dict2.c src/patricia_tree_dict.c $(SRC_COMMON)
OBJ2 = $(SRC2:%.c=obj/%.o)
EXE2 = dict2

# -------- build rules --------
all: $(EXE1) $(EXE2)

$(EXE1): $(OBJ1)
	$(CC) $(OBJ1) -o $@

$(EXE2): $(OBJ2)
	$(CC) $(OBJ2) -o $@

obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj $(EXE1) $(EXE2)
