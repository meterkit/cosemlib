#ifndef LUA_WRAPPER_H
#define LUA_WRAPPER_H

#include "i_script.h"
#include "ThreadQueue.h"

/* Include the Lua API header files. */
extern "C" {
   #include <lua.h>
   #include <lauxlib.h>
   #include <lualib.h>
}

class LuaWrapper : public IScript
{

public:

    // Static methods
    static int DelayMs(lua_State *L);
    static int LuaPrint(lua_State *L);
    static void EntryPoint(void *pthis);

    // non-static methods
    int Initialize();
    void Close();
    void RedirectOutput(IScript::IListener *printer);
    void DisableOutputRedirection();
    int DoFile(const std::string &fileName);

    // From IScript
    virtual void Execute(const std::string &script);

private:
    static IScript::IListener *mListener;
    static ThreadQueue<std::string> mQueue;
    static std::thread mThread;
    static bool mQuit;

    static lua_State *state;
};

#endif // LUA_WRAPPER_H

