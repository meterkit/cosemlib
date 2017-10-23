
#include <iostream>
#include "JsonReader.h"
#include "CosemClient.h"


bool ParseComFile(Modem &modem, Cosem &cosem, const std::string &file)
{
    JsonReader reader;
    JsonValue json;
    bool ok = false;

    if (reader.ParseFile(json, file))
    {
        JsonValue obj = json.FindValue("modem");

        if (obj.IsObject())
        {
            JsonValue val = obj.FindValue("phone");
            if (val.IsString())
            {
                modem.phone = val.GetString();

                val = obj.FindValue("port");
                if (val.IsString())
                {
                    modem.port = val.GetString();
                    ok = true;
                }
            }
        }

        if (ok)
        {
            ok = false;
            obj = json.FindValue("cosem");
            if (obj.IsObject())
            {
                JsonValue val = obj.FindValue("lls");
                if (val.IsString())
                {
                    cosem.lls = val.GetString();
                    ok = true;
                }
            }
        }
    }
    else
    {
        std::cout << "** Error opening file: \r\n" << file;
    }

    return ok;
}

bool ParseObjectsFile(std::vector<Object> &list, const std::string &file)
{
    JsonReader reader;
    JsonValue json;
    bool ok = false;

    if (reader.ParseFile(json, file))
    {
        JsonValue val = json.FindValue("objects");
        if (val.IsArray())
        {
            JsonArray arr = val.GetArray();
            for (std::uint32_t i = 0U; i < arr.Size(); i++)
            {
                Object object;
                JsonValue obj = arr.GetEntry(i);

                val = obj.FindValue("name");
                if (val.IsString())
                {
                    object.name = val.GetString();
                }
                val = obj.FindValue("logical_name");
                if (val.IsString())
                {
                    object.ln = val.GetString();
                }
                val = obj.FindValue("class_id");
                if (val.IsInteger())
                {
                    object.class_id = static_cast<std::uint16_t>(val.GetInteger());
                }
                val = obj.FindValue("attribute_id");
                if (val.IsInteger())
                {
                    object.attribute_id = static_cast<std::int16_t>(val.GetInteger());
                }

                object.Print();
                list.push_back(object);
            }
            ok = true;
        }
    }

    return ok;
}


int main(int argc, char **argv)
{
    bool ok = true;
    CosemClient client;

    setbuf(stdout, NULL); // disable printf buffering

    puts("** DLMS/Cosem Client started\r\n");

    if (argc >= 5)
    {
        Cosem cosem;
        Modem modem;
        std::vector<Object> list;

        std::string commFile(argv[1]); // First file is the communication parameters
        std::string objectsFile(argv[2]); // Second is the objects to retrieve
        cosem.start_date = std::string(argv[3]); // startDate for the profiles
        cosem.end_date = std::string(argv[4]); // endDate for the profiles

        ok = ParseComFile(modem, cosem, commFile);

        if (ok)
        {
            ok = ParseObjectsFile(list, objectsFile);
            std::cout << "** Using LLS: " << cosem.lls << std::endl;
        }

        // Before application, test connectivity
        if (client.Open(modem.port, 9600))
        {
            // create reader thread
            client.Initialize();
            printf("** Serial port success!\r\n");
            if (client.Test() > 0)
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
            ok = client.PerformTask(modem, cosem, list);
        }
    }
    else
    {
        printf("Usage example: cosem_client /path/comm.json /another/objectlist.json 2001-08-23.14:55:02\r\n");
        puts("\r\nDate-time format: %Y-%m-%d.%H:%M:%S");
    }

    printf("** Exit task loop, waiting for reading thread...\r\n");
    client.WaitForStop();

    return 0;

}

