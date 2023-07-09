#include "ReliableConnection.h"
#include "UdpSocket.h"
#include "byteorder64.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

int seq = 0;

void hton_probe(Probe *p) {
    p->sequence = htonl(p->sequence);
    p->timestamp = hton64(p->timestamp);
}

void ntoh_probe(Probe *p) {
    p->sequence = ntohl(p->sequence);
    p->timestamp = ntoh64(p->timestamp);
}

void print_probe(Probe *p) {
    printf("sequence  : %u\n", p->sequence);
    printf("timestamp : %lu\n", p->timestamp);
    printf("syn       : %i\n", p->syn);
    printf("ack       : %i\n", p->ack);
    printf("fin       : %i\n", p->fin);
    if(p->message[0] != '\0') printf("message:\n%s\n", p->message);
    printf("\n");
}

void empty_probe(Probe *p) {
    struct timespec t;
    if(clock_gettime(CLOCK_REALTIME, &t) != 0) {
        printf("clock_gettime(): error");
        exit(0);
    }

    p->sequence = 0;
    p->syn = 0;
    p->ack = 0;
    p->fin = 0;
    p->timestamp = t.tv_nsec;
    p->message[0] = '\0';
}

int sendData(const UdpSocket_t *l_socket, const UdpSocket_t *r_socket, int finish, const char *message) {
    seq++;
    // set timeout
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = SET_TIMEOUT_MS * 1000;
    setsockopt(l_socket->sd, SOL_SOCKET, SO_RCVTIMEO, &tv,sizeof(tv));

    Probe *estConn = malloc(sizeof(Probe)), *in = malloc(sizeof(Probe));
    empty_probe(estConn);
    estConn->sequence = seq;
    if(message) {
        strcpy(estConn->message, message);
    } else if(finish) {
        estConn->fin = 1; // finish
    } else estConn->syn = 1;

    UdpBuffer_t buffer, r_buffer;
    buffer.bytes = (void *) estConn;
    buffer.n = sizeof(Probe) + 1111;
    r_buffer.bytes = (void *) in;
    r_buffer.n = sizeof(Probe);

    int flag = 0, count = 0;
    while(!flag) {
        hton_probe(estConn);

        if (sendUdp(l_socket, r_socket, &buffer) != buffer.n) {
            printf("sendUpd(): error\n");
            free(in);
            free(estConn);
            return -1;
        }
        printf("sendUpd(): done\n");

        ntoh_probe(estConn);

        UdpSocket_t receive;
        empty_probe(in);

        if (recvUdp(l_socket, &receive, &r_buffer) < 0) {
            //timeout
            if(count == 10) return -1;
            count++;
            seq++;
            estConn->sequence++;
        } else {
            printf("recvUpd(): done\n");
            ntoh_probe(in);

            if(in->ack != 1) {
                printf("received unacknowledged data\n");
                free(in);
                free(estConn);
                return -1;
            }
            print_probe(in);

            double ms = (double) (in->timestamp - estConn->timestamp) / 1000000;
            //printf("out: %lu\n", estConn->timestamp);
            //printf("in: %lu\n", in->timestamp);
            printf("rtt: %.6f ms\n", ms);

            flag = 1;
        }
    }
    free(in);
    free(estConn);
    return 0;
}

int establishConnection(const UdpSocket_t *l_socket, const UdpSocket_t *r_socket) {
    return sendData(l_socket, r_socket, 0, NULL);
}

int terminateConnection(const UdpSocket_t *l_socket, const UdpSocket_t *r_socket) {
    return sendData(l_socket, r_socket, 1, NULL);
}

int serverListen(const UdpSocket_t *l_socket) {
    UdpSocket_t r_socket;

    UdpBuffer_t r_buffer, s_buffer;
    Probe *in = malloc(sizeof(Probe)), *out = malloc(sizeof(Probe));

    r_buffer.bytes = (void *) in;
    r_buffer.n = sizeof(Probe);

    s_buffer.bytes = (void *) out;
    s_buffer.n = sizeof(Probe);

    printf("ready on port %d ...\n\n", ntohs(l_socket->addr.sin_port));

    int flag = 0;
    while(!flag) {
        empty_probe(in);

        if (recvUdp(l_socket, &r_socket, &r_buffer) < 0) {
            printf("recvUdp() problem\n");
        } else {
            printf("recvUpd(): done\n");
            ntoh_probe(in);

            print_probe(in);

            empty_probe(out);
            out->sequence = in->sequence;
            out->ack = 1;
            // to est connection
            if(in->syn) {
                out->syn = 1;
            }
            if(in->fin) {
                out->fin = 1;
                //flag = 1;
            }

            hton_probe(out);
            if (sendUdp(l_socket, &r_socket, &s_buffer) != s_buffer.n) {
                printf("sendUpd(): error\n");
                free(in);
                free(out);
                return -1;
            }
            printf("sendUpd(): done\n");

        }
    }
    free(in);
    free(out);
    return 0;
}

