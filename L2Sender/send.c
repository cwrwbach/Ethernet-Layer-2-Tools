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

#define DEST_POS 0
#define SOURCE_POS 6
#define PROTO_POS 12
#define PKT_POS 14

//Global
int sdx; //socket descriptor
struct sockaddr_ll sax_addrll;
unsigned char frame_direct_buffer[ETH_FRAME_LEN];
unsigned int frame_len;

//---

int network_setup()
{
struct ifreq ifx_buffer;
int ifindex;
char *iface = "enp1s0";
unsigned char source[ETH_ALEN];
unsigned char dest[ETH_ALEN];

if ((sdx = socket(AF_PACKET, SOCK_RAW, 0 )) <0) //htons(proto))) < 0) 
    {
    printf("Error: could not open socket\n");
    return -1;
    }

memset(&ifx_buffer, 0x00, sizeof(ifx_buffer));
strncpy(ifx_buffer.ifr_name, iface, IFNAMSIZ);

if (ioctl(sdx, SIOCGIFINDEX, &ifx_buffer) < 0) 
    {
    printf("Error: could not get interface index\n");
    close(sdx);
    return -1;
    }
ifindex = ifx_buffer.ifr_ifindex;

if (ioctl(sdx, SIOCGIFHWADDR, &ifx_buffer) < 0) 
    {
    printf("Error: could not get interface address\n");
    close(sdx);
    return -1;
    }

//set source address
memcpy((void*)source, (void*)(ifx_buffer.ifr_hwaddr.sa_data),ETH_ALEN); 

printf("Source MAC address: "); //print it
for(int k = 0; k !=ETH_ALEN; k++)
    printf("%x:",source[k]);
printf(" \n");

frame_len = ETH_FRAME_LEN;

//Setup the packet type  
memset((void*)&sax_addrll, 0, sizeof(sax_addrll));
sax_addrll.sll_family = PF_PACKET;   
sax_addrll.sll_ifindex = ifindex;
sax_addrll.sll_halen = ETH_ALEN;
memcpy((void*)(sax_addrll.sll_addr), (void*)dest, ETH_ALEN);

//destination MAC
for(int i=0;i<6;i++)
    frame_direct_buffer[DEST_POS+i] = 0xff; //Destination = Broadcast 

//source MAC
for(int i=0;i<6;i++)
    frame_direct_buffer[SOURCE_POS+i] = source[i]; 

//Protocol word
frame_direct_buffer[PROTO_POS] = 0x88; //User choosable
frame_direct_buffer[PROTO_POS+1] = 0xb5;   // This is Experiment Ethertyps 1
return 0;
}

//===

int main()
{
char debug;
unsigned char * user_pkt_data;

network_setup();

user_pkt_data = frame_direct_buffer + PKT_POS;

//put some test data in the user packet 
for(int x=0;x<256;x++)
    user_pkt_data[x] = x;

//send test packet in loop
while(1)
    {
    user_pkt_data[0] = debug++;//just a test
    if (sendto(sdx, frame_direct_buffer, frame_len, 0,
             (struct sockaddr*)&sax_addrll, sizeof(sax_addrll)) > 0)
    printf("Frame sent\n");
    else
        printf("Error, could not send\n");
    sleep(1);
    }
close(sdx);
return 0;
}



//extras--->>>



//union ethframe
//{
//struct
//    {
//    struct ethhdr    xxheader;
//    unsigned char    xxdata[ETH_DATA_LEN];
//    } field;
//};

//unsigned short proto; // = 0x88b5;

//unsigned char payload[MAX_PAYLOAD];
//unsigned short payload_len = 64; //strlen(data); //FIXME
//union ethframe frame;

