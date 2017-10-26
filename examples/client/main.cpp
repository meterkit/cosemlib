
#include <iostream>
#include "JsonReader.h"
#include "CosemClient.h"


Device device;

bool ParseComFile(Modem &modem, Cosem &cosem, Serial &serial, const std::string &file)
{
    JsonReader reader;
    JsonValue json;
    bool ok = false;

    if (reader.ParseFile(json, "comm.json"))
    {
        JsonValue obj = json.FindValue("modem");

        JsonValue dev = obj.FindValue("device");
        if (dev.IsString())
        {
            std::string devString = dev.GetString();
            if (devString == "modem")
            {
                device = MODEM;
            }
            else
            {
                device = NONE;
            }
        }


        if (obj.IsObject())
        {
            JsonValue val = obj.FindValue("phone");
            if (val.IsString())
            {
                modem.phone = val.GetString();

                val = obj.FindValue("init");
                if (val.IsString())
                {
                    modem.init = val.GetString();
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

        if (ok)
        {
            ok = false;
            obj = json.FindValue("serial");
            if (obj.IsObject())
            {
                JsonValue val = obj.FindValue("port");
                if (val.IsString())
                {
                    serial.port = val.GetString();
                    val = obj.FindValue("baudrate");
                    if (val.IsInteger())
                    {
                        serial.baudrate = static_cast<unsigned int>(val.GetInteger());
                        ok = true;
                    }
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

    if (reader.ParseFile(json, "objectlist.json"))
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
        Serial serial;
        std::vector<Object> list;

        std::string commFile(argv[1]); // First file is the communication parameters
        std::string objectsFile(argv[2]); // Second is the objects to retrieve
        cosem.start_date = std::string(argv[3]); // startDate for the profiles
        cosem.end_date = std::string(argv[4]); // endDate for the profiles

        ok = ParseComFile(modem, cosem, serial, commFile);

        if (ok)
        {
            ok = ParseObjectsFile(list, objectsFile);
            std::cout << "** Using LLS: " << cosem.lls << std::endl;

            client.SetDevice(device);

            if (device == MODEM)
            {
                std::cout << "** Using Modem device" << std::endl;
            }
        }

        // Before application, test connectivity
        if (client.Open(serial.port, serial.baudrate))
        {
            // create reader thread
            client.Initialize();
            printf("** Serial port success!\r\n");
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
        printf("Usage example: cosem_client /path/comm.json /another/objectlist.json 2017-08-01.00:00:00 2017-10-23.14:55:02\r\n");
        puts("\r\nDate-time format: %Y-%m-%d.%H:%M:%S");
    }

    printf("** Exit task loop, waiting for reading thread...\r\n");
    client.WaitForStop();

    return 0;

}

