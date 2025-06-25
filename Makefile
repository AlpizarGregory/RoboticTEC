# Variables
GCC = gcc
MPICC = mpicc
AR = ar
RUN = mpirun

SERVER_C_FILE = server.c
SERVER_O_FILE = server.o
SERVER_EXEC = server
CLIENT_C_FILE = client.c
CLIENT_EXEC = client
LIB_C_FILE = static_library/bridge.c
LIB_O_FILE = static_library/bridge.o
STATIC_LIB = static_library/lib_bridge.a
TXT_FILE = don_quixote.txt

# Compile code with static library
make:
	$(GCC) -c $(LIB_C_FILE) -o $(LIB_O_FILE)
	$(AR) rcs $(STATIC_LIB) $(LIB_O_FILE)
	$(MPICC) -c $(SERVER_C_FILE) -o $(SERVER_O_FILE)
	$(MPICC) -o $(SERVER_EXEC) $(SERVER_O_FILE) -L. $(STATIC_LIB)
	$(GCC) $(CLIENT_C_FILE) -o $(CLIENT_EXEC)

# Run server
serv:
	sudo $(RUN) --allow-run-as-root -np 4 ./$(SERVER_EXEC) $(TXT_FILE)

cli:
	sudo ./$(CLIENT_EXEC)

# Clean target to remove generated files
clean:
	rm -f $(LIB_O_FILE) $(STATIC_LIB) $(SERVER_O_FILE) $(SERVER_EXEC) $(CLIENT_EXEC)