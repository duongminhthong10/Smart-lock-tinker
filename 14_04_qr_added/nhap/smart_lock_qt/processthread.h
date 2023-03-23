#ifndef PROCESSTHREAD_H
#define PROCESSTHREAD_H
#include <QThread>

#ifdef __cplusplus
extern "C"
{
#endif

extern void bat_video_mo_tu();
extern void tat_video_so_tu();
extern void tat_video_qc();
extern void bat_video_qc();
extern void luongDem();
extern void luong_dem_12s();
extern void luong_dem_12sNhapSai();
extern void tat_video_nhap_sai();
extern void bat_video_nhap_sai();

#ifdef __cplusplus
}
#endif
#endif // PROCESSTHREAD_H
