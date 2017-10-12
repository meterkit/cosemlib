


#include "CosemClient.h"


int main(int argc, char **argv)
{
    bool ok = true;
    CosemClient modem;

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

