--[[
    TODO: add wifi getter and setter for pw and ssid
        eg. setWifiSSID(ssid) setWifiPassword(password)
            getWifiSSID()
            setWifiEnabled(enable)
            isWifiEnabled()


]]
print("Hello from Test. Lua here")

print("Current uptime: " .. math.floor(millis()))

function loop()
    print("Current uptime: " .. math.floor(millis()))
    delay(10000)
end