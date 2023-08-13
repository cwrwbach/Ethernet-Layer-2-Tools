#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>    /* Must precede if*.h */
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <arpa/inet.h> //htons
#include <unistd.h> //close

#define MAX_PAYLOAD 1408 //(11*128)

//union ethframe
//{
//struct
//    {
//    struct ethhdr    xxheader;
//    unsigned char    xxdata[ETH_DATA_LEN];
//    } field;
//};

unsigned char frame_direct_buffer[ETH_FRAME_LEN];


//---

int sd; //socket descriptor
char *iface = "enp1s0";
unsigned char dest[ETH_ALEN];

//unsigned short proto; // = 0x88b5;

//unsigned char payload[MAX_PAYLOAD];
//unsigned short payload_len = 64; //strlen(data); //FIXME
//union ethframe frame;

struct sockaddr_ll saddrll;
struct ifreq if_buffer;
int ifindex;
unsigned char source[ETH_ALEN];
unsigned int frame_len;

//---

int network_setup()
{
if ((sd = socket(AF_PACKET, SOCK_RAW, 0 )) <0) //htons(proto))) < 0) 
    {
    printf("Error: could not open socket\n");
    return -1;
    }

memset(&if_buffer, 0x00, sizeof(if_buffer));
strncpy(if_buffer.ifr_name, iface, IFNAMSIZ);

if (ioctl(sd, SIOCGIFINDEX, &if_buffer) < 0) 
    {
    printf("Error: could not get interface index\n");
    close(sd);
    return -1;
    }
ifindex = if_buffer.ifr_ifindex;
 
if (ioctl(sd, SIOCGIFHWADDR, &if_buffer) < 0) 
    {
    printf("Error: could not get interface address\n");
    close(sd);
    return -1;
    }

//set source address
memcpy((void*)source, (void*)(if_buffer.ifr_hwaddr.sa_data),ETH_ALEN); //KEEP THIS FOR NOW but do direct later ???
//set dest address
//memset((void*)dest,0xff,6); //braodcast

//printf source mac address
printf("Source MAC address: ");
for(int k = 0; k !=ETH_ALEN; k++)
    printf("%x:",source[k]);
printf(" \n");

frame_len = ETH_FRAME_LEN; //payload_len + ETH_HLEN;
  
memset((void*)&saddrll, 0, sizeof(saddrll));
saddrll.sll_family = PF_PACKET;   
saddrll.sll_ifindex = ifindex;
saddrll.sll_halen = ETH_ALEN;
memcpy((void*)(saddrll.sll_addr), (void*)dest, ETH_ALEN);

//destination MAC
for(int i=0;i<6;i++)
    frame_direct_buffer[i] = 0xff; //Destination = Broadcast 

//source MAC
for(int i=0;i<6;i++)
    frame_direct_buffer[i+6] = source[i];

//The protocol word
frame_direct_buffer[12] = 0x88;
frame_direct_buffer[13] = 0xb5;
return 0;
}

//---

int main()
{
char debug;
network_setup();

//the user data 
for(int x=0;x<256;x++)
    frame_direct_buffer[x+14] = x;

//send test packet in loop
while(1)
    {
    frame_direct_buffer[14] = debug++;//just a test
    if (sendto(sd, frame_direct_buffer, frame_len, 0,
             (struct sockaddr*)&saddrll, sizeof(saddrll)) > 0)
    printf("Frame sent\n");
    else
        printf("Error, could not send\n");
    sleep(1);
    }
close(sd);
return 0;
}
