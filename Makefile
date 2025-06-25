# Variables
CC = mpicc
AR = ar
RUN = mpirun

MAIN_C_FILE = cluster.c
MAIN_O_FILE = cluster.o
MAIN_EXEC = cluster
LIB_C_FILE = static_library/bridge.c
LIB_O_FILE = static_library/bridge.o
STATIC_LIB = static_library/lib_bridge.a
TXT_FILE = don_quixote.txt

# Compile code with static library
make:
	$(CC) -c $(LIB_C_FILE) -o $(LIB_O_FILE)
	$(AR) rcs $(STATIC_LIB) $(LIB_O_FILE)
	$(CC) -c $(MAIN_C_FILE) -o $(MAIN_O_FILE)
	$(CC) -o $(MAIN_EXEC) $(MAIN_O_FILE) -L. $(STATIC_LIB)

# Run executable
run:
	sudo $(RUN) --allow-run-as-root -np 4 ./$(MAIN_EXEC) $(TXT_FILE)

# Clean target to remove generated files
clean:
	rm -f $(LIB_O_FILE) $(STATIC_LIB) $(MAIN_O_FILE) $(MAIN_EXEC)