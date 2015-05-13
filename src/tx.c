#include <stc12.h>
#include <serial_IO.h>
#include <stdio.h>
#include "debug.h"
#include "cc1101.h"
#include "polarssl/sha256.h"
#include "protocol.h"

unsigned char _sdcc_external_startup(void) { return 1; }

void main(void);
void port_init(void);

void UartInit(void)		//38400bps@11.0592MHz
{
  PCON |= 0x80;
  SCON = 0x53;
  AUXR |= 0x04;
  BRT = 0xDC;
  AUXR |= 0x01;
  AUXR |= 0x10;
}



void main(void) {
  __data unsigned char i;
  __code char const key[]="Hello World 123 456 This is a key";        //Remember keylen <= 64!!!
  __xdata unsigned char message[62]; 
  // message format:
  // 1 sequence number, sequence number means this same message will be repeated n times
  // 3 command
  // 30 local random number
  // 28 hash_224 ( command + remote_rand + local_rand )
  port_init();
  UartInit();
  spi_init();
  EA  = 1;
  putchar(0);
  cc1101_init();
  while (1) { // BEGIN MAIN LOOP
    printf("System ready, say 'S' to start\n");
    while (getchar()!='S'){};

    request_authentication(message,key,sizeof(key));
    printf("Request of Service sent, waiting for Response\n");
    
    i = get_response(message,key,sizeof(key),50);
    if (i) {
      if ( i == 1 ) {
        printf("Timed out. Start over\n");
      }
      else if ( i == 2 ) {
        #ifdef DEBUG
        printf("Failed to verify response. Start over\n");
        for (i=0;i<sizeof(spi_buffer);++i) { dprintf(" %02X",spi_buffer[i]); }
        dprintf("\r\n");
        #endif
      }
      continue;
    }
    
    
    printf("Got response, and verified response.\n");
    #ifdef DEBUG
    for (i=0;i<sizeof(spi_buffer);++i) { dprintf(" %02X",spi_buffer[i]); }
    #endif
    dprintf("\r\n");

    message[1] = 'O';
    message[2] = 'P';
    message[3] = 'N';
    send_signed_message(message,key,sizeof(key));

    printf("Request to open sent, waiting for Confirm\r\n");

    if (get_response(message,key,sizeof(key),50) != 0 ) {
      #ifdef DEBUG
      printf("Timed out, or failed to verify confirm. Start over\r\n");
      for (i=0;i<sizeof(spi_buffer);++i) { dprintf(" %02x",spi_buffer[i]); }
      dprintf("\r\n");
      #endif
      continue;
    }
    
    #ifdef DEBUG
    printf("Got confirm, and verified confirm.\n");
    for (i=0;i<sizeof(spi_buffer);++i) { dprintf(" %02x",spi_buffer[i]); }
    dprintf("\r\n");
    #endif
    P3_7 = 0;
  } // END MAIN LOOP
}

//======================init code=================
void port_init(void) {
  //P1M0 = 0b10100000;
  //P1M1 = 0b11111111;
  P1ASF |= 0x01;
}

