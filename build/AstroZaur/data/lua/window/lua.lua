local function menu_lua_load()
    local name = gui.filechooser("Load Lua file", "*.lua", ".")
    
    if name then
	local chunk, err = loadfile(name)
    
	if chunk then
    	    gui.message("Load ok")
	else
	    gui.alert("Load error "..err)
	end
    end
end

local function menu_lua_run()
    local name = gui.filechooser("Run Lua file", "*.lua", ".")
    
    if name then
	local chunk, err = loadfile(name)
    
	if chunk then
	    local stat, res = pcall(chunk)
	    
	    if not stat then
		gui.alert("Start error "..res)
	    end
	else
	    gui.alert("Load error "..err)
	end
    end
end

local function menu_lua_console()
    if lua_console then
	lua_console.show()
    else
	lua_console = gui.window(320, 240, "Lua console")
	
	local edit = lua_console.multilineinput(0, 0, lua_console.w(), lua_console.h() - 25, "")
	local run = lua_console.button(0, edit.b(), edit.w(), 25, "Run")
	
	run.edit = edit
	
	run.callback = function(self)
	    local chunk, res = loadstring(self.edit.value())
	    
	    if chunk then
		local stat, res = pcall(chunk)
		
		if not stat then
		    gui.alert("Error "..res)
		end
	    else
		gui.alert("Error "..res)
	    end
	end
	
	lua_console.resizable(edit)
	lua_console.show()
    end
end

-- Start

local window = ...

local lua = window.menu.itemgroup("Lua")
    
lua.item(
    { label="Load file", key="Alt+L", callback = menu_lua_load },
    { label="Run file", key="Alt+R", callback = menu_lua_run }, 
    { label="Console", key="Alt+C", callback = menu_lua_console },
    { label="---" }
)
	
local addon = lua.itemgroup("AddOn")
    
load_addon(window, addon, "data/lua/window/lua")
