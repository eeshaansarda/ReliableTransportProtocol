#ifndef __ReliableConnection_h__
#define __ReliableConnection_h__

#include "UdpSocket.h"
#include <inttypes.h>


#define SET_TIMEOUT_MS 200
// sequence number should be 0 for the first one, 0 + sizeof(Probe) for the next one
// can put all flags in one uint8_t
// but since the total sizeof(Probe) remains the same, it does not matter
//uint32_t acknowledgement;
typedef struct data {
    uint32_t sequence;
    uint8_t syn;
    uint8_t ack; // to check if ack is sent
    uint8_t fin;
    uint8_t padding;
    uint64_t timestamp;
    char message[8];
} Probe;

// Client
int establishConnection(const UdpSocket_t *l_socket, const UdpSocket_t *r_socket);
int terminateConnection(const UdpSocket_t *l_socket, const UdpSocket_t *r_socket);
int sendData(const UdpSocket_t *l_socket, const UdpSocket_t *r_socket, int finish, const char *message);

// Server
int serverListen(const UdpSocket_t *l_socket);

#endif
