#ifndef TLC_H
#define TLC_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
       Define & const
 ****************************************************************************/
#define DATA   23
#define CLOCK  18

typedef unsigned char byte;

#define PACKET_BYTES 28


/*******************************************************************************
   API
 ******************************************************************************/
void writeHeaderToPacket(byte packet[], byte brightness);
void updateData() ;
void updateDataDimming();
void ghepDataVaoFrame();
void guiDuLieuPacket(int fd, byte packet[]);
void guiDuLieu();
void batCong(byte packet[], int cong);
void tatCong(byte packet[], int cong);
void resetPacket(byte packet[]);

void tlc_init();
byte get_data_proc(int i);
void set_data_proc(int i, byte value);

#ifdef __cplusplus
}
#endif

#endif // MY_LCD_H
