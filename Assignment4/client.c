#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {exit (1);}
    char* ip_addr = argv[1];
    int port = atoi(argv[2]);

    int sockfd;
    struct sockaddr_in serv_addr;
    socklen_t addrlen = sizeof(serv_addr);

    // creates a new socket of a given type, returning a file desciptor referring to the socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {perror("socket failed"); exit(1);}

    memset(&serv_addr, 0, addrlen);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // converts IP adress from text to binary form and stores it in serv_addr.sin_addr
    if (inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) <= 0) {perror("inet_pton failed"); close(sockfd); exit(1);}

    // establishes a connection to a remote socket at a given address
    if (connect(sockfd, (struct sockaddr*)&serv_addr, addrlen) < 0) {
        perror("connect failed"); close(sockfd); exit(1);}

    printf("Ready\n");

    char buf[32];
    char receive_buf[12];
    ssize_t bytes_read;
    ssize_t bytes_received;
    memset(buf, '\0', 31);

    while (1) {	
	bytes_read = read(STDIN_FILENO, buf, sizeof(buf) - 1);	

        if (bytes_read < 0) {close(sockfd); exit(1);} // reading failed
	else if (bytes_read <= 1) break; // read EOF or newline
        buf[bytes_read] = '\0';

	size_t len = strlen(buf);

        // passing the information to server
        if (send(sockfd, buf, len, 0) <= 0) {perror("send failed"); exit(1);}
        
        // receiving info from the server
        if ((bytes_received = recv(sockfd, receive_buf, sizeof(receive_buf) - 1, 0)) <= 0) {
            perror("recv failed"); close(sockfd); exit(1);}
	
        if (receive_buf[0] == '\n') break; // blank line
       
	receive_buf[bytes_received] = '\0';

        // if after receiving 11 bytes from the server still no newline
        if (strchr(receive_buf, '\n') == NULL) {
	    close(sockfd); exit(1);}
        
        printf("%s\n", receive_buf);
    }

    close(sockfd);
    exit(0);
}



