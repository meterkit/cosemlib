                   -- Welcome to Shaddam! -- 
                   -------------------------

-- Introduction
----------------

-- Shaddam is the successor of Manitoo, a DLMS/Cosem test tool to test meters.
-- It brings a true scripting language to write your automated scripts: the Lua language

-- Visit the official website: http://www.lua.org


-- Example
----------------

local x = os.clock()
local f = assert(io.open("out.txt", "w"))
local i=0

repeat
    f:write(i.."\n")    
    delay_ms(1000);
    i = i+1
    print(i.."\n")
until (os.clock()-x) >= 10

f:close()
print("fini")
-- fin du fichier

-- Introduction
----------------
