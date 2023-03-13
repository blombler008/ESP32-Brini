#ifndef LUA_HANDLER_HPP
#define LUA_HANDLER_HPP
#include <LuaWrapper.h>

class LuaHandler {
    public:
        LuaHandler(lua_State* L);
        uint32_t executeSetPixel();
        void executeLoop();
        void wait(int hold);

    private:
        lua_State* L;
};

#endif