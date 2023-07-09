#include "UdpSocket.h"
#include "ReliableConnection.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
    if(argc != 1) printf("usage: server");

    int MY_PORT = getuid();
    UdpSocket_t *l_socket;//, *r_socket;

    if((l_socket = setupUdpSocket_t(NULL, MY_PORT)) == NULL) {
        printf("local problem\n");
        exit(0);
    }

    if (openUdp(l_socket) < 0) {
        printf("openUdp() problem");
        exit(0);
    }

    serverListen(l_socket);

    closeUdp(l_socket);
}
