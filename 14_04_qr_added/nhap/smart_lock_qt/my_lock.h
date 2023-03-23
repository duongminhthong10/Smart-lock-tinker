#ifndef my_lock_h
#define my_lock_h

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * API
 ******************************************************************************/
// initialize hardware
void my_lock_init();

// unlock a box at input address (count from 0)
void my_lock_unlock(int box_id);

// lock a box at input address (count from 0)
void my_lock_lock(int box_id);

// get state
char my_lock_get_state(int box_id);

//unlck and lock
void my_lock_unLock_and_lock();

//to test, khi nao lam that thi thay LOCK_TLC_TEST Thanh id tu tra ve
void my_lock_unLock_and_lock_test();
void my_lock_unLock_and_lock_test_0();
void my_lock_unLock_and_lock_test_1();
void my_lock_unLock_and_lock_test_2();
void my_lock_unLock_and_lock_test_3();
void my_lock_unLock_and_lock_test_4();
void my_lock_unLock_and_lock_test_5();
void my_lock_unLock_and_lock_test_6();
void my_lock_unLock_and_lock_test_7();
void my_lock_unLock_and_lock_test_8();
void my_lock_unLock_and_lock_test_9();
void my_lock_unLock_and_lock_test_10();
void my_lock_unLock_and_lock_test_11();

#ifdef __cplusplus
}
#endif

#endif // my_lock_h
