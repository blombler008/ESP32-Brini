#ifndef LUA_HANDLER_HPP
#define LUA_HANDLER_HPP
#include <LuaWrapper.h> 
#include <Adafruit_NeoPixel.h>
#include "PrintHelper.hpp"
#include "FSWrapper.hpp"

class LuaHandler {
    public:
        LuaHandler(bool formatOnFail = false);
        void initialize();
        void start(); 
        void executeLoop();
        void executeSetup();
        void wait(int hold);

    private:
        void lua_loop0(void);
        lua_State* L;
        LuaWrapper luaWrapper;
        FSWrapper fswrapper;
        bool formatOnFail;
        TaskHandle_t fs_lua; 
};

#endif