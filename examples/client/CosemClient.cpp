
#include <Util.h>
#include <iostream>
#include <iomanip>
#include "CosemClient.h"
#include "serial.h"
#include "os_util.h"

#include "hdlc.h"
#include "csm_array.h"

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
    std::lock_guard<std::mutex> lock(mMutex);
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
    std::unique_lock<std::mutex> lock(mMutex);
    int retCode = -1;

    while (!mCount)
    {
        if (mCv.wait_for(lock, timeout) != std::cv_status::timeout)
        {
            retCode = 0;
        }
        else
        {
            break;
        }
    }

    if (mCount)
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
    , mThread(0)
    , mClient(true, 1, 1, DLMS_AUTHENTICATION_LOW, "001CA021", DLMS_INTERFACE_TYPE_HDLC)
    , mTerminate(false)
    , mReadIndex(0U)
    , mDevice(NONE)
{

}


void CosemClient::Initialize(Device device, const Modem &modem, const Cosem &cosem, const hdlc_t &hdlc, const std::vector<Object> &list)
{
    CGXByteBuffer pass;
    pass.AddString(cosem.lls);

    mDevice = device;
    mModem = modem;
    mCosem = cosem;
    mHdlc = hdlc;
    mList = list;

    mClient.m_Settings.SetPassword(pass);
    mClient.m_Settings.SetClientAddress(hdlc.client_addr);
    mClient.m_Settings.SetServerAddress(hdlc.phy_address);


    if (mDevice != MODEM)
    {
        // Skip Modem state chart when no modem is in use
        mModemState = CONNECTED;
    }
    else
    {
        std::cout << "** Using Modem device" << std::endl;
        mModemState = DISCONNECTED;
    }

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


    mMutex.lock();
    data = mData;
    mData.clear();
    mMutex.unlock();

    // Echo cancellation
    if (data.compare(0, mSendCopy.size(), mSendCopy) == 0)
    {
        // remove echo from the string
        data = data.substr(mSendCopy.size());
    }

    if (data.size() > 0)
    {
        ok = true;
    }

    return ok;
}


bool CosemClient::HdlcProcess(hdlc_t &hdlc, std::string &data, int timeout)
{
    bool retCode = false;
    csm_array array;
    uint32_t number_of_frames = 0U;

    csm_array_init(&array, (uint8_t*)&mHdlcBuf[0], cBufferSize, 0U, 0U);

    bool loop = true;
    do
    {
        bool notified = true;
      //  int ret = mSem.wait_for(std::chrono::seconds(timeout));
        std::unique_lock<std::mutex> lock(mMutex);
        while (!mData.size())
        {
            if (mCv.wait_for(lock, std::chrono::seconds(timeout)) == std::cv_status::timeout)
            {
                notified = false;
                break;
            }
        }

        if (notified)
        {
            // We have something, add buffer
            if (!csm_array_write_buff(&array, (const uint8_t*)mData.c_str(), mData.size()))
            {
                loop = false;
                retCode = false;
            }
            mData.clear();

            if (loop)
            {
                uint8_t *ptr = csm_array_rd_data(&array);
                uint32_t size = csm_array_unread(&array);


//                printf("Ptr: 0x%08X, Unread: %d\r\n", (unsigned long)ptr, size);
//
//                puts("Sent frame: ");
//                print_hex(mSendCopy.c_str(), mSendCopy.size());
//                puts("\r\n");

                // the frame seems correct, check echo
                if (memcmp(mSendCopy.c_str(), ptr, mSendCopy.size()) == 0)
                {
                    // remove echo from the string
                    csm_array_reader_jump(&array, size);
                    ptr = csm_array_rd_data(&array);
                    size = csm_array_unread(&array);
                    puts("Echo canceled!\r\n");
                }

//                puts("Decoding: ");
//                print_hex((const char *)ptr, size);
//                puts("\r\n");

                hdlc.sender = HDLC_SERVER;
                int ret = hdlc_decode(&hdlc, ptr, size);
                if (ret == HDLC_OK)
                {
                    puts("Good packet\r\n");
                    number_of_frames++;

                    // God packet! Copy to cosem data
                    data.append((const char*)&ptr[hdlc.data_index], hdlc.data_size);

                    // Continue with next one
                    csm_array_reader_jump(&array, hdlc.frame_size);

                    // Test if it is a last HDLC packet
                    if ((hdlc.segmentation == 0U) &&
                        (hdlc.poll_final == 1U))
                    {
                        puts("Final packet\r\n");
                        retCode = true; // good Cosem packet
                        loop = false; // quit
                    }
                    else if (hdlc.segmentation == 1U)
                    {
                        puts("Segmentation packet: ");
                        hdlc_print_result(&hdlc, HDLC_OK);
                        // There are remaining frames to be received.
                        // At this time, it depends of the window size negociated
                        if (mHdlc.window_tx == number_of_frames)
                        {
                            // Send RR
                            hdlc.sender = HDLC_CLIENT;
                            hdlc.rrr = hdlc.sss + 1;
                            size = hdlc_encode_rr(&hdlc, (uint8_t*)&mBuffer[0], cBufferSize);
                            if (Send(std::string(&mBuffer[0], size), PRINT_HEX))
                            {
                                number_of_frames = 0U;
                            }
                        }
                    }
                }
                else
                {
                    // Maybe a partial packet, re-try later
                //    printf("Not a good packet: %d\r\n", ret);
                }
            }
        }
        else
        {
            // Timeout, we can't wait further for HDLC packets
            retCode = false;
            loop = false;
        }
    }
    while (loop);

    return retCode;
}

void * CosemClient::Reader()
{
    printf("Reader thread started\r\n");

    while (!mTerminate)
    {
        int ret = serial_read(mSerialHandle, &mBuffer[0], cBufferSize, 10);

        if (ret > 0)
        {
            printf("<==== Got data: %d bytes: ", ret);
            std::string data(&mBuffer[0], ret);

            Printer(data.c_str(), data.size(), PRINT_HEX);

            puts("\r\n");

            // Add data
            mMutex.lock();
            mData += data;
            mMutex.unlock();

            mCv.notify_one();

            // Signal new data available
        //    mSem.notify();
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

        if (WaitForData(data, 60))
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

    int size = hdlc_encode_snrm(&mHdlc, (uint8_t *)&mBuffer[0], cBufferSize);

    if (Send(std::string(&mBuffer[0], size), PRINT_HEX))
    {
        std::string data;
        hdlc_t hdlc;
        hdlc_init(&hdlc); // default values, in case of server does not have any parameters
        if (HdlcProcess(hdlc, data, 4))
        {
            ret = data.size();
            Printer(data.c_str(), data.size(), PRINT_HEX);

            // Decode UA
            ret = hdlc_decode_info_field(&hdlc, (const uint8_t *)data.c_str(), data.size());
            if (ret == HDLC_OK)
            {
                hdlc_print_result(&hdlc, ret);
                mHdlc.max_info_field_rx = hdlc.max_info_field_rx;
                mHdlc.max_info_field_tx = hdlc.max_info_field_tx;
                mHdlc.window_rx = hdlc.window_rx;
                mHdlc.window_tx = hdlc.window_tx;

                ret = 1U;
            }
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
            hdlc_t hdlc;
            hdlc_init(&hdlc);
            if (HdlcProcess(hdlc, data, 5))
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
                    std::cout << "\r\n|DATA|" << obj.name << "|" <<  reg.GetValue().ToInteger() << "|" << std::endl;
                }
            }
        }
    }

    return ret;
}


int CosemClient::ReadProfile(const Object &obj)
{
    int ret;
    std::vector<CGXByteBuffer> data;

    CGXDLMSProfileGeneric profile(obj.ln);

    CGXDLMSClock clock("0.0.1.0.0.255");
    profile.SetSortObject(&clock);

    std::tm tm_start = {};
    std::tm tm_end = {};
    std::stringstream ss(mCosem.start_date);
    ss >> std::get_time(&tm_start, "%Y-%m-%d.%H:%M:%S");

    std::stringstream ss2(mCosem.end_date);
    ss2 >> std::get_time(&tm_end, "%Y-%m-%d.%H:%M:%S");

    //Read data from the meter.
    ret = mClient.ReadRowsByRange(&profile, &tm_start, &tm_end, data);

    if ((ret == 0) && (data.size() > 0))
    {
        CGXByteBuffer gxPacket = data.at(0);
        std::string request((const char *)gxPacket.GetData(), gxPacket.GetSize());

        printf("** Sending ReadProfile request...\r\n");
        if (Send(request, PRINT_HEX))
        {
            std::string data;

            hdlc_t hdlc;

            if (HdlcProcess(hdlc, data, 5))
            {
                ret = data.size();
                Printer(data.c_str(), data.size(), PRINT_HEX);
/*
                gxPacket.Set(data.data(), data.size());
                CGXReplyData reply;
                if (mClient.GetData(gxPacket, reply) == DLMS_ERROR_CODE_OK)
                {
                    bool noError = true;
                    while (reply.IsMoreData() && noError)
                    {
                        gxPacket.Clear();
                        if ((ret = mClient.ReceiverReady(reply.GetMoreData(), gxPacket)) != 0)
                        {
                            noError = false;
                        }

                        printf("** Get next block...\r\n");
                        request.assign((const char *)gxPacket.GetData(), gxPacket.GetSize());
                        if (Send(request, PRINT_HEX))
                        {
                            if (HdlcProcess(mHdlc, data, 5))
                            {
                                ret = data.size();
                                Printer(data.c_str(), data.size(), PRINT_HEX);

                                gxPacket.Set(data.data(), data.size());
                                if (mClient.GetData(gxPacket, reply) != DLMS_ERROR_CODE_OK)
                                {
                                    noError = false;
                                }
                            }
                            else
                            {
                                noError = false;
                            }
                        }
                    }

                    // We have received all the packets
                    if (mClient.UpdateValue(profile, 2, reply.GetValue()) !=0 )
                    {
                        printf("** Read profile failure \r\n");
                        ret = -1;
                    }
                    else
                    {
                        printf("** Read profile success\r\n");
                        ret = 0;
                    }
                }
                */
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
        mSendCopy = data;
        ret = serial_write(mSerialHandle, data.c_str(), data.size());
    }

    return ret;
}


bool  CosemClient::PerformCosemRead()
{
    bool ret = false;

    switch(mCosemState)
    {
        case HDLC:
            printf("** Sending HDLC SNRM...\r\n");
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
            if (mReadIndex < mList.size())
            {
                Object obj = mList[mReadIndex];

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
                else if (obj.class_id == 7)
                {
                    (void) ReadProfile(obj);
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
bool CosemClient::PerformTask()
{
    bool ret = false;

    switch (mModemState)
    {
        case DISCONNECTED:
        {
            if (Test() > 0)
            {
                printf("** Modem test success!\r\n");

                mModemState = DIAL;
                ret = true;
            }
            else
            {
                printf("** Modem test failed.\r\n");
            }

            break;
        }

        case DIAL:
        {
            if (Dial(mModem.phone) > 0)
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
        }

        case CONNECTED:
        {
            ret = PerformCosemRead();
            break;
        }
        default:
            break;
    }
    return ret;
}

