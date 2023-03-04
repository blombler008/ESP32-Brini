--[[
    TODO: add wifi getter and setter for pw and ssid
        eg. setWifiSSID(ssid) setWifiPassword(password)
            getWifiSSID()
            setWifiEnabled(enable)
            isWifiEnabled()


]]
print("Hello from Test.")

print("Current uptime: " .. math.floor(millis()))

local hue = 0
function loop() 
    hue = hue + 100
    if hue > 65535 then
        hue = 0
    end
end
function setPixel()
    return hue, 255, 255
end