--[[
    TODO: add wifi getter and setter for pw and ssid
        eg. setWifiSSID(ssid) setWifiPassword(password)
            getWifiSSID()
            setWifiEnabled(enable)
            isWifiEnabled()
]]
local hue = 0
local startTime

function setup()
    print("Hello from Test.")
    hue = 0
    startTime = millis()
    print("[LUA] StartTime: " .. math.floor(startTime))
end

function loop()
    hue = hue + 100
    if hue > 65535 then
        hue = 0
    end
end

function setPixel()
    return hue, 255, 255
end
