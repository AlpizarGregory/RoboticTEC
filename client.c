#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TXT_FILE "don_quixote.txt"
#define MAX_TEXT_SIZE 3000000  // ~3MB
#define PORT 12345

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = inet_addr("127.0.0.1") 
    };

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    FILE *f = fopen(TXT_FILE, "r");
    if (!f) {
        perror("Failed to open file\n");
        exit(1);
    }

    char *buffer = malloc(MAX_TEXT_SIZE);
    size_t len = fread(buffer, 1, MAX_TEXT_SIZE, f);
    fclose(f);

    send(sock, &len, sizeof(len), 0);
    send(sock, buffer, len, 0);

    printf("Text sent to server...\n");
    free(buffer);
    close(sock);
    return 0;
}
