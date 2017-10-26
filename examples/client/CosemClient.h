#ifndef EXAMPLE_COSEM_CLIENT_H
#define EXAMPLE_COSEM_CLIENT_H

#include <unistd.h>
#include <cstdint>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <list>

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
    DIAL,
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

struct Modem
{
    std::string phone;
    std::string init;
};

struct Serial
{
    Serial()
        : baudrate(9600)
    {

    }
    std::string port;
    unsigned int baudrate;
};

struct Cosem
{
    Cosem()
        : client(1U)
        , server(1U)
    {

    }
    std::string lls;
    std::uint16_t client;
    std::uint16_t server;

    std::string start_date;
    std::string end_date;
};

enum Device
{
    NONE = 0,
    MODEM = 1
};


struct Object
{
    Object()
        : class_id(0U)
        , attribute_id(0)
    {

    }

    void Print()
    {
        std::cout << "Object " << name << ": " << ln << " Class ID: " << class_id << " Attribute: " << attribute_id << std::endl;
    }

    std::string name;
    std::string ln;
    std::uint16_t class_id;
    std::int16_t attribute_id;
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

    bool Open(const std::string &comport, std::uint32_t baudrate);
    int Test();
    int Dial(const std::string &phone);

    void SetDevice(Device dev) {
        mDevice = dev;
    }

    // return the bytes read
    int Send(const std::string &data, PrintFormat format);
    int ConnectHdlc();

    bool WaitForData(std::string &data, int timeout);

    void * Reader();

    static void *thread_reader(void *context)
    {
        return ((CosemClient *)context)->Reader();
    }

    bool PerformTask(const Modem &modem, const Cosem &cosem, const std::vector<Object> &list);
    bool PerformCosemRead(const std::vector<Object> &list, const Cosem &cosem);
    int ConnectAarq();
    int ReadClock();
    int ReadRegister(const Object &obj);
    int ReadProfile(const Object &obj, const Cosem &cosem);

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
    std::uint32_t mReadIndex;
    Device mDevice;

    pthread_mutex_t mDataMutex;
    Semaphore mSem;

};

#endif // EXAMPLE_COSEM_CLIENT_H
