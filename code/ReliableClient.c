#include "UdpSocket.h"
#include "ReliableConnection.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>


// States
// 0 SYN
// 1 SYN ACK
// 2 ACK
// 2 ACK

int main(int argc, char *argv[]) {
    if(argc != 2) printf("usage: client <hostname>");

    int MY_PORT = getuid();

    UdpSocket_t *l_socket, *r_socket;
    if((l_socket = setupUdpSocket_t(NULL, MY_PORT)) == NULL) {
        printf("local problem\n");
        exit(0);
    }

    if((r_socket = setupUdpSocket_t(argv[1], MY_PORT)) == NULL) {
        printf("remote problem\n");
        exit(0);
    }

    if (openUdp(l_socket) < 0) {
        printf("openUdp() problem");
        exit(0);
    }

    char *message = "hello";
    establishConnection(l_socket, r_socket);
    sendData(l_socket, r_socket, 0, message);
    terminateConnection(l_socket, r_socket);

    return 0;
}
