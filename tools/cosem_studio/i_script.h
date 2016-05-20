#ifndef I_SCRIPT_H
#define I_SCRIPT_H

#include <string>

class IScript
{
public:
    class IListener
    {
    public:
        // Prints a message
        virtual void Print(const std::string &message) = 0;

        // script execution ended with the specified result code
        virtual void Result(int code) = 0;
    };

    virtual void Execute(const std::string &script) = 0;
};

#endif // I_SCRIPT_H

