#ifndef _SPI_H
#define _SPI_H

#include <stc12.h>

#define SPI_BUFFER_SIZE 65
#define SPI_MOSI        P1_5
#define SPI_MISO        P1_6

#define spi_wait() while (SPI_BUSY) {++PCON;}

__bit volatile SPI_BUSY;
__xdata unsigned char spi_buffer[SPI_BUFFER_SIZE+1]; //buffer is sent starting from top
unsigned char spi_buffer_remain;                    //Also the susbcript of the byte last finished

void spi_init(void);
unsigned char spi_send(unsigned char data);
void spi_send_async(unsigned char size);
void spi_isr(void) __interrupt 9 __using 1;

void spi_init(void) {
  SPCTL  = 0xD3;  //SSIG=1, SPEN=1, DORD=0, MSTR=1, CPOL=0, CPHA=0, SPR=00; P1_4 can be used
  //SPCTL  = 0x50;  //SSIG=0, SPEN=1, DORD=0, MSTR=1, CPOL=0, CPHA=0, SPR=00
  SPSTAT = 0xC0;  //clear SPI flags
  IE2    |= 0x02; //Enable SPI Interrupt
  SPI_BUSY = 0;
  spi_buffer_remain=0;
}


unsigned char spi_send(unsigned char data) {
// syncronous spi transfer
// Note: does not set SPI_SS. take care of SS signal before calling
//data in buffer is send in reverse order, top first
  spi_wait();           // wait if there is ongoing transfer (if any async)
  SPI_BUSY = 1;
  SPDAT    = data;
  spi_wait();
  return SPDAT;
}

void spi_send_async(unsigned char size) {
  if (size>SPI_BUFFER_SIZE) return;
  spi_wait();
  SPI_BUSY  = 1;
  spi_buffer_remain=size;
  spi_isr();
}

void spi_isr(void) __interrupt 9 {
  SPSTAT    = 0xC0;  //clear SPI flags
  //spi_buffer_remain is the bytes not yet sent, (spi_buffer_remain+1)th data has just been sent at this point
  if (spi_buffer_remain) {
    spi_buffer[spi_buffer_remain] = SPDAT;
    --spi_buffer_remain;
    SPDAT = spi_buffer[spi_buffer_remain];
  }
  else {
    //last byte finished
    spi_buffer[0] = SPDAT;
    SPI_BUSY  = 0;
  }
}

#endif
