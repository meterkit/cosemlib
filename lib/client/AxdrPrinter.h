
#ifndef AXDR_PRINTER
#define AXDR_PRINTER

#include <vector>
#include <sstream>
#include <cstdint>

struct element
{
    uint32_t counter;
    uint32_t size;
};



class AxdrPrinter
{

public:

    void Clear()
    {
        mStream.str("");
        mLevels.clear();
    }


    void Append(uint8_t type, uint32_t size, uint8_t *data);
    std::string Get();

private:
    void PrintIndent();
    static std::string DataToString(uint8_t type, uint32_t size, uint8_t *data);

    std::vector<element> mLevels;
    std::stringstream mStream;

};


#endif // AXDR_PRINTER

