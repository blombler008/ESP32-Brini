#include "LuaHandler.hpp"
void printErrorRunningFunction(const char* func, const char* msg);
void printErrorMustReturn(const char* func, const char* type);

int readLuaInt(lua_State* L, const char* func) {
    if (!lua_isnumber(L, -1)) 
        printErrorMustReturn(func, "number");
    
    int val = (int) lua_tonumber(L, -1);
    lua_pop(L, 1);
    return val; 
}

LuaHandler::LuaHandler(bool formatOnFail) { 
    LuaHandler::formatOnFail = formatOnFail; 
    L = luaWrapper.L();
    int args = lua_gettop(L);
    dprintf("Lua Top: %i\n", args);
}

void LuaHandler::initialize() { 
    fswrapper.begin(formatOnFail);
    fswrapper.listDir("/", 0); 
    
    const String constants = (const String) luaWrapper.addConstants();
    luaWrapper.Lua_dostring(&constants); 
}

void LuaHandler::start() {
    xTaskCreatePinnedToCore([](void* o){ ((LuaHandler*)o)->lua_loop0(); }, "lua_loop", 10000, this, 1, &fs_lua, 1);  
}

uint32_t LuaHandler::executeSetPixel () {
    const char* function = "setPixel";
    int color[3] = {255, 255, 0};  
    lua_getglobal(L, function);
    if(lua_isfunction(L, -1)) {  
        int args = lua_gettop(L);
        if(lua_pcall(L, 0, 3, 0) != 0)
            printErrorRunningFunction(function, lua_tostring(L, -1));  
        
        args = lua_gettop(L) - args;
        for(int i=0; i<=args; i++) {
            color[i] = readLuaInt(L, function);
        }
    }
    return Adafruit_NeoPixel::ColorHSV(color[2], color[1], color[0]);
}

void LuaHandler::executeLoop() { 
    const char* function = "loop";
    lua_getglobal(L, function);
    if(lua_isfunction(L, -1)) { 
        if(lua_pcall(L, 0, 0, 0) != 0)
            printErrorRunningFunction(function, lua_tostring(L, -1)); 
    }  
}

void LuaHandler::executeSetup() { 
    const char* function = "setup";
    lua_getglobal(L, function);
    if(lua_isfunction(L, -1)) { 
        if(lua_pcall(L, 0, 0, 0) != 0)
            printErrorRunningFunction(function, lua_tostring(L, -1)); 
    }  
}

void LuaHandler::wait(int hold) { 
    long time = esp_timer_get_time();
    long next = time + hold;
        while(time < next) {
        time = esp_timer_get_time();
        portYIELD();
    }
}

void LuaHandler::lua_loop0(void) { 
    String result = luaWrapper.Lua_doFile("/littlefs/test.lua");
    if (!result.isEmpty()) Serial.println(result);
 
    executeSetup(); 
    while(true) {
        executeLoop(); 
        uint32_t color = executeSetPixel(); 

        // pixel.setPixelColor(0, color);
        // pixel.setBrightness(10);
        // pixel.show(); 

        wait(50000);
        portYIELD();
    }
    vTaskDelete(fs_lua);
}

void printErrorRunningFunction(const char *func, const char *msg) {
        printf("error running function '%s': %s", func, msg);
}

void printErrorMustReturn(const char* func, const char* type) {
    printf("function '%s' must return a %s\n", func, type); 
}