local function write(str)
    local logfile = io.open(_luadir.."\\log.txt", "a")
    logfile:write(str)
    logfile:flush()
    logfile:close()
    print(str)
end
function Q_(t)
    a_(require "inspect"(t))    
end
function a_(...)
    local str = ""
    local temp = {...}
    for i = 1, table.maxn(temp) do
        str = str..tostring(temp[i]).."  "
    end
    str = str.."\n"
    write("["..tostring(os.date()).."] "..str)
end

function A_( ... )
    local str = ""
    local temp = {...}
    for i = 1, table.maxn(temp) do
        str = str..tostring(temp[i]).."  "
    end
    str = "["..tostring(os.date()).."] "..str
    str = str.."\n"
    UTableUtil.log(str)
end

function errhandle(err)
    local err = err..'\n'..debug.traceback()..'\n'
    a_(err)
    UTableUtil.log(err)
end

function Xpcall(f)
    xpcall(f, errhandle)
end