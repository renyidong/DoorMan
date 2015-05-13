#ifndef _CC1101_H
#define _CC1101_H

#include "debug.h"
#include <string.h>
#include "spi.h"
#include "cc1101_cfg.h"

#define cc1101_buffer spi_buffer
#define RF_RX_INT     P3_2

typedef enum {
  // config registers, r/w, burst will iterate over
  CC1101_IOCFG2        =0x00,
  CC1101_IOCFG1        =0x01,
  CC1101_IOCFG0        =0x02,
  CC1101_FIFOTHR       =0x03,
  CC1101_SYNC1         =0x04,
  CC1101_SYNC0         =0x05,
  CC1101_PKTLEN        =0x06,
  CC1101_PKTCTRL1      =0x07,
  CC1101_PKTCTRL0      =0x08,
  CC1101_ADDR          =0x09,
  CC1101_CHANNR        =0x0A,
  CC1101_FSCTRL1       =0x0B,
  CC1101_FSCTRL0       =0x0C,
  CC1101_FREQ2         =0x0D,
  CC1101_FREQ1         =0x0E,
  CC1101_FREQ0         =0x0F,
  CC1101_MDMCFG4       =0x10,
  CC1101_MDMCFG3       =0x11,
  CC1101_MDMCFG2       =0x12,
  CC1101_MDMCFG1       =0x13,
  CC1101_MDMCFG0       =0x14,
  CC1101_DEVIATN       =0x15,
  CC1101_MCSM2         =0x16,
  CC1101_MCSM1         =0x17,
  CC1101_MCSM0         =0x18,
  CC1101_FOCCFG        =0x19,
  CC1101_BSCFG         =0x1A,
  CC1101_AGCCTRL2      =0x1B,
  CC1101_AGCCTRL1      =0x1C,
  CC1101_AGCCTRL0      =0x1D,
  CC1101_WOREVT1       =0x1E,
  CC1101_WOREVT0       =0x1F,
  CC1101_WORCTRL       =0x20,
  CC1101_FREND1        =0x21,
  CC1101_FREND0        =0x22,
  CC1101_FSCAL3        =0x23,
  CC1101_FSCAL2        =0x24,
  CC1101_FSCAL1        =0x25,
  CC1101_FSCAL0        =0x26,
  CC1101_RCCTRL1       =0x27,
  CC1101_RCCTRL0       =0x28,
  CC1101_FSTEST        =0x29,
  CC1101_PTEST         =0x2A,
  CC1101_AGCTEST       =0x2B,
  CC1101_TEST2         =0x2C,
  CC1101_TEST1         =0x2D,
  CC1101_TEST0         =0x2E,
  // command strobe, non burst access only, r/w doesn't matter, retrun value differs
  CC1101_SRES          =0x30,
  CC1101_SFSTXON       =0x31,
  CC1101_SXOFF         =0x32,
  CC1101_SCAL          =0x33,
  CC1101_SRX           =0x34,
  CC1101_STX           =0x35,
  CC1101_SIDLE         =0x36,
  CC1101_SWOR          =0x38,
  CC1101_SPWD          =0x39,
  CC1101_SFRX          =0x3A,
  CC1101_SFTX          =0x3B,
  CC1101_SWORRST       =0x3C,
  CC1101_SNOP          =0x3C,
  // PATable, access follows 6 data bytes
  CC1101_PATABLE       =0x3D,
  // TX when write, RX when read, burst will fill the buffer
  CC1101_TX_FIFO       =0x3F,
  CC1101_RX_FIFO       =0xBF,
  // Status register, read only, burst access only
  CC1101_PARTNUM       =0xF0,
  CC1101_VERSION       =0xF1,
  CC1101_FREQEST       =0xF2,
  CC1101_LQI           =0xF3,
  CC1101_RSSI          =0xF4,
  CC1101_MARCSTATE     =0xF5,
  CC1101_WORTIME1      =0xF6,
  CC1101_WORTIME0      =0xF7,
  CC1101_PKTSTATUS     =0xF8,
  CC1101_VCO_VC_DAC    =0xF9,
  CC1101_TXBYTES       =0xFA,
  CC1101_RXBYTES       =0xFB,
  CC1101_RCCTRL1_STATUS=0xFC,
  CC1101_RCCTRL0_STATUS=0xFD
} cc1101_address;

void          cc1101_init (void);
void          cc1101_register_async (cc1101_address addr, __bit write, unsigned char data);
unsigned char cc1101_register_sync  (cc1101_address addr, __bit write, unsigned char data);
unsigned char cc1101_command (cc1101_address cmd);
__xdata unsigned char * cc1101_burst_access(cc1101_address addr, __bit write, unsigned char size);

__data volatile unsigned char t1_count;

void cc1101_init (void) {
  __data unsigned char i=0,j;
  EA    =1;
  // Timer 1 is used
  TMOD  = (TMOD&0x0F)|0x10;     //T1 16bit
  AUXR  &= ~0x40;               //T1 CLK/12
  TF1   = 0;
  TR1   = 1;
  ET1   = 1;
  RF_SPI_SS = 0;
  while(--i);//wait for ~40 us
  RF_SPI_SS = 1;
  while(--i);
  cc1101_command(CC1101_SRES);
  for (i=0,j=sizeof(CC1101_CFG)-1; i<sizeof(CC1101_CFG); ++i,--j) {
    cc1101_buffer[i]=CC1101_CFG[j];
  }
  cc1101_burst_access(0x00,1,sizeof(CC1101_CFG));  //write config registers
  IT0=1;
  EX0=1;
}

void cc1101_register_async(cc1101_address addr,__bit write, unsigned char data) {
  // config reister single access
  // will wait for last access finish
  // then send data in background
  // if you need return value, call spi_wait then read cc1101_buffer
  if (!write) addr|= 0x80; //set rw bit
  //Wait for cc1101 ready
  spi_wait();
  // burst bit should be clear, but you may hack if you know what you'r doing
  spi_buffer[1]=addr;
  spi_buffer[0]=data;
  //send
  RF_SPI_SS = 0;
  while (SPI_MISO){};
  spi_send_async(2);
  // Sigle access does not require ss pin to high between each access
}

unsigned char cc1101_register_sync(cc1101_address addr,__bit write, unsigned char data) {
  // same as async, block and return value
  cc1101_register_async(addr, write, data);
  spi_wait();
  RF_SPI_SS = 1;
  return SPDAT;
}

unsigned char cc1101_command (cc1101_address cmd) {
  unsigned char ret;
  spi_wait();
  RF_SPI_SS = 0;
  while (SPI_MISO){};
  ret = spi_send(cmd);
  RF_SPI_SS = 1;
  return ret;
}

__xdata unsigned char * cc1101_burst_access(cc1101_address addr, __bit write, unsigned char size) {
  //spi buffer should be prepared before calling

  spi_wait();
  //Wait for c1101 ready
  RF_SPI_SS = 0;
  while (SPI_MISO){};
  if (!write) addr|= 0x80;  //set rw bit
  addr|= 0x40;               //set burst bit
  cc1101_buffer[size] = addr;
  spi_send_async(size+1);
  spi_wait();
  RF_SPI_SS = 1;        //end of burst access
  return cc1101_buffer;
}

void cc1101_wait_idle(void) {
  //dprintf("WAIT Idle\n");
  while( cc1101_command(CC1101_SNOP) & 0x70 );
}

void cc1101_set_idle(void) {
  cc1101_command(CC1101_SIDLE);
  cc1101_wait_idle();
  //dprintf("IDLE\n");
}

void cc1101_put_data (unsigned char addr, unsigned char * data, unsigned char size) { 
  // size should be <=62, if status byte then <=60
#ifdef DEBUG
  if (size>62) dprintf("cc1101_put_data() Overflow!\n");
#endif
  cc1101_set_idle();
  cc1101_command(CC1101_SFTX);

  memcpy(cc1101_buffer,data,size);
  cc1101_buffer[size] =addr;
  ++size;
  cc1101_buffer[size] =size;
  ++size;
  
  cc1101_burst_access(CC1101_TX_FIFO,1,size);
  dprintf("RTS\n");
  cc1101_command(CC1101_STX);                     // start TX
  dprintf("STX\n");
  while ( cc1101_register_sync(CC1101_TXBYTES,0,0xFF) ) {
    //dprintf("TX\n");
  };
  dprintf("Sent\n");
}

unsigned char cc1101_get_data (unsigned char addr, unsigned char * data, unsigned char max_size, unsigned char timeout_tenms) {
  unsigned char size;
  cc1101_set_idle();
  cc1101_command(CC1101_SFRX);
  // config addr and max size here?
  cc1101_command(CC1101_SRX);
  t1_count = timeout_tenms;
  TR1 = 1;
  while ( RF_RX_INT ) {                           //RF_RX_INT should be config as 0x47
    if (t1_count==0) {
      cc1101_set_idle();                          // Leave RX when timed out
      return 0;                                   // Got no data, of course
    }
    ++PCON;
  } 
  cc1101_set_idle();                              //Leave RX when get one packet
  size = cc1101_register_sync(CC1101_RX_FIFO,0,0xFF);
  memset(spi_buffer,0xFF,size);
  cc1101_burst_access(CC1101_RX_FIFO,0,size);
  if (data) {
    // if data pointer is 0, data received is kept in spi_buffer to save overhead of copy
    memcpy(data,spi_buffer,size);
  }
  // returns size without size byte
  // but with address byte included
  return size;
}

//===================== isr ======================
void t1_isr (void) __interrupt 3 {
  // 10ms @ 22.1184MHz/12
  // t1_count is used as a countdown timer
  // Usage: Load t1_count with desired value, set TR1. t1_count will stop at 0
  TL1    = 0x00;
  TH1    = 0xB8;
  if (t1_count) --t1_count;
  else           TR1 = 0;
}
#endif
