#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/if_ether.h>

#define PAK_SIZE 2048

// Global variables
int level_2_socket;
struct sockaddr addr;

// Create raw socket
void create_socket()
{
if ((level_2_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1)
    {
    printf(" Could not create socket!\n");
    exit(0);
    }
}

void sniffer() // Receive network packet
{
int pak_size;
socklen_t addr_size = sizeof(addr);
memset(&addr, 0, sizeof(addr));
unsigned char pak_buf[PAK_SIZE];
memset(pak_buf, 0, PAK_SIZE);

while (1)
    {
    if ((pak_size = recvfrom(level_2_socket, pak_buf, PAK_SIZE, 0, &addr, &addr_size)) < 0)
        break;
    else
        {
        printf(" Caught a packet: %d %d %x %x \n",addr_size,pak_size, pak_buf[12], pak_buf[13] );
        }
    }
    
printf("Closing socket. \n");
close(level_2_socket);
exit(0);
}

void main()
{
system("clear");
printf(" Level 2 Packet Sniffer \n");
create_socket();
sniffer();
}


