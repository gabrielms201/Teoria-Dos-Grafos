PROGRAM_NAME = Grafos
SRC_DIR = ./
OBJ_DIR = bin
CFLAGS = -std=c89 -lc -ansi

output: $(OBJ_DIR)/$(PROGRAM_NAME)
	@echo "Finished Compiling"

$(OBJ_DIR)/$(PROGRAM_NAME): $(OBJ_DIR)/$(PROGRAM_NAME).o
	gcc $(OBJ_DIR)/$(PROGRAM_NAME).o -o $(OBJ_DIR)/$(PROGRAM_NAME) $(CFLAGS)

$(OBJ_DIR)/$(PROGRAM_NAME).o: $(SRC_DIR)/$(PROGRAM_NAME).c
	gcc -c $(SRC_DIR)/$(PROGRAM_NAME).c -o $(OBJ_DIR)/$(PROGRAM_NAME).o -$(CFLAGS)

clean: 
	rm -f $(OBJ_DIR)/*.o $(OBJ_DIR)/$(PROGRAM_NAME)
