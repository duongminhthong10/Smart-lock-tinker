#include "KHOI_TLC5971.h"
#include "config.h"
#include <QThread>
#include "processthread.h"
#include"my_lock.h"
#include <QMutex>

/*******************************************************************************
 * VARIABLES
 ******************************************************************************/
unsigned int sellected_id = -1;
QMutex mutex;

/*******************************************************************************
 * API
 ******************************************************************************/
static void *unlockThreadHandle(void *arg);

/*******************************************************************************
 * Definitions
 ******************************************************************************/

// initialize hardware
void my_lock_init(){
    tlc_init();
}

// unlock a box at input address
void my_lock_unlock(int box_id){
//    cong 14 hong, map sang 48
//    if(box_id == 13) box_id = 45;
//    if(box_id == 17) box_id = 46;
    set_data_proc(box_id, 0xff);
    updateData();
    guiDuLieu();
}

// lock a box at input address
void my_lock_lock(int box_id){
// cong 14 hong, map sang 48
//    if(box_id == 13) box_id = 45;
    set_data_proc(box_id, 0x00);    /* data_proc[box_id] = 0x00 */
    updateData();
    guiDuLieu();
}

// get state
char my_lock_get_state(int box_id){
    return get_data_proc(box_id);
}


void* unlockThreadHandle(void *arg){
    int position;
    mutex.lock();
    position = sellected_id -1;
    my_lock_unlock(position);
    //cong 43, 44 bi ngao nen de 300ms
    if(sellected_id == 30){
        QThread::msleep(150);
    }
    else {
        //cac cong khac de 200
        QThread::msleep(150);
    }

//    QThread::msleep(345);
    my_lock_lock(position);
    mutex.unlock();
    return nullptr;
}

void my_lock_unLock_and_lock()
{
    pthread_t lockIDThread;
    pthread_create(&lockIDThread, nullptr, &unlockThreadHandle, nullptr);
}

void* unlockThreadHandleTest_0(void *arg){
    my_lock_unlock(LOCK_TEST_TLC_0);
    QThread::msleep(180);
    my_lock_lock(LOCK_TEST_TLC_0);
    return nullptr;
}

void my_lock_unLock_and_lock_test_0()
{
    pthread_t lockIDThreadTest0;
    pthread_create(&lockIDThreadTest0, nullptr, &unlockThreadHandleTest_0, nullptr);
}

void* unlockThreadHandleTest_1(void *arg){
    my_lock_unlock(LOCK_TEST_TLC_1);
    QThread::msleep(180);
    my_lock_lock(LOCK_TEST_TLC_1);
    return nullptr;
}

void my_lock_unLock_and_lock_test_1()
{
    pthread_t lockIDThreadTest1;
    pthread_create(&lockIDThreadTest1, nullptr, &unlockThreadHandleTest_1, nullptr);
}

void* unlockThreadHandleTest_2(void *arg){
    my_lock_unlock(LOCK_TEST_TLC_2);
    QThread::msleep(180);
    my_lock_lock(LOCK_TEST_TLC_2);
    return nullptr;
}

void my_lock_unLock_and_lock_test_2()
{
    pthread_t lockIDThreadTest2;
    pthread_create(&lockIDThreadTest2, nullptr, &unlockThreadHandleTest_2, nullptr);
}

void* unlockThreadHandleTest_3(void *arg){
    my_lock_unlock(LOCK_TEST_TLC_3);
    QThread::msleep(180);
    my_lock_lock(LOCK_TEST_TLC_3);
    return nullptr;
}

void my_lock_unLock_and_lock_test_3()
{
    pthread_t lockIDThreadTest3;
    pthread_create(&lockIDThreadTest3, nullptr, &unlockThreadHandleTest_3, nullptr);
}

void* unlockThreadHandleTest_4(void *arg){
    my_lock_unlock(LOCK_TEST_TLC_4);
    QThread::msleep(180);
    my_lock_lock(LOCK_TEST_TLC_4);
    return nullptr;
}

void my_lock_unLock_and_lock_test_4()
{
    pthread_t lockIDThreadTest4;
    pthread_create(&lockIDThreadTest4, nullptr, &unlockThreadHandleTest_4, nullptr);
}

void* unlockThreadHandleTest_5(void *arg){
    my_lock_unlock(LOCK_TEST_TLC_4);
    QThread::msleep(180);
    my_lock_lock(LOCK_TEST_TLC_4);
    return nullptr;
}

void my_lock_unLock_and_lock_test_5()
{
    pthread_t lockIDThreadTest5;
    pthread_create(&lockIDThreadTest5, nullptr, &unlockThreadHandleTest_5, nullptr);
}

void* unlockThreadHandleTest_6(void *arg){
    my_lock_unlock(LOCK_TEST_TLC_6);
    QThread::msleep(180);
    my_lock_lock(LOCK_TEST_TLC_6);
    return nullptr;
}

void my_lock_unLock_and_lock_test_6()
{
    pthread_t lockIDThreadTest6;
    pthread_create(&lockIDThreadTest6, nullptr, &unlockThreadHandleTest_6, nullptr);
}

void* unlockThreadHandleTest_7(void *arg){
    my_lock_unlock(LOCK_TEST_TLC_7);
    QThread::msleep(180);
    my_lock_lock(LOCK_TEST_TLC_7);
    return nullptr;
}

void my_lock_unLock_and_lock_test_7()
{
    pthread_t lockIDThreadTest7;
    pthread_create(&lockIDThreadTest7, nullptr, &unlockThreadHandleTest_7, nullptr);
}

void* unlockThreadHandleTest_8(void *arg){
    my_lock_unlock(LOCK_TEST_TLC_8);
    QThread::msleep(180);
    my_lock_lock(LOCK_TEST_TLC_8);
    return nullptr;
}

void my_lock_unLock_and_lock_test_8()
{
    pthread_t lockIDThreadTest8;
    pthread_create(&lockIDThreadTest8, nullptr, &unlockThreadHandleTest_8, nullptr);
}

void* unlockThreadHandleTest_9(void *arg){
    my_lock_unlock(LOCK_TEST_TLC_9);
    QThread::msleep(180);
    my_lock_lock(LOCK_TEST_TLC_9);
    return nullptr;
}

void my_lock_unLock_and_lock_test_9()
{
    pthread_t lockIDThreadTest9;
    pthread_create(&lockIDThreadTest9, nullptr, &unlockThreadHandleTest_9, nullptr);
}

void* unlockThreadHandleTest_10(void *arg){
    my_lock_unlock(LOCK_TEST_TLC_10);
    QThread::msleep(180);
    my_lock_lock(LOCK_TEST_TLC_10);
    return nullptr;
}

void my_lock_unLock_and_lock_test_10()
{
    pthread_t lockIDThreadTest10;
    pthread_create(&lockIDThreadTest10, nullptr, &unlockThreadHandleTest_10, nullptr);
}

void* unlockThreadHandleTest_11(void *arg){
    my_lock_unlock(LOCK_TEST_TLC_11);
    QThread::msleep(180);
    my_lock_lock(LOCK_TEST_TLC_11);
    return nullptr;
}

void my_lock_unLock_and_lock_test_11()
{
    pthread_t lockIDThreadTest11;
    pthread_create(&lockIDThreadTest11, nullptr, &unlockThreadHandleTest_11, nullptr);
}


