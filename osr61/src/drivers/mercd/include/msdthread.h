/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdthread.h
 * Description                  : threading interface
 *
 *
 **********************************************************************/

#if ! defined( __MSDTHREAD_H__ )
#	define __MSDTHREAD_H__

#ifdef __USE_PTHREAD__
#	include	<pthread.h>

#	define	THR_BOUND    0
#	define	THR_NEW_LWP  0
#	define	THR_DETACHED 0
#	define	THR_SUSPEND  0
#	define	THR_DAEMON   0

#else
#	include	<thread.h>
#	include	<synch.h>
#endif 

#ifdef __USE_PTHREAD__

/* Posix Thread interface */

typedef pthread_t         msd_thread_t;
typedef	pthread_mutex_t   msd_mutex_t;
typedef	pthread_key_t     msd_thread_key_t;

#define msd_thread_join( _tid_, _param_ )  pthread_join(_tid_, (void **)&(_param_).status)  

#define msd_mutex_lock( _mutex_ )    pthread_mutex_lock(_mutex_)  
#define msd_mutex_unlock( _mutex_ )  pthread_mutex_unlock(_mutex_)  

#define msd_mutex_init( _mutex_, _attr_ )  pthread_mutex_init(_mutex_, _attr_)  

#define msd_thread_setspecific( _key_, _ret_)  pthread_setspecific((_key_), (void *) (_ret_)) 
#define msd_thread_getspecific( _key_, _ret_ )  _ret_ = pthread_getspecific( _key_ ) 

#define msd_thread_key_create(_key_, _dtor_, _ret_)  _ret_ = pthread_key_create((_key_), (_dtor_)) 

#define msd_thread_create(_tid_, _param_, _ThreadFunc_, _ThreadArg_) \
pthread_create(_tid_, (pthread_attr_t *) _param_.threadattr, _ThreadFunc_, _ThreadArg_)  

#else   /* USL Thread interface */

typedef	mutex_t        msd_mutex_t;
typedef thread_t       msd_thread_t;
typedef	thread_key_t   msd_thread_key_t;

#define msd_thread_join(_tid_, _param_)  thr_join(_tid_, (_param_).departedTid, (void **)&(_param_).status)  

#define msd_mutex_lock(_mutex_)       mutex_lock(_mutex_)  
#define msd_mutex_unlock(_mutex_)     mutex_unlock(_mutex_)  

#define msd_thread_setspecific(_key_, _ret_)  thr_setspecific((_key_), (void *)(_ret_)) 
#define msd_thread_getspecific(_key_, _ret_)  thr_getspecific((_key_), (void **)&(_ret_)) 

/* 
 * There is a caveat here, suppose we want to init a shared mutex between processes then we
 * need to set USYNC_PROCESS. 
 */
#define msd_mutex_init(_mutex_, _attr_)  mutex_init(_mutex_, USYNC_THREAD, _attr_) 

#define msd_thread_key_create(_key_, _dtor_, _retCode_) \
  _retCode_ = thr_keycreate((_key_), (_dtor_))

#define msd_thread_create(_tid_, _param_, _ThreadFunc_, _ThreadArg_) \
  thr_create(_param_.stackBase, _param_.stackSize, _ThreadFunc_, _ThreadArg_, _param_.flags, _tid_)  

#endif

/* 
 * Structure needed by both pthread_create() or thr_create() to make the
 * interface generic to both USL and POSIX.
 */
typedef struct {
    long    flags;          /* For USL thread */
    size_t  stackSize;      /* For USL thread */
    void*   stackBase;      /* For USL thread */
    void*   threadattr;     /* For Posix thread pthread_attr_t */
} msd_thread_create_param;

/* 
 * Structure needed by both pthread_join() or thr_join() to make the
 * interface generic to both USL and POSIX.
 */
typedef struct {
	msd_thread_t* departedTid;  /* For USL thread */
	void*        status;       /* For both Posix and USL threads */
} msd_thread_join_param;

#endif	/* __MSDTHREAD_H__ */
