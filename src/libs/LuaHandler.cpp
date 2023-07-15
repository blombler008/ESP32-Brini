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

void LuaHandler::initialize(int sdFS_ss) { 
    fswrapper = (FSWrapper*) malloc(sizeof(FSWrapper));
    const FSWrapper wr(sdFS_ss);
    memcpy(fswrapper, &wr, sizeof(FSWrapper));
    
    fswrapper->begin(formatOnFail);
    fswrapper->listDir("/", 0); 
    
    const String constants = (const String) luaWrapper.addConstants();
    luaWrapper.Lua_dostring(&constants); 
}

void LuaHandler::start() {
    xTaskCreatePinnedToCore([](void* o){ ((LuaHandler*)o)->lua_loop0(); }, "lua_loop", 10000, this, 1, &fs_lua, tskNO_AFFINITY);  
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
    bool init = false;
    if(!init && fswrapper->isLittleFSMounted()) { 
        //String result = luaWrapper.Lua_doFile("/littlefs/autoinit.lua");
        //if (!result.isEmpty()) Serial.println(result);
        //init = true;
    }
    if(!init && fswrapper->isSDMounted()) {
        
        String result = luaWrapper.Lua_doFile("/sd/autoinit.lua");
        if (!result.isEmpty()) Serial.println(result);
        init = true;
    }

    if(init) { 
        executeSetup(); 
        
        Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, 18, NEO_GRB+NEO_KHZ800);
                
        pixel.begin();  
        while(true) {
            executeLoop();


            lua_getglobal(L, "setPixel");
            if(lua_isfunction(L, -1)) {
                lua_pcall(L, 0, 1, 0);
                int args = lua_gettop(L); 

                float hue;
                if (lua_isnumber(L, -1)) { 
                    hue = (float) lua_tonumber(L, -1);
                        
                    pixel.setPixelColor(0, pixel.ColorHSV(hue, 255, 255));
                    pixel.setBrightness(10);
                    pixel.show(); 
                };
                lua_pop(L, 1);   
            }

            wait(50000);
            portYIELD();
        }
    } 
    vTaskDelete(fs_lua);
}

void printErrorRunningFunction(const char *func, const char *msg) {
        printf("error running function '%s': %s", func, msg);
}

void printErrorMustReturn(const char* func, const char* type) {
    printf("function '%s' must return a %s\n", func, type); 
}