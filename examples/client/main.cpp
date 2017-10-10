
#include <iostream>
#include <cstdint>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include "serial.h"
#include "util.h"

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


int StringToBin(const std::string &in, char *out)
{
    uint32_t sz = in.size();
    int ret = 0;

    if (!(sz % 2))
    {
        hex2bin(in.c_str(), out, sz);
        ret = sz/2;
    }
    return ret;
}

void Printer(const char *text, int size, PrintFormat format)
{
    if (format != NO_PRINT)
    {
        if (format == PRINT_RAW)
        {
            fwrite(text, size, 1, stdout);
        }
        else
        {
            print_hex(text, size);
        }
    }
}



class Modem
{
public:
    Modem();

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
        return ((Modem *)context)->Reader();
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
    sem_t mSem;

};

Modem::Modem()
    : mModemState(DISCONNECTED)
    , mCosemState(HDLC)
    , mUseTcpGateway(false)
    , mSerialHandle(0)
    , mBufSize(0)
    , mThread(0)
    , mClient(true, 1, 1, DLMS_AUTHENTICATION_LOW, "001CA021", DLMS_INTERFACE_TYPE_HDLC)
    , mTerminate(false)
{
    mDataMutex = PTHREAD_MUTEX_INITIALIZER;
    sem_init(&mSem, 0, 0);
}


void Modem::Initialize()
{
    pthread_create(&mThread, NULL, &Modem::thread_reader, this);
}


void Modem::WaitForStop()
{
    mTerminate = true;
    pthread_join(mThread, NULL);
}


bool Modem::WaitForData(std::string &data, int timeout)
{
    bool ok = false;
    struct timespec ts;

    if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
    {
        //throw error
    }

    ts.tv_sec += timeout;

    int ret = 0;
    // Loop until data is received, 5 seconds max
    do {
        ret = sem_timedwait(&mSem, &ts);
    } while (ret != -1);


    pthread_mutex_lock( &mDataMutex );
    data = mData;
    pthread_mutex_unlock( &mDataMutex );

    if (data.size() > 0)
    {
        ok = true;
        mData.clear();
    }

    return ok;
}

void * Modem::Reader()
{
    printf("Reader thread started\r\n");

    while (!mTerminate)
    {
        int ret = serial_read(mSerialHandle, &mBuffer[0], cBufferSize, 30);

        if (ret > 0)
        {
            printf("<==== Got data: %d bytes: ", ret);
            std::string data(&mBuffer[0], ret);

            Printer(data.c_str(), data.size(), PRINT_HEX);

            puts("\r\n");

            // Add data
            pthread_mutex_lock( &mDataMutex );
            mData += data;
            pthread_mutex_unlock( &mDataMutex );

            // Signal new data available
            sem_post(&mSem);
        }
        else if (ret == 0)
        {
            puts("Still waiting for data...\r\n");
        }
        else
        {
            puts("Serial read error, exiting...\r\n");
            mTerminate = true;
        }
    }

    return NULL;
}

int Modem::Test()
{
    int ret = -1;

    if (Send("AT\r\n", PRINT_RAW) > 0)
    {
        std::string data;

        // Wait 2 seconds
        if (WaitForData(data, 2))
        {
            ret = data.size();
            Printer(data.c_str(), data.size(), PRINT_RAW);
        }
    }
    return ret;
}

int Modem::Dial(const std::string &phone)
{
    int ret = -1;

    std::string dialRequest = std::string("ATD") + phone + std::string("\r\n");

    if (Send(dialRequest, PRINT_RAW))
    {
        std::string data;
        sleep(20); // let the modem dial

        if (WaitForData(data, 60))
        {
            ret = data.size();
            Printer(data.c_str(), data.size(), PRINT_RAW);
        }
    }

    return ret;
}

int Modem::ConnectHdlc()
{
    int ret = -1;
    static const std::string snrm = "7EA0210002002303939A74818012050180060180070400000001080400000007655E7E";

    std::vector<CGXByteBuffer> data;

   // FIXME: SNRM does not seems to work, restest after changes made in HDLC framing
    (void) mClient.SNRMRequest(data);


    int size = StringToBin(snrm, &mBuffer[0]);

    sleep(1); // let the communication go on

    if (Send(std::string(&mBuffer[0], size), PRINT_HEX))
    {
        std::string data;
        sleep(1); // let the communication go on

        if (WaitForData(data, 5))
        {
            ret = data.size();
            Printer(data.c_str(), data.size(), PRINT_HEX);
        }
    }

    return ret;
}

int Modem::ConnectAarq()
{
    std::vector<CGXByteBuffer> data;

    int ret = 0;
    ret = mClient.AARQRequest(data);

    if ((ret == 0) && (data.size() > 0))
    {
      //  QByteArray req = QByteArray::fromHex("7EA0210002002303939A74818012050180060180070400000001080400000007655E7E");
        CGXByteBuffer gxPacket = data.at(0);
        std::string request((const char *)gxPacket.GetData(), gxPacket.GetSize());

        if (Send(request, PRINT_HEX))
        {
            std::string data;
            sleep(1); // let the communication go on

            if (WaitForData(data, 5))
            {
                ret = data.size();
                Printer(data.c_str(), data.size(), PRINT_HEX);
            }
        }
    }

    return ret;
}


int Modem::ReadClock()
{
    int attributeIndex = 2;
    int ret;
    std::vector<CGXByteBuffer> data;
    CGXReplyData reply;
    CGXDLMSClock clock;

    //Read data from the meter.
    ret = mClient.Read(&clock, attributeIndex, data);

    if ((ret == 0) && (data.size() > 0))
    {
        CGXByteBuffer gxPacket = data.at(0);
        std::string request((const char *)gxPacket.GetData(), gxPacket.GetSize());

        if (Send(request, PRINT_HEX))
        {
            std::string data;
            sleep(1); // let the communication go on

            if (WaitForData(data, 5))
            {
                ret = data.size();
                Printer(data.c_str(), data.size(), PRINT_HEX);

                CGXByteBuffer buffer;
                buffer.Set(data.data(), data.size());

                // Lecture de l'heure:
                             // 7ea021030002002352f04fe6e700 c401c1 00 09 0c07e0010906152638ff80000095237e

                // Bad: 7EA0140300020023521969E6E700 C401C10103 73827E


                CGXReplyData reply;
                mClient.GetData(buffer, reply);
                CGXDLMSVariant replyData = reply.GetValue();
                if (mClient.UpdateValue(clock, 2, replyData) == 0)
                {
                    std::string time = clock.GetTime().ToString();
                    printf("Meter time: %s\r\n", time.c_str());
                }
            }
        }
    }

    return ret;
}

int Modem::ReadRegister()
{
    int attributeIndex = 2;
    int ret;
    std::vector<CGXByteBuffer> data;
    CGXReplyData reply;

    CGXDLMSRegister reg("1.1.1.8.0.255", 0, DLMS_UNIT_ACTIVE_ENERGY, CGXDLMSVariant(0UL)); // ImportActiveEnergyAggregate

    //Read data from the meter.
    ret = mClient.Read(&reg, attributeIndex, data);

    if ((ret == 0) && (data.size() > 0))
    {
        CGXByteBuffer gxPacket = data.at(0);
        std::string request((const char *)gxPacket.GetData(), gxPacket.GetSize());

        if (Send(request, PRINT_HEX))
        {
            std::string data;
            sleep(1); // let the communication go on

            if (WaitForData(data, 5))
            {
                ret = data.size();
                Printer(data.c_str(), data.size(), PRINT_HEX);

                CGXByteBuffer buffer;
                buffer.Set(data.data(), data.size());

                // Lecture de l'heure:
                             // 7ea021030002002352f04fe6e700 c401c1 00 09 0c07e0010906152638ff80000095237e

                // Bad: 7EA0140300020023521969E6E700 C401C10103 73827E


                CGXReplyData reply;
                mClient.GetData(buffer, reply);
                CGXDLMSVariant replyData = reply.GetValue();
                if (mClient.UpdateValue(reg, 2, replyData) == 0)
                {
                    printf("ImportActiveEnergyAggregate: %d\r\n", reg.GetValue().ToInteger());
                }
            }
        }
    }

    return ret;
}


bool Modem::Open(const std::string &comport, uint32_t baudrate)
{
    bool ret = false;
    mSerialHandle = serial_open(comport.c_str());

    if (mSerialHandle >= 0)
    {
        if (serial_setup(mSerialHandle, baudrate) == 0)
        {
            ret = true;
        }
    }
    return ret;
}




int Modem::Send(const std::string &data, PrintFormat format)
{
    int ret = -1;

    // Print request
    puts("====> Sending: ");
    Printer(data.c_str(), data.size(), format);
    puts("\r\n");

    if (mUseTcpGateway)
    {
        // TODO
      //  socket.write(data);
      //  socket.flush();
    }
    else
    {
        ret = serial_write(mSerialHandle, data.c_str(), data.size());
    }

    return ret;
}


bool  Modem::PerformCosemRead()
{
    bool ret = false;
    static bool onetime = false;

    switch(mCosemState)
    {
        case HDLC:
            printf("** Sending HDLC...\r\n");
            if (ConnectHdlc() > 0)
            {
               printf("** HDLC success!\r\n");
               ret = true;
               mCosemState = ASSOCIATION_PENDING;
            }
            else
            {
               printf("** Cannot connect to meter.\r\n");
            }
         break;
        case ASSOCIATION_PENDING:

            printf("** Sending AARQ...\r\n");
            if (ConnectAarq() > 0)
            {
               printf("** AARQ success!\r\n");
               ret = true;
               mCosemState = ASSOCIATED;
            }
            else
            {
               printf("** Cannot AARQ to meter.\r\n");
            }
            break;
        case ASSOCIATED:
            if (!onetime)
            {
                onetime = true;
                printf("** Sending ReadClock request...\r\n");
                if (ReadClock() > 0)
                {
                   printf("** Read clock success!\r\n");
                   ret = true;

                   printf("** Sending ReadRegister request...\r\n");
                   if (ReadRegister() > 0)
                   {
                       printf("** Read register success!\r\n");
                   }
                   else
                  {
                     printf("** Cannot read register from meter.\r\n");
                  }
                }
                else
                {
                   printf("** Cannot read clock from meter.\r\n");
                }
                break;
            }
            break;

        default:
            break;

    }

    return ret;
}


// Global state chart
bool Modem::PerformTask(const std::string &phone, int client, const std::string &lls)
{
    bool ret = false;

    switch (mModemState)
    {
        case DISCONNECTED:
            if (Dial(phone) > 0)
            {
               printf("** Modem dial success!\r\n");
               ret = true;
               mModemState = CONNECTED;
            }
            else
            {
               printf("** Dial failed.\r\n");
            }

            break;
        case CONNECTED:
        {
            CGXByteBuffer pass;
            pass.AddString(lls);

            mClient.m_Settings.SetPassword(pass);
            mClient.m_Settings.SetClientAddress(client);
            ret = PerformCosemRead();
            break;
        }
        default:
            break;
    }
    return ret;
}

int main(int argc, char **argv)
{
    bool ok = true;
    Modem modem;

    setbuf(stdout, NULL); // disable printf buffering

    if (argc >= 5)
    {
        std::string port(argv[1]);
        std::string phone(argv[2]);
        int client = atoi(argv[3]);
        std::string lls(argv[4]);

        // Before application, test connectivity
        if (modem.Open(port, 9600))
        {
            // create reader thread
            modem.Initialize();
            printf("** Serial port success!\r\n");
            if (modem.Test() > 0)
            {
                printf("** Modem test success!\r\n");
            }
            else
            {
                printf("** Modem test failed.\r\n");
                ok = false;
            }
        }
        else
        {
            printf("** Cannot open serial port.\r\n");
            ok = false;
        }

        while(ok)
        {
            sleep(1);
            ok = modem.PerformTask(phone, client, lls);
        }
    }
    else
    {
        printf("Usage: cosem_client /dev/ttyUSB0 0244059867 2 ABCDEFGH\r\n");
    }

    printf("** Exit task loop, waiting for reading thread...\r\n");
    modem.WaitForStop();

    return 0;

}

