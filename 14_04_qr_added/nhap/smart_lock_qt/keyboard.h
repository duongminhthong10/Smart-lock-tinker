#ifndef KEYBOARD_H
#define KEYBOARD_H

#ifdef __cplusplus
extern "C"
{
#endif

void *getchHandler(void *arg);
void getch(unsigned char checkterminal);


#ifdef __cplusplus
}
#endif


#endif // KEYBOARD_H
