#include <stc12.h>
#include <serial_IO.h>
#include <stdio.h>
#include "cc1101.h"
#include <string.h>
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
  __data unsigned char i,err_no;
  __code char const key[]="Hello World 123 456 This is a key";        //Remember keylen <= 64!!!
  __xdata unsigned char message[62]; 
  port_init();
  UartInit();
  spi_init();
  EA  = 1;
  putchar(0);
  cc1101_init();
  memset( message, 0, sizeof(message) );
  while (1) { // BEGIN MAIN LOOP
    
    
    printf("Waiting Request...\n");
    err_no = get_request(message,key,sizeof(key),50);
    #ifdef DEBUG
    for (i=0;i<sizeof(message);++i) { dprintf(" %02x",message[i]); }
    putchar('\r');
    putchar('\n');
    #endif
    if ( err_no ) {
      if (err_no>1) printf("failed to verify request. Start over\n");
      continue;
    }
    printf("Got request, and verified request.\n");
    
    
    message[1] = 'W';
    message[2] = 'R';
    message[3] = 'U';
    send_signed_message(message,key,sizeof(key));
    
    printf("Sent response, wait auth info\n");
    err_no = get_response(message,key,sizeof(key),50);
    #ifdef DEBUG
    for (i=0;i<sizeof(spi_buffer);++i) { dprintf(" %02x",spi_buffer[i]); }
    putchar('\r');
    putchar('\n');
    #endif
    if ( err_no ) {
      if (err_no>1) printf("failed to verify auth. Start over\n");
      continue;
    }
    printf("Got auth, and verified. Open the door...\n");
    printf("Send confirm...\n");
    
    message[1] = 'A';
    message[2] = 'C';
    message[3] = 'K';
    send_signed_message(message,key,sizeof(key));
    
    printf("Sent confirm\n");

  }  // END MAIN LOOP
}


//======================init code==================
void port_init(void) {
  //P1M0 = 0b10100000;
  //P1M1 = 0b11111111;
  P1ASF |= 0x01;
}
