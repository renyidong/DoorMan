#ifndef _CC1101_CFG_H
#define _CC1101_CFG_H

#define RF_SPI_SS       P1_4
//=========== Config data saved in code area =======
//vvvvvvvvvvv Paste output of SmartRF vvvvvvvvvvvvvv
// Data format = Normal mode 
// Channel number = 0 
// Sync word qualifier mode = 30/32 + carrier-sense above threshold 
// RX filter BW = 812.500000 
// Channel spacing = 25.390625 
// Address config = No address check 
// Base frequency = 432.999817 
// Carrier frequency = 432.999817 
// CRC autoflush = true 
// TX power = -10 
// Whitening = true 
// Device address = 0 
// Data rate = 49.9878 
// PA ramping = false 
// Preamble count = 2 
// Manchester enable = true 
// Packet length mode = Variable packet length mode. Packet length configured by the first byte after sync word 
// Deviation = 203.125000 
// Packet length = 63 
// CRC enable = true 
// Modulated = true 
// Modulation format = GFSK 
__code const unsigned char CC1101_CFG[] = {
0x47, //0000 IOCFG2            GDO2 Output Pin Configuration
0x2E, //0001 IOCFG1            GDO1 Output Pin Configuration
0x2E, //0002 IOCFG0            GDO0 Output Pin Configuration
0x07, //0003 FIFOTHR           RX FIFO and TX FIFO Thresholds
0x44, //0004 SYNC1             Sync Word, High Byte
0x4D, //0005 SYNC0             Sync Word, Low Byte
0x40, //0006 PKTLEN            Packet Length
0x08, //0007 PKTCTRL1          Packet Automation Control
0x45, //0008 PKTCTRL0          Packet Automation Control
0x00, //0009 ADDR              Device Address
0x00, //000A CHANNR            Channel Number
0x0E, //000B FSCTRL1           Frequency Synthesizer Control
0x00, //000C FSCTRL0           Frequency Synthesizer Control
0x10, //000D FREQ2             Frequency Control Word, High Byte
0xA7, //000E FREQ1             Frequency Control Word, Middle Byte
0x62, //000F FREQ0             Frequency Control Word, Low Byte
0x0A, //0010 MDMCFG4           Modem Configuration
0xF8, //0011 MDMCFG3           Modem Configuration
0x1F, //0012 MDMCFG2           Modem Configuration
0x00, //0013 MDMCFG1           Modem Configuration
0x00, //0014 MDMCFG0           Modem Configuration
0x70, //0015 DEVIATN           Modem Deviation Setting
0x07, //0016 MCSM2             Main Radio Control State Machine Configuration
0x0C, //0017 MCSM1             Main Radio Control State Machine Configuration
0x18, //0018 MCSM0             Main Radio Control State Machine Configuration
0x1D, //0019 FOCCFG            Frequency Offset Compensation Configuration
0x1C, //001A BSCFG             Bit Synchronization Configuration
0xC7, //001B AGCCTRL2          AGC Control
0x00, //001C AGCCTRL1          AGC Control
0xB0, //001D AGCCTRL0          AGC Control
0x87, //001E WOREVT1           High Byte Event0 Timeout
0x6B, //001F WOREVT0           Low Byte Event0 Timeout
0xF8, //0020 WORCTRL           Wake On Radio Control
0xB6, //0021 FREND1            Front End RX Configuration
0x10, //0022 FREND0            Front End TX Configuration
0xE9, //0023 FSCAL3            Frequency Synthesizer Calibration
0x2A, //0024 FSCAL2            Frequency Synthesizer Calibration
0x00, //0025 FSCAL1            Frequency Synthesizer Calibration
0x1F, //0026 FSCAL0            Frequency Synthesizer Calibration
0x41, //0027 RCCTRL1           RC Oscillator Configuration
0x00, //0028 RCCTRL0           RC Oscillator Configuration
0x59, //0029 FSTEST            Frequency Synthesizer Calibration Control
0x7F, //002A PTEST             Production Test
0x3F, //002B AGCTEST           AGC Test
0x88, //002C TEST2             Various Test Settings
0x31, //002D TEST1             Various Test Settings
0x09, //002E TEST0             Various Test Settings
};
//^^^^^^^^^^^^^^^^^^^^^Paste output of SmartRF^^^^^^^^^^^^^^^^^^^^^^^^^^^
#endif
