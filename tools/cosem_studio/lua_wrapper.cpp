
#include <windows.h>
#include <iostream>
#include <thread>
#include "lua_wrapper.h"
#include "main.h"

IScript::IListener *LuaWrapper::mListener = NULL;
ThreadQueue<std::string> LuaWrapper::mQueue;
std::thread LuaWrapper::mThread;
bool LuaWrapper::mQuit = false;
lua_State *LuaWrapper::state = NULL;


int LuaWrapper::DelayMs(lua_State *L)
{
    long msecs = lua_tointeger(L, -1);
    Sleep(msecs);
    return 0;                  /* No items returned */
}

int LuaWrapper::LuaPrint(lua_State *L)
{
    int nArgs = lua_gettop(L);
    int i;
    lua_getglobal(L, "tostring");
    std::string ret;//this is where we will dump the output
    //make sure you start at 1 *NOT* 0
    for(i = 1; i <= nArgs; i++)
    {
        const char *s;
        lua_pushvalue(L, -1);
        lua_pushvalue(L, i);
        lua_call(L, 1, 1);
        s = lua_tostring(L, -1);
        if (s == NULL)
        {
            return luaL_error(L, LUA_QL("tostring") " must return a string to ", LUA_QL("print"));
        }

        ret.append(s);
        lua_pop(L, 1);
    }
    //Send it wherever
    if (mListener != NULL)
    {
        mListener->Print(ret);
    }

    return 0;
}

void LuaWrapper::EntryPoint(void *pthis)
{
    (void) pthis;

    for(;;)
    {
        std::string script;
        mQueue.WaitAndPop(script);
        if (mQuit)
        {
            break;
        }
        else
        {
            if (script.size() > 0)
            {
                int retCode = luaL_dostring(state, script.c_str());
                if (mListener != NULL)
                {
                    mListener->Result(retCode);
                }
            }
        }
    }
}

int LuaWrapper::Initialize()
{
    mThread = std::thread(LuaWrapper::EntryPoint, this);

    // on crée un contexte d'exécution de Lua
    state = luaL_newstate();
    if (state == NULL)
    {
        std::cout <<  "cannot create state: not enough memory" << std::endl;
        return EXIT_FAILURE;
    }

    // Load lua standard library
    luaL_openlibs(state);

    // Constants
    lua_pushstring(state, SHADDAM_VERSION);
    lua_setglobal(state, "_SHADDAM_VERSION");

    // Additional functions exported to Lua world
    lua_register(state, "delay_ms", DelayMs );

    return 0;
}

void LuaWrapper::Close()
{
    // Send a dummy message to exit the thread
    mQuit = true;
    mQueue.Push("");
    mThread.join();

    // Shutdown lua
    lua_close(state);
}

void LuaWrapper::RedirectOutput(IScript::IListener *printer)
{
    mListener = printer;
    // Print overload
    lua_register(state, "print", LuaWrapper::LuaPrint );
}

void LuaWrapper::DisableOutputRedirection()
{
    mListener = NULL;
}

int LuaWrapper::DoFile(const std::string &fileName)
{
    return luaL_dofile(state, fileName.c_str());
}

void LuaWrapper::Execute(const std::string &script)
{
    mQueue.Push(std::string(script));
}



// END OF FILE
