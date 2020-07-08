local rc4hex = require "rc4hex"

-- char t_Data[] = "569908541";
-- char t_Key[] = "VMI9yw0eTtQ0z14l";
local data = "522252147"
local key = "VMI9yw0eTtQ0z14l"
local en_string = rc4hex.en_hex(data, key)
--print(en_string)
freeswitch.consoleLog("info", en_string)

----en_string = "a3396500a76913e34b"
--data = rc4hex.de_hex(en_string, key)
--print(data)

---- time_test.lua
--local mylualib = require "mylualib"
--local now = mylualib.milli_time()
--print(os.date("%Y-%m-%d %H:%M:%S").. ":millisecond = " .. now)
