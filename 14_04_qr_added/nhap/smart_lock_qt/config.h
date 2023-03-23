#ifndef config_h
#define config_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
/*******************************************************************************
   Definitions
 ******************************************************************************/
#define this_esp_number_of_tlc             2

//fit in the old code format
#define BOARD_COUNT this_esp_number_of_tlc

#define REGISTER_COUNT (BOARD_COUNT * 2)
#define PORT_COUNT (REGISTER_COUNT * 12)

/*SFR-05 */
#define TRIG_PIN 0
#define ECHO_PIN 2
#define LED_PIN_TEST 3
#define LOCK_TEST_TLC_0     0
#define LOCK_TEST_TLC_1     1
#define LOCK_TEST_TLC_2     2
#define LOCK_TEST_TLC_3     3
#define LOCK_TEST_TLC_4     4
#define LOCK_TEST_TLC_5     5
#define LOCK_TEST_TLC_6     6
#define LOCK_TEST_TLC_7     7
#define LOCK_TEST_TLC_8     8
#define LOCK_TEST_TLC_9     9
#define LOCK_TEST_TLC_10    10
#define LOCK_TEST_TLC_11    11
#define LOCK_TEST_TLC_12    12



#define SO_TU_MAX 12

#define DISTANCE_MAX 30
#define DISTANCE_MIN 3

#define COUNTER_QR_CODE 100
#define PATH_FILE_SETTING "/home/pi/Desktop/ADELA/14_04_qr_added/nhap/build_Smart_LOCK_qt/Setting.txt"
#define File_Log "/home/pi/Desktop/ADELA/Log/"
#define PATH_VIDEO_MOTU "/home/pi/Desktop/ADELA/video_mo_tu/build/video_mo_tu"
// video nhap dung
#define FILE_ID_VIDEO "/home/pi/Desktop/ADELA/Name_video_tu/"
#define PATH_VIDEO_NHAP_SAI "/home/pi/Desktop/ADELA/video_nhap_sai/build/video_nhap_sai"
/* LOck to test TLC with Qr code */

/*******************************************************************************
   API
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // config_h
