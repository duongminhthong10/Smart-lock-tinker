/*
   224 bit packet
    | 32 bit header
       | 6 bit WC (Write Command)
       | 5 bit FC (Function Control)
       | 21 bit BC (Brightness Control)
          | 7 bit Blue (0-3)
          | 7 bit Green (0-3)
          | 7 bit Red (0-3)
    | 192 bit GS (GrayScale)
       | 16 bit Blue3   -> channel 1
       | 16 bit Green30 -> channel 2
       | 16 bit Red3    -> channel 3
       | 16 bit Blue2   -> channel 4
        ...
       | 16 bit Red0    -> channel 12
*/
//#include <SPI.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "KHOI_TLC5971.h"
#include "config.h"

/****************************************************************************
       variable
 ****************************************************************************/

static const char *device = "/dev/spidev0.0";
static uint8_t mode;

static uint8_t bits = 8;
static uint32_t speed = 20000000;  //3000 tam on
static uint16_t delay;

//static const int spiClk = 100000; // 1 MHz// update: 1Mhz -> 0.1Mhz
//SPIClass * vspi = NULL;

/**
 * #define BOARD_COUNT this_esp_number_of_tlc
 * #define REGISTER_COUNT (BOARD_COUNT * 2)
 * #define PORT_COUNT (REGISTER_COUNT * 12)*
*/

static byte packets[REGISTER_COUNT][PACKET_BYTES]; /* So luong board * so_ic, so byte cua 1 goi */

static byte data_proc[REGISTER_COUNT * 12];     /* data procedure */
static byte data_proc_inv[REGISTER_COUNT * 12]; /* data procedure invert */

static int data_proc_dimming[REGISTER_COUNT * 12];  /* dimming: mowf(vietkey) */
static int data_proc_inv_dimming[REGISTER_COUNT * 12];

/*****************************************************************************
       code
 *****************************************************************************/
static void pabort(const char *s)
{
  perror(s);
  abort();
}

void writeHeaderToPacket(byte packet[], byte brightness) {
  // Dùng 32-bit long làm header 6 bit WC, 5 bit FC, 21 bit BC
  //              |6 WC|
  //                    |5FC|
  long header = 0b10010100010 << (32 - 6 - 5);

  // độ sáng mặc định là max ping
  brightness >>= 1; // lấy 7 bit
  header |= brightness;             // BC red
  header |= (long)brightness << 7;   // BC green
  header |= (long)brightness << 7 * 2; // BC blue

  // cop vào packet
  packet[0] = header >> 8 * 3;
  packet[1] = header >> 8 * 2;
  packet[2] = header >> 8;
  packet[3] = header;
}

void updateData() {
  for (int i = 0; i < BOARD_COUNT; i++) {
    for (int j = 0; j < 24; j++) {
      data_proc_inv[(i * 24 + j)] = data_proc[((BOARD_COUNT - 1 - i) * 24 + j)];
    }
  }
  for (int j = 0; j < REGISTER_COUNT; j++) {    /* so ic * 2 (do moi board co 2 con) */
    for (int i = 0; i < 12; i++) {      /* 12 chanel moi ic */
      int buff_data = (int)data_proc_inv[PORT_COUNT - 1 - (j * 12 + i)] * 256;
      packets[j][i * 2 + 4] = buff_data >> 8;
      packets[j][i * 2 + 5] = buff_data;
    }
  }
}

//void updateDataDimming() {
//  for (int i = 0; i < BOARD_COUNT; i++) {
//    for (int j = 0; j < 24; j++) {
//      data_proc_inv[(i * 24 + j)] = data_proc[((BOARD_COUNT - 1 - i) * 24 + j)];
//    }
//  }
//  for (int j = 0; j < REGISTER_COUNT; j++) {
//    for (int i = 0; i < 12; i++) {
//      int buff_data = (int)data_proc_inv[PORT_COUNT - 1 - (j * 12 + i)]*256;
//      packets[j][i * 2 + 4] = buff_data >> 8;
//      packets[j][i * 2 + 5] = buff_data;
//    }
//  }
//}

void updateDataDimming() {
  for (int i = 0; i < BOARD_COUNT; i++) {
    for (int j = 0; j < 24; j++) {
      data_proc_inv_dimming[(i * 24 + j)] = data_proc_dimming[((BOARD_COUNT - 1 - i) * 24 + j)];
    }
  }
  for (int j = 0; j < REGISTER_COUNT; j++) {
    for (int i = 0; i < 12; i++) {
      int buff_data = (int)data_proc_dimming[PORT_COUNT - 1 - (j * 12 + i)];
      packets[j][i * 2 + 4] = buff_data >> 8;
      packets[j][i * 2 + 5] = buff_data;
    }
  }
}

void ghepDataVaoFrame() {
  for (int i = 0; i < REGISTER_COUNT; i++) {
    writeHeaderToPacket(packets[i], 0x33);  //00110011
  }
  updateData();
}

void guiDuLieuPacket(int fd, byte packet[]) {
  //  for (byte i = 0; i < PACKET_BYTES; i++) {
  //    //    SPI mềm
  //    //        shiftOut(DATA, CLOCK, MSBFIRST, packet[i]);
  //    //    SPI.write(packet[i]);
  //    vspi->transfer(packet[i]);
  //  }
  //  uint8_t rx[ARRAY_SIZE(tx)] = {0, };
  int ret = 0;
  struct spi_ioc_transfer tr = {
    (unsigned long)packet,
    0,

    PACKET_BYTES,
    (uint32_t)speed,

    delay,
    bits,
    0,
    0,
    0,
    0,
    0,
  };

  ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
  if (ret < 1)
    pabort("can't send spi message");

}

void guiDuLieu() {
  //  noInterrupts();
  //  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  int ret = 0;
  int fd;

  fd = open(device, O_RDWR);

  for (int i = 0; i < REGISTER_COUNT; i++) {
    guiDuLieuPacket(fd, packets[i]);
  }

  close(fd);
  //  vspi->endTransaction();
  //  //  interrupts();
  //  delayMicroseconds(200);
  //  //  delay(10);
}

void batCong(byte packet[], int cong) {
  packet[cong * 2 + 4] = 0xff;
  packet[cong * 2 + 5] = 0xff;
}

void tatCong(byte packet[], int cong) {
  packet[cong * 2 + 4] = 0x00;
  packet[cong * 2 + 5] = 0x00;
}

void resetPacket(byte packet[]) {
  for (int i = 0; i < 12; i++) {
    tatCong(packet, i);
  }
}

void set_data_proc(int i, byte value) {
  data_proc[i] = value;
}

byte get_data_proc(int i) {
  return data_proc[i];
}

void tlc_init() {
  //vspi = new SPIClass(VSPI);
  //vspi->begin();
  //  vspi->setBitOrder(MSBFIRST); // Set the SPI_1 bit order
  //  vspi->setDataMode(SPI_MODE0); //Set the  SPI_2 data mode 0
  //  vspi->setClockDivider(SPI_CLOCK_DIV64);      // Slow speed (72 / 16 = 4.5 MHz SPI_1 speed)
  //  pinMode(VSPI_SS, OUTPUT); //VSPI SS
  //  SPI.begin();
  //  SPI.setBitOrder(MSBFIRST); // Set the SPI_1 bit order
  //  SPI.setDataMode(SPI_MODE0); //Set the  SPI_2 data mode 0
  //  SPI.setClockDivider(SPI_CLOCK_DIV64);      // Slow speed (72 / 16 = 4.5 MHz SPI_1 speed)
  //pinMode(DATA, OUTPUT);
  //pinMode(CLOCK, OUTPUT);
  int ret = 0;
  int fd;
  fd = open(device, O_RDWR);    //open device with permission read/write
  if (fd < 0)
    pabort("can not open device");

  /*
     spi mode
  */
  ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);      //thiet lap thuoc tinh spi
  if (ret == -1)
    pabort("can't set spi mode");

  ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
  if (ret == -1)
    pabort("can't get spi mode");

  /*
     bits per word
  */
  ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits); /* thiet lap so bits:8 moi tu */
  if (ret == -1)
    pabort("can't set bits per word");

  ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
  if (ret == -1)
    pabort("can't get bits per word");

  /*
     max speed hz
  */
  ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed); /* Thiet lap speed */
  if (ret == -1)
    pabort("can't set max speed hz");

  ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
  if (ret == -1)
    pabort("can't get max speed hz");

  close(fd);

  ghepDataVaoFrame();
  guiDuLieu();
}

