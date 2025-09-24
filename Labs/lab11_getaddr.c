#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Takes sockaddr but actually expects sockaddr_in
void print_inet4_addr(const struct sockaddr *a) {
  if (a->sa_family != AF_INET) {
    fprintf(stderr, "non-IPv4 address\n");
    exit(1);
  }
  const struct sockaddr_in *a4 = (const struct sockaddr_in *)a;
  char buf[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, & a4->sin_addr, buf, INET_ADDRSTRLEN);
  printf("%s\n", buf);
}

int main(int argc, char *argv[]) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_protocol = 0;
    hints.ai_flags = 0;

    struct addrinfo *result;
    
    for (int i = 0; i < argc; i++) {
        int num = getaddrinfo(argv[1 + i], "http", &hints, &result);
        if (num != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(num));
            exit(1);
        }
        
        for (struct addrinfo *p = result; p != NULL; p=p->ai_next) { 
            void *addr;
            struct sockaddr_in *ipv4 = (struct sockaddr_in*)p->ai_addr;
            addr = &(ipv4->sin_addr);
            
            char ipstr[INET_ADDRSTRLEN];
            inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));

            if (strcmp(ipstr, "127.0.0.1") == 0) {
                continue;
            }

            print_inet4_addr(p->ai_addr);
        } 
    }

    freeaddrinfo(result);
    
  // Use getaddrinfo() to find the IPv4 addresses of domain/host name argv[1].
  // For simplicity, you may assume argc>=2 so argv[1] exists.
  //
  // Set the service parameter to the string "80" or "http", meaning port number
  // is 80.
  //
  // Set up the hint parameter to request IPv4-only and stream-only.
  //
  // For each IPv4 address provided by getaddrinfo() (traverse the whole linked list),
  // use print_inet4_addr() above to print the IPv4 address in dot notation.
  //
  // Lastly, use freeaddrinfo() to free the linked list.
  //
  // Optional: If getaddrinfo() returns a non-zero number, use gai_strerror()
  // for printing out the error message to stderr, then exit.

  return 0;
}
