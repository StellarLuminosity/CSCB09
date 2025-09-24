#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <signal.h>
#include "record.h"

int get_sunspots(FILE *f, const char *name, unsigned short *psunspots) {
    if (name == NULL || psunspots == NULL || f == NULL) {return 0;}

    record customer;

    if (fseek(f, 0, SEEK_SET) != 0) {return 0;}

    while (fread(&customer, sizeof(record), 1, f) == 1) {
        if (strncmp(customer.name, name, strlen(name)) == 0) {
            *psunspots = customer.sunspots;
            return 1;
        }
    }

    return 0;
}

void sunspots(int sockfd, char *filename) {
    char buf[32]; // buffer to receive client input
    char sunspots_str[12]; // buffer to hold the string representation of sunspots
    ssize_t received_len;
    unsigned short sunspots = 0;

    while(1) {
        received_len = recv(sockfd, buf, sizeof(buf) - 1, 0);
	if (received_len <= 0) {perror("recv failed"); break;} // received error or EOF
        if (received_len == 1) {break;} // received \n
	
	// after 30 bytes no newline received
	if (strchr(receive_buf, '\n') == NULL) {break;}

        buf[received_len] = '\0';

	// remove newline character from input
	size_t len = strlen(buf);
	if (buf[len - 1] == '\n') {
	    buf[len - 1] = '\0';
	    len --;
	}
        
        FILE *client_file;
        if ((client_file = fopen(filename, "r")) == NULL) {
            perror("fopen failed"); close(sockfd); exit(1);}

        int found = get_sunspots(client_file, buf, &sunspots);
	fclose(client_file);

        if (found == 1) {
	    snprintf(sunspots_str, sizeof(sunspots_str), "%u\n", sunspots);
	    if (send(sockfd, sunspots_str, strlen(sunspots_str), 0) <= 0) {
                perror("send failed"); break;} // received error or EOF
        }
        else {
            if (send(sockfd, "none\n", strlen("none\n"), 0) <= 0) {
                perror("send failed"); break;}
        }
    }

    return;
}


int main(int argc, char* argv[]) {
    if (argc != 3) {exit(1);}

    int server_port = atoi(argv[1]);
    int backlog = 5;

    int sockfd;
    int newsockfd;
 
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {perror("socket failed"); exit(1);}

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // associates a socket to a local address
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed"); close(sockfd); exit(1);}

    // marks the socket as willing to accept incoming connections
    if (listen(sockfd, backlog) < 0) {perror("listen failed"); close(sockfd); exit(1);}

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    while (1) {
    	if ((newsockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len)) < 0) {
        perror("accept failed"); close(sockfd); exit(1);}

	pid_t pid;
        if ((pid = fork()) < 0) {perror("fork failed"); close(newsockfd); close(sockfd); exit(1);

	if (pid == 0) {
	    // child process
	    close(sockfd); 
	    sunspots(newsockfd, argv[2]);

	    close(newsockfd);
	    exit(0);}
	else {
	    // parent process
	    close(newsockfd);}
    }
}

