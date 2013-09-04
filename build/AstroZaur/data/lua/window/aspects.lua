local window = ...

-- window.menu[2].divider()

local aspect_group = window.menu[2].itemgroup("Aspects")
local path = "data/lua/window/aspects"

aspects = {}

for file in lfs.dir(path) do
    local f = path..'/'..file
    local attr = lfs.attributes(f)
    
    if attr.mode == "file" then
	local chunk, err = loadfile(f)
    
	if chunk then
	    local plug = chunk(window)
	
	    if plug.label and plug.func then
		print("\t", file, plug.label)

		item = aspect_group.item(plug)
		item.func = plug.func
		item.window = window
		
		item.callback = function(self)
		    self.func(self.window)
		    window.aspected = window.aspected
		    program.calc()
		    program.draw()
		end
		
		aspects[plug.label] = plug.func
	    else
		gui.alert("Addon not contain label and callback")
	    end
	else
	    gui.alert("Error start plug "..err)
	end
    end
end

-- Start

local default = aspects[pref.aspects]

if default then
    default(window)
end
