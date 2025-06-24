# Variables
CC = gcc
AR = ar

MAIN_O_FILE = main.o
MAIN_EXEC = main
LIB_C_FILE = static_library/bridge.c
LIB_O_FILE = static_library/bridge.o
STATIC_LIB = static_library/lib_bridge.a

# Compile code with static library
make:
	$(CC) -c $(LIB_C_FILE) -o $(LIB_O_FILE)
	$(AR) rcs $(STATIC_LIB) $(LIB_O_FILE)
	$(CC) -c main.c -o $(MAIN_O_FILE)
	$(CC) -o $(MAIN_EXEC) $(MAIN_O_FILE) -L. $(STATIC_LIB)

# Run executable
run:
	sudo ./$(MAIN_EXEC)

# Clean target to remove generated files
clean:
	rm -f $(LIB_O_FILE) $(STATIC_LIB) $(MAIN_O_FILE) $(MAIN_EXEC)