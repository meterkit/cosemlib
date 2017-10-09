
#include <iostream>
#include <cstdint>
#include <pthread.h>


#include "serial.h"
#include "util.h"

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

#if 0
void MainWindow::ReadClock()
{
    int attributeIndex = 2;
    int ret;
    std::vector<CGXByteBuffer> data;
    CGXReplyData reply;
    //Read data from the meter.
    ret = client.Read(&clock, attributeIndex, data);
    if ((ret == 0) && (data.size() > 0))
    {
        Send(data.at(0));
    }
}

void MainWindow::readSocket()
{
    mReply = socket.readAll();

    if (!timer.isActive())
        timer.start(200);

}



void MainWindow::AppendToRequest(const QByteArray &data)
{
    QString txt = ui->requestText->toPlainText();
    txt = txt + data + "\r\n";
    ui->requestText->setPlainText(txt);
}

void MainWindow::AppendToResponse(const QByteArray &data)
{
    QString txt = ui->responseText->toPlainText();
    txt = txt + data + "\r\n";
    ui->responseText->setPlainText(txt);
}



void MainWindow::Send(CGXByteBuffer &request)
{
    QByteArray req((const char *)request.GetData(), request.GetSize());
    Send(req);
}

void MainWindow::connectHdlc()
{
    if (modemState == CONNECTED)
    {
        if (cosemState == HDLC)
        {
            std::vector<CGXByteBuffer> data;

            // FIXME: SNRM does not seems to work, restest after changes made in HDLC framing
             int ret = 0;
             ret = client.SNRMRequest(data);

             if ((ret == 0) && (data.size() > 0))
             {
                 //CGXByteBuffer request = data.at(0);
                 //QByteArray req((const char *)request.GetData(), request.GetSize());
                 QByteArray req = QByteArray::fromHex("7EA0210002002303939A74818012050180060180070400000001080400000007655E7E");
                 Send(req);
             }
        }
/*
        //Get meter's send and receive buffers size.
        if (() != 0 ||
            (ret = ReadDataBlock(data, reply)) != 0 ||
            (ret = m_Parser->ParseUAResponse(reply.GetData())) != 0)
        {
            TRACE("SNRMRequest failed %d.\r\n", ret);
            return ret;
        }
        reply.Clear();
        if ((ret = m_Parser->AARQRequest(data)) != 0 ||
                (ret = ReadDataBlock(data, reply)) != 0 ||
                (ret = m_Parser->ParseAAREResponse(reply.GetData())) != 0)
        {
            if (ret == DLMS_DLMS_ERROR_CODE_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED)
            {
                TRACE1("Use Logical Name referencing is wrong. Change it!\r\n");
                return ret;
            }
            TRACE("AARQRequest failed %d.\r\n", ret);
            return ret;
        }

        */

        /*
        // 7EA0210002002303939A74818012050180060180070400000001080400000007655E7E adresse 17
        //QByteArray req = QByteArray::fromHex("7EA0210002481BF1932188818012050180060180070400000001080400000007655E7E"); // adresse 4621
        QByteArray req = QByteArray::fromHex("7EA0210002002303939A74818012050180060180070400000001080400000007655E7E"); // adresse 17

        // reponse : 7ea023030002002373c0488180140502008006HEX: 02008007040000000708040000000103327e

        //socket.write("/?!\r\n");
        socket.write(req);
        socket.flush();

        */
    }
}


void MainWindow::connectAarq()
{
    if (modemState == CONNECTED)
    {
        std::vector<CGXByteBuffer> data;

        int ret = 0;
        ret = client.AARQRequest(data);

        if ((ret == 0) && (data.size() > 0))
        {
          //  QByteArray req = QByteArray::fromHex("7EA0210002002303939A74818012050180060180070400000001080400000007655E7E");
            Send(data.at(0));
        }

        /*
        QByteArray req = QByteArray::fromHex("6036A1090607608574050801018A0207808B0760857405080201AC0A80084142434445464748BE10040E01000000065F1F04001C1E7D0000");

        socket.write(req);
        socket.flush();
        */
    }
}

#endif



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

    bool WaitForData(std::string &data);

    void * Reader();

    static void *thread_reader(void *context)
    {
        return ((Modem *)context)->Reader();
    }

    bool PerformTask(const std::string &phone);
    bool PerformCosemRead();


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
    pthread_mutex_t mDataMutex;
    pthread_mutex_t mCvMutex;
    pthread_cond_t  mCvCond;

};

Modem::Modem()
    : mModemState(DISCONNECTED)
    , mCosemState(HDLC)
    , mUseTcpGateway(false)
    , mSerialHandle(0)
    , mBufSize(0)
    , mThread(NULL)
    , mDataMutex(PTHREAD_MUTEX_INITIALIZER)
    , mCvMutex(PTHREAD_MUTEX_INITIALIZER)
    , mCvCond(PTHREAD_COND_INITIALIZER)
{

}


void Modem::Initialize()
{
    pthread_create(&mThread, NULL, &Modem::thread_reader, this);
}


void Modem::WaitForStop()
{
    pthread_join(mThread, NULL);
}

bool Modem::WaitForData(std::string &data)
{
    bool ret = false;

    pthread_mutex_lock( &mCvMutex );
    pthread_cond_wait( &mCvCond, &mCvMutex );
    pthread_mutex_unlock( &mCvMutex );

    pthread_mutex_lock( &mDataMutex );
    data = mData;
    pthread_mutex_unlock( &mDataMutex );

    if (data.size() > 0)
    {
        ret = true;
    }

    return ret;
}

void * Modem::Reader()
{
    printf("Reader thread started\r\n");

    while (1)
    {
        int ret = serial_read(mSerialHandle, &mBuffer[0], cBufferSize, 30);

        if (ret > 0)
        {
            printf("<==== Got data: %d bytes", ret);
            std::string data(&mBuffer[0], ret);

            Printer(data.c_str(), data.size(), PRINT_HEX);

            puts("\r\n");

            // Add data
            pthread_mutex_lock( &mDataMutex );
            mData += data;
            pthread_mutex_unlock( &mDataMutex );

            // Signal new data available
            pthread_mutex_lock( &mCvMutex );
            pthread_cond_signal( &mCvCond );
            pthread_mutex_unlock( &mCvMutex );
        }
        else if (ret == 0)
        {
            puts("Reader timeout!\r\n");
        }
        else
        {
            puts("Serial read error, exiting...\r\n");
            break;
        }
    }

    return NULL;
}



int Modem::ConnectHdlc()
{
    int ret = -1;
    static const std::string snrm = "7EA0210002002303939A74818012050180060180070400000001080400000007655E7E";

    int size = StringToBin(snrm, &mBuffer[0]);

    if (Send(std::string(&mBuffer[0], size), PRINT_HEX))
    {
        std::string data;
        sleep(1); // let the communication go on

        if (WaitForData(data))
        {
            ret = data.size();
            Printer(data.c_str(), data.size(), PRINT_HEX);
        }
    }

    return ret;
}

int Modem::Test()
{
    int ret = -1;

    if (Send("AT\r\n", PRINT_RAW) > 0)
    {
        std::string data;

        sleep(1); // let the modem answer

        if (WaitForData(data))
        {
            ret = data.size();
            Printer(data.c_str(), data.size(), PRINT_RAW);
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
    Printer(data.c_str(), data.size(), format);

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

int Modem::Dial(const std::string &phone)
{
    int ret = -1;

    std::string dialRequest = std::string("ATD") + phone + std::string("\r\n");

    if (Send(dialRequest, PRINT_RAW))
    {
        std::string data;
        sleep(20); // let the modem dial

        if (WaitForData(data))
        {
            ret = data.size();
            Printer(data.c_str(), data.size(), PRINT_RAW);
        }
    }

    return ret;
}



#if 0


void MainWindow::discModem()
{
    QByteArray disc("+++\r\n");
    QByteArray ath0("ATH0\r\n");

    AppendToRequest(disc);
    Send(disc);
    std::this_thread::sleep_for(std::chrono::seconds(1U));

    AppendToRequest(ath0);
    Send(ath0);

    modemState = DISCONNECTED;
    cosemState = HDLC;

    socket.close();
    serial.close();
    ui->connectButton->setText("Disconnect");
    connected = false;
}


void MainWindow::WriteToFile(QString data)
{
    QFile file("gps_output.wor");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
       return;

    QTextStream out(&file);
    out << data << "\n";

}

void MainWindow::startSlave()
{
    if (!connected)
    {
        modemState = DISCONNECTED;
        if (ui->radioTcp->isChecked())
        {
            const int Timeout = 5 * 1000;
            socket.connectToHost(ui->tcpAddress->text(), ui->tcpPort->value());
            if (!socket.waitForConnected(Timeout))
            {
                QString error = socket.errorString();
                processError(error);
                return;
            }
        }
        else
        {
            serial.close();
            serial.setPortName(ui->serialPortComboBox->currentText());

            serial.setDataBits(m_settingsDialog->settings().dataBits);
            serial.setBaudRate(m_settingsDialog->settings().baud);
            serial.setParity(m_settingsDialog->settings().parity);
            serial.setStopBits(m_settingsDialog->settings().stopBits);

            if (!serial.open(QIODevice::ReadWrite)) {
                processError(tr("Can't open %1, error code %2")
                             .arg(serial.portName()).arg(serial.error()));
                return;
            }

        }

        if (!ui->checkUseModem->isChecked())
        {
            modemState = CONNECTED;
        }
        else
        {
            QByteArray checkModem = "AT\r\n";
            AppendToRequest(checkModem);
            Send(checkModem, false);
        }

        connected = true;
        ui->connectButton->setText("Disconnect");
        statusBar()->showMessage(tr("Port open."), 5000);
    }
    else
    {
        if (ui->radioTcp->isChecked())
        {
            discModem();
        }
        else
        {
            serial.close();
        }

        connected = false;
        ui->connectButton->setText("Connect");
        statusBar()->showMessage(tr("Port closed."), 5000);
    }
}


void MainWindow::readRequest()
{
    while (!serial.atEnd()) {
      mReply += serial.read(serial.bytesAvailable());
    }

    if (!timer.isActive())
        timer.start(200);

}

void MainWindow::Decode(const QByteArray &response)
{
    if (modemState == CONNECTED)
    {
        CGXByteBuffer buffer;
        buffer.Set(response.data(), response.size());

        if (cosemState == HDLC)
        {
            if (client.ParseUAResponse(buffer) == 0)
            {
                cosemState = ASSOCIATION_PENDING;
            }
        } else if (cosemState == ASSOCIATION_PENDING)
        {
         //   if (client.ParseAAREResponse(buffer) == 0)
            {
                cosemState = ASSOCIATED;
            }
        } else if (cosemState == ASSOCIATED)
        {
            // Lecture de l'heure:
            // 7ea021030002002352f04fe6e700c401c100090c07e0010906152638ff80000095237e
            CGXReplyData reply;
            client.GetData(buffer, reply);
            CGXDLMSVariant replyData = reply.GetValue();
            if (client.UpdateValue(clock, 2, replyData) == 0)
            {
                std::string time = clock.GetTime().ToString();
                QByteArray timeString = "Meter time: " + QByteArray(time.c_str());
                AppendToResponse(timeString);
            }
        }
    }
    else
    {
        QString modemReply = response.toStdString() .c_str();

        switch(modemState)
        {
        case DISCONNECTED:
            if (modemReply.contains("OK"))
            {
                modemState = MODEM_OK;
            }
            break;
        case MODEM_OK:
            if (modemReply.contains("CONNECT"))
            {
                modemState = CONNECTED;
            }
            break;
        case CONNECTED:
        default:
             break;
        }
    }

    AppendToResponse(response.toHex());
}

void MainWindow::processTimeout()
{
    timer.stop();
    Decode(mReply);
    mReply.clear();
}

#endif

static const uint8_t snrm[] = {0x7E, 0xA0, 0x21, 0x00, 0x02, 0x00, 0x23, 0x03, 0x93, 0x9A, 0x74, 0x81, 0x80, 0x12,
                               0x05, 0x01, 0x80, 0x06, 0x01, 0x80, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00, 0x00, 0x07, 0x65, 0x5E, 0x7E };

#define BUF_IN_SIZE sizeof(buf_in)
#define BUF_OUT_SIZE sizeof(buf_out)


bool  Modem::PerformCosemRead()
{
    bool ret = false;

    switch(mCosemState)
    {
        case HDLC:
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
        default:
            break;

    }

    return ret;
}


// Global state chart
bool Modem::PerformTask(const std::string &phone)
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
            ret = PerformCosemRead();
            break;
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

    if (argc >= 3)
    {

        std::string port(argv[1]);
        std::string phone(argv[2]);

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
            ok = modem.PerformTask(phone);
        }
    }
    else
    {
        printf("Usage: cosem_client /dev/ttyUSB0 0244059867\r\n");
    }

    modem.WaitForStop();

    return 0;

}

