#ifndef EXAMPLE_COSEM_CLIENT_H
#define EXAMPLE_COSEM_CLIENT_H

#include <unistd.h>
#include <cstdint>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <chrono>

// Gurux
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "GXDLMSClient.h"
#include "GXDLMSClock.h"
#include "GXDLMSRegister.h"
#pragma GCC diagnostic pop


enum ModemState
{
    DISCONNECTED,
    CONNECTED
};

enum CosemState
{
    HDLC,
    ASSOCIATION_PENDING,
    ASSOCIATED
};

enum PrintFormat
{
    NO_PRINT,
    PRINT_RAW,
    PRINT_HEX
};

class Semaphore
{
public:

    explicit Semaphore(size_t count = 0);
    Semaphore(const Semaphore&) = delete;
    Semaphore(Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore& operator=(Semaphore&&) = delete;

    void notify();
//    void wait();
//    bool try_wait();

    int wait_for(std::chrono::milliseconds timeout);
//    template<class Clock, class Duration>
//    bool wait_until(const std::chrono::time_point<Clock, Duration>& t);

private:
    std::mutex   mMutex;
    std::condition_variable mCv;
    size_t  mCount;
};



class CosemClient
{
public:
    CosemClient();

    void Initialize();
    void WaitForStop();

    bool Open(const std::string &comport, uint32_t baudrate);
    int Test();
    int Dial(const std::string &phone);

    // return the bytes read
    int Send(const std::string &data, PrintFormat format);
    int ConnectHdlc();

    bool WaitForData(std::string &data, int timeout);

    void * Reader();

    static void *thread_reader(void *context)
    {
        return ((CosemClient *)context)->Reader();
    }

    bool PerformTask(const std::string &phone, int client, const std::string &lls);
    bool PerformCosemRead();
    int ConnectAarq();
    int ReadClock();
    int ReadRegister();

private:
    ModemState mModemState;
    CosemState mCosemState;
    bool mUseTcpGateway;
    int mSerialHandle;

    static const uint32_t cBufferSize = 40U*1024U;
    char mBuffer[cBufferSize];
    int mBufSize;

    std::string mData;
    pthread_t mThread;
    CGXDLMSClient mClient;
    bool mTerminate;

    pthread_mutex_t mDataMutex;
    Semaphore mSem;

};

#endif // EXAMPLE_COSEM_CLIENT_H
