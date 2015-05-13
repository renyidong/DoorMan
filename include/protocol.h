#include <stc12.h>
#include <stdio.h>
#include "debug.h"
#include "cc1101.h"
#include "polarssl/sha256.h"

#define MESSAGE_SIZE 62

void get_rand(unsigned char rand[5]);
void request_authentication (unsigned char message[62], unsigned char * key, unsigned char klen);
void send_signed_message(unsigned char message[62], unsigned char * key,unsigned char klen);
void sign_message (unsigned char message[60], unsigned char * key, unsigned char klen);
void send_message ( unsigned char message[62],unsigned char repeat );
char get_response (unsigned char message[62], unsigned char * key, unsigned char klen, unsigned char timeout_tenms);
char verify_message(unsigned char message[62], unsigned char local_rand[30], unsigned char * key, unsigned char klen );

__xdata sha256_context ctx;
__code unsigned char initial_rand[30] = { 0,0,0,0,0,
                                          0,0,0,0,0,
                                          0,0,0,0,0,
                                          0,0,0,0,0,
                                          0,0,0,0,0,
                                          0,0,0,0,0,
                                        }; //30 zeros


char get_request (unsigned char message[62], unsigned char * key, unsigned char klen, unsigned char timeout_tenms) {
  if( cc1101_get_data(0,message,62,timeout_tenms) == 0 ) return 1;
  if( verify_message(message,initial_rand,key,klen) ) return 2;
  return 0;
}
                                    
char get_response (unsigned char message[62], unsigned char * key, unsigned char klen, unsigned char timeout_tenms) {
  if( cc1101_get_data(0,0,62,timeout_tenms) == 0 ) return 1;
  if( verify_message(spi_buffer,&(message[4]),key,klen) ) return 2;
  // verified message
  memcpy(message,spi_buffer,62);
  return 0;
}

void send_signed_message(unsigned char message[62], unsigned char * key,unsigned char klen) {
  
  #ifdef DEBUG
  __data unsigned char i;
  #endif
  
  printf("Command is %.3s\r\n",&(message[1]));
    
  sign_message(message,key,klen);
  
  #ifdef DEBUG
  dprintf("Message is:\t");
  for (i=0;i<MESSAGE_SIZE;++i) { dprintf(" %02X ",message[i]); }
  dprintf("\r\n");
  #endif
  
  send_message ( message, 10);
}

void request_authentication (unsigned char message[62], unsigned char * key, unsigned char klen) {
  #ifdef DEBUG
  __data unsigned char i;
  dprintf("Key is:\t");
  for (i=0;i<klen;++i) { dprintf("%02X ",key[i]); }
  dprintf("\r\n");
  #endif
  
  printf("Command is REQ\r\n");
  message[1] = 'R';
  message[2] = 'E';
  message[3] = 'Q';
  
  memset( &(message[4]), 0, 30 );  //initial remote rand is 0
  sign_message(message,key,klen);
  
  #ifdef DEBUG
  dprintf("Message is:\t");
  for (i=0;i<MESSAGE_SIZE;++i) { dprintf(" %02X",message[i]); }
  dprintf("\r\n");
  #endif
  
  send_message ( message, 10);
}

void send_message ( unsigned char message[62], unsigned char repeat ) {
  t1_count = 0;
  while (--repeat){
    printf("\nSend %d",repeat);
    message[0] = repeat;
    cc1101_put_data(0,message,62);
    while (t1_count) {++PCON;}
    t1_count = 10;
    TR1 = 1;
  }
  message[0] = 0;
  cc1101_put_data(0,message,MESSAGE_SIZE);
}

void sign_message (unsigned char message[62], unsigned char * key, unsigned char klen) {
  // input data:
  // 1 sequence number
  // 3 command
  // 30 rand number from remote (all zero if no remote rand is used)
  // 28 empty for hash output
  // return data:
  // 1 sequence number not changed
  // 3 command not changed
  // 30 new rand from self
  // 28 sha224_hmac ( command + remote rand + local rand )
  unsigned char i,j;
  unsigned char rand[5];
  
  sha256_init( &ctx );
  sha256_hmac_starts( &ctx, key, klen, 1 );
  printf("\nHMAC:");
  
  for (j=0;j<33;++j) printf(" %02X",message[1+j]);
  sha256_hmac_update( &ctx, &(message[1]), 33 );    //3 command + 30 remote rand
  
  for (i=4; i<34; i+=5) {                       //generate, hash and save local rand
    get_rand(rand);
    memcpy( &(message[i]), rand, 5 );
    
    for (j=0;j<5;++j) printf(" %02X",rand[j]);
    sha256_hmac_update( &ctx, rand, 5 );
  }
  printf("\r\n");
  sha256_hmac_finish( &ctx, &(message[34]) );
  sha256_free( &ctx );
}


char verify_message(unsigned char message[62], unsigned char local_rand[30], unsigned char * key, unsigned char klen ) {
  // message:
  // 1 sequence number ignored
  // 3 remote command
  // 30 rand number from remote
  // 28 hash from remote
  // local_rand:
  // 30 rand number sent in last transmission
  __xdata unsigned char hash[28];
  unsigned char i;
  sha256_init( &ctx );
  sha256_hmac_starts( &ctx, key, klen, 1 );
  printf("\nHMAC:");
  
  for (i=0;i<3;++i) printf(" %02X", message[1+i]);
  sha256_hmac_update( &ctx, &(message[1]), 3);
  
  for (i=0;i<30;++i) printf(" %02X",local_rand[i]);
  sha256_hmac_update( &ctx, local_rand, 30);
  
  for (i=0;i<30;++i) printf(" %02X",message[4+i]);  
  sha256_hmac_update( &ctx, &(message[4]), 30);
  
  printf("\r\n");
  
  sha256_hmac_finish( &ctx, hash );
  return memcmp( &(message[34]), hash, sizeof(hash) );
}


// ============= hardware routines ===============
void get_rand ( unsigned char rand[5] ) {
  __data unsigned char i;
  P1M0  &= ~0x01;
  P1M1  |= 0x01;
  P1ASF |= 0x01;
  EADC = 1;
  rand[4] = 0;
  for (i=0;i<4;++i) {
    ADC_CONTR = 0xE8;
    __asm__("NOP\nNOP\nNOP\nNOP");
    while (! (ADC_CONTR&0x10) ) {++PCON;}
    rand[i] = ADC_RES;
    rand[4] |= (ADC_RESL&0x03)<<(i*2);
  }
  ADC_CONTR = 0;
}
