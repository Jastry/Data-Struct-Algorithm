#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "locker.h"

/* 线程池类，将他定义为模板类是为了代码复用。模板参数 T 是任务类 */
template < typename T >
class threadpool {
public:
    /*
     *  thread_number 是线程池中线程的数量，
     *  max_request   是请求队列中最多允许的、等待处理请求的数量
     */
    threadpool( int thread_number = 8, int max_request = 8192 );
    ~threadpool();

    /* 往请求队列中添加数据 */
    bool append( T * request );

private:
    /* 工作线程运行的函数，他不断从工作队列中取出任务并执行之 */
    static void * worker( void * arg );

    void run();

private:
    /* 线程池中线程数 */
    int m_thread_number;

    /* 请求队列中允许的最大请求数 */
    int m_max_request;

    /* 描述线程池的数组，其大小为 m_pthread_number */
    pthread_t * m_threads;

    /* 请求队列 */
    std::list< T* > m_workqueue;

    /* 保护请求队列的互斥锁 */
    locker m_queuelocker;

    /* 是否有任务需要处理 */
    sem m_queuestat;

    /* 是否结束线程 */
    bool m_stop;
};

template < typename T >
threadpool< T >::threadpool( int thread_num, int max_request )
    :m_thread_number( thread_num ), m_max_request( max_request ),
    m_stop( false ), m_threads( NULL )
{
    if (( m_thread_number <= 0 ) || ( max_request <= 0 )) {
        throw std::exception();
    }

    m_threads = new pthread_t[ m_thread_number ];
    if ( !m_threads ) {
        throw std::exception();
    }

    /* 创建 thread_number 个线程，并将它们都设置为脱离线程 */
    for ( int i = 0; i < m_thread_number; ++i ) {
        
        /* 初始化线程池 */
        //printf("create the %dth thread\n", i);
        if ( pthread_create(m_threads + i, NULL, worker, this) != 0 ) {
            delete [] m_threads;
            throw std::exception();
        }
        
        /* 
         * 将线程设置为分离，在线程终止时由操作系统自动回收，
         * 其他线程不能杀死或回收此线程。
         */
        if ( pthread_detach( m_threads[i] ) != 0 ) {
            delete [] m_threads;
            throw std::exception();
        }
    }
}

template < typename T >
threadpool< T >::~threadpool()
{
    delete [] m_threads;
    m_stop = true;
}

template <typename T >
bool threadpool< T >::append( T * request )
{
    m_queuelocker.lock();
    /* 操作工作队列时一定要加锁，因为工作队列被所有线程所共享 */
    if ( m_workqueue.size() > m_max_request ) {
#if 0
    while (m_workqueue.size() > m_max_request) {
        m_queuelocker.wait();    
    }

    m_workqueue.push_back( request );
    m_queuelocker.unlock();
    m_queuestat.post();
    reutrn true;
#endif 
        m_queuelocker.unlock();
        return false;
    }

    m_workqueue.push_back( request );
    m_queuelocker.unlock();
    
    /* 临界资源个数减一 */
    m_queuestat.post();
    return true;
}

template < typename T>
void * threadpool< T >::worker( void * arg )
{
    threadpool * pool = (threadpool*)arg;
    pool->run();
    return pool;
}

template < typename T >
void threadpool< T >::run()
{
    while ( !m_stop ) {
        /* 信号量减一 */
        m_queuestat.wait();
        m_queuelocker.lock();
        if ( m_workqueue.empty() ) {
            m_queuelocker.unlock();
            continue;
        }

        T * request = m_workqueue.front();
        
        /*_______________________________*/
        m_workqueue.pop_front();
        /*_______________________________*/
        m_queuelocker.unlock();
        if ( !request ) {
            continue;
        }
        //printf("线程执行 process\n");
        request->process();
    }
}

#endif  //__THREADPOOL_H__

/*
 * 值得一提的是，在 C++ 程序中使用 pthread_create 函数时，
 * 该函数的第三个参数必须指向一个静态函数。
 * 而要在一个静态函数中使用一个动态成员（包括成员函数和成员变量），
 * 则只能通过如下两种方法来实现
 *
 *  1. 通过类的静态对象来调用。比如单例（单体）模式中，
 *  静态函数可以通过类的全局唯一实例来访问动态成员函数。
 *
 *  2. 将类的对象作为参数传递给静态函数，然后在静态函数中引用这个对象，
 *  并调用其动态方法
 *
 *  本例中使用第二种方法，将线程参数设置为 this 指针，
 *  然后在 worker 函数中获取该指针并调用其动态方法 run
 */
