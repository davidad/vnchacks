#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <zlib.h>

#define PORT 4092
#define FBUFZ 1000

#include "colorspaces.c"
#include "open_port.c"

const char protover[] = "RFB 003.003\n";
const char securitytype[] = {0x00, 0x00, 0x00, 0x02};
const char securitychallenge[16] = {0xaa};
const char securityresult[4] = {0};
const char name[] = "hello!";
const uint16_t width=1024, height=1024;

int main() {
  const char serverInit[] = {
    /*frame size*/   width>>8, width&0xff, height>>8, height&0xff,
    /*bpp*/ 32, /*depth*/ 24, /*big-endian*/ 0, /*true-colour*/ 1,
    /*red mask*/     0, 0xff,
    /*green mask*/   0, 0xff,
    /*blue mask*/    0, 0xff,
    /*red shift*/    0,
    /*green shift*/  8,
    /*blue shift*/  16, /*padding*/ 0,0,0,
    /*name length*/  0, 0, 0, sizeof(name)-1 };
  const char fbuf_refresh[] = {
    /*message-type*/ 0,
    /*padding*/      0,
    /*nrects*/       0, 1,
    /*xpos*/         0, 0,
    /*ypos*/         0, 0,
    /*width*/        width>>8, width&0xff,
    /*height*/       height>>8, height&0xff,
    /*encoding-type*/0, 0, 0, 16 };

  int   connfd = open_port(PORT);
  write(connfd, protover,          sizeof(protover)-1);
  write(connfd, securitytype,      sizeof(securitytype));
  write(connfd, securitychallenge, sizeof(securitychallenge));
  write(connfd, securityresult,    sizeof(securityresult));
  write(connfd, serverInit,        sizeof(serverInit));
  write(connfd, name,              sizeof(name)-1);

  z_streamp z = malloc(sizeof(z_stream));
  deflateInit(z,6);
  uint8_t* buf=malloc(FBUFZ);
  uint8_t tile[] = {0x01, 0, 0, 255}; //solid blue
  const int frame_size=sizeof(tile)*(width/64)*(height/64);
  uint8_t* frame=malloc(frame_size);
  int t;
  double h=0, c=1, l=0.5;
  while(1) {
    hcl2pix(&tile[1],h,c,l);
    h+=0.01;
    for(t=0;t<(width/64)*(height/64);t++)
      memcpy(&frame[t*sizeof(tile)],tile,sizeof(tile));
    z->next_in=frame;
    z->avail_in=frame_size;
    z->next_out=buf;
    z->avail_out=FBUFZ;
    z->total_out=0;
    deflate(z,Z_SYNC_FLUSH);
    int length = htonl(z->total_out);
    printf("h=%lf\n",h);
    write(connfd,fbuf_refresh,sizeof(fbuf_refresh));
    write(connfd,&length,4);
    write(connfd,buf,z->total_out);
    usleep(1e6/30);
  }

  unsigned char rb[1], i;
  while(1) if(read(connfd,rb,1)) fprintf(stdout,"%02x%c",rb[0],++i%8?' ':'\n');
  close(connfd);
  return 0;
}
