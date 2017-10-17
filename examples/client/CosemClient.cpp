
#include <Util.h>
#include <iostream>
#include "CosemClient.h"
#include "serial.h"
#include "os_util.h"

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
    std::cout << Util::CurrentDateTime("%Y-%m-%d.%X") << ": ";

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


Semaphore::Semaphore(size_t count)
    : mCount{count}
{

}


void Semaphore::notify()
{
    std::lock_guard<std::mutex> lock{mMutex};
    ++mCount;
    mCv.notify_one();
}


//void Semaphore::wait() {
//    std::unique_lock<std::mutex> lock{mMutex};
//    mCv.wait(lock, [&]{ return mCount > 0; });
//    --mCount;
//}
//
//
//bool Semaphore::try_wait() {
//    std::lock_guard<std::mutex> lock{mMutex};
//
//    if (mCount > 0) {
//        --mCount;
//        return true;
//    }
//
//    return false;
//}


int Semaphore::wait_for(std::chrono::milliseconds timeout)
{
    std::unique_lock<std::mutex> lock{mMutex};
    int retCode = -1;

    if (mCv.wait_for(lock, timeout) != std::cv_status::timeout)
    {
        retCode = 0;
        --mCount;
    }
    return retCode;
}


//template<class Clock, class Duration>
//bool Semaphore::wait_until(const std::chrono::time_point<Clock, Duration>& t) {
//    std::unique_lock<std::mutex> lock{mMutex};
//    auto finished = mCv.wait_until(lock, t, [&]{ return mCount > 0; });
//
//    if (finished)
//        --mCount;
//
//    return finished;
//}


CosemClient::CosemClient()
    : mModemState(DISCONNECTED)
    , mCosemState(HDLC)
    , mUseTcpGateway(false)
    , mSerialHandle(0)
    , mBufSize(0)
    , mThread(0)
    , mClient(true, 1, 1, DLMS_AUTHENTICATION_LOW, "001CA021", DLMS_INTERFACE_TYPE_HDLC)
    , mTerminate(false)
    , mReadIndex(0U)
{
    mDataMutex = PTHREAD_MUTEX_INITIALIZER;
}


void CosemClient::Initialize()
{
    pthread_create(&mThread, NULL, &CosemClient::thread_reader, this);
}


void CosemClient::WaitForStop()
{
    mTerminate = true;
    pthread_join(mThread, NULL);
}


bool CosemClient::WaitForData(std::string &data, int timeout)
{
    bool ok = false;
    int ret = -1;

    // loop until timeout
    do {
        ret = mSem.wait_for(std::chrono::seconds(timeout));
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

void * CosemClient::Reader()
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
            mSem.notify();
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

int CosemClient::Test()
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

int CosemClient::Dial(const std::string &phone)
{
    int ret = -1;

    std::cout << "** Dial: " << phone << std::endl;

    std::string dialRequest = std::string("ATD") + phone + std::string("\r\n");

    if (Send(dialRequest, PRINT_RAW))
    {
        std::string data;

        if (WaitForData(data, 40))
        {
            ret = data.size();
            Printer(data.c_str(), data.size(), PRINT_RAW);
        }
    }

    return ret;
}

int CosemClient::ConnectHdlc()
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

        if (WaitForData(data, 4))
        {
            ret = data.size();
            Printer(data.c_str(), data.size(), PRINT_HEX);
        }
    }

    return ret;
}

int CosemClient::ConnectAarq()
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


int CosemClient::ReadClock()
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

        printf("** Sending ReadClock request...\r\n");
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
                // FIXME: parser les erreurs

                printf("** Read clock success!\r\n");
                CGXReplyData reply;
                mClient.GetData(buffer, reply);
                CGXDLMSVariant replyData = reply.GetValue();
                if (mClient.UpdateValue(clock, 2, replyData) == 0)
                {
                    std::string time = clock.GetTime().ToString();
                    std::cout << "\r\n|DATA|Clock|" <<  time << "|" << std::endl;
                }
            }
            else
            {
                ret = -1;
            }
        }
        else
        {
            ret = -1;
        }
    }

    if (ret < 0)
    {
       printf("** Cannot read clock from meter.\r\n");
    }

    return ret;
}

int CosemClient::ReadRegister(const Object &obj)
{
    int attributeIndex = 2;
    int ret;
    std::vector<CGXByteBuffer> data;
    CGXReplyData reply;

    CGXDLMSRegister reg(obj.ln, 0, DLMS_UNIT_ACTIVE_ENERGY, CGXDLMSVariant(0UL)); // ImportActiveEnergyAggregate

    //Read data from the meter.
    ret = mClient.Read(&reg, attributeIndex, data);

    if ((ret == 0) && (data.size() > 0))
    {
        CGXByteBuffer gxPacket = data.at(0);
        std::string request((const char *)gxPacket.GetData(), gxPacket.GetSize());

        printf("** Sending ReadRegister request...\r\n");
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

                // FIXME: parser les erreurs
                printf("** Read register success!\r\n");
                CGXReplyData reply;
                mClient.GetData(buffer, reply);
                CGXDLMSVariant replyData = reply.GetValue();
                if (mClient.UpdateValue(reg, 2, replyData) == 0)
                {
                    std::cout << "\r\n|DATA|ImportActiveEnergyAggregate|" <<  reg.GetValue().ToInteger() << "|" << std::endl;
                }
            }
        }
    }

    return ret;
}


bool CosemClient::Open(const std::string &comport, uint32_t baudrate)
{
    bool ret = false;

    std::cout << "** Opening serial port " << comport << " at " << baudrate << std::endl;
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




int CosemClient::Send(const std::string &data, PrintFormat format)
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


bool  CosemClient::PerformCosemRead(const std::vector<Object> &list)
{
    bool ret = false;

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
               mReadIndex = 0U;
               mCosemState = ASSOCIATED;
            }
            else
            {
               printf("** Cannot AARQ to meter.\r\n");
            }
            break;
        case ASSOCIATED:
        {
            if (mReadIndex < list.size())
            {
                Object obj = list[mReadIndex];

                if (obj.class_id == 8)
                {
                    (void) ReadClock();
                    ret = true;
                }
                else if (obj.class_id == 3)
                {
                    (void) ReadRegister(obj);
                    ret = true;
                }
                else
                {
                    std::cout << "** Unknown class ID.\r\n";
                }

                mReadIndex++;
            }
            break;
        }
        default:
            break;

    }

    return ret;
}


// Global state chart
bool CosemClient::PerformTask(const Modem &modem, const Cosem &cosem, const std::vector<Object> &list)
{
    bool ret = false;

    CGXByteBuffer pass;
    pass.AddString(cosem.lls);

    mClient.m_Settings.SetPassword(pass);
    mClient.m_Settings.SetClientAddress(cosem.client);

    switch (mModemState)
    {
        case DISCONNECTED:
            if (Dial(modem.phone) > 0)
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
            ret = PerformCosemRead(list);
            break;
        }
        default:
            break;
    }
    return ret;
}
