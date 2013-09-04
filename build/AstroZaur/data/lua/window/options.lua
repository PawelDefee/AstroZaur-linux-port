local function html_view(name)
    if win_html == nil then
	win_html = gui.window(320, 200, "HTML")
	
	local html = win_html.html(0, 0, win_html.w(), win_html.h())
	html.textsize(13)
	
	win_html.resizable(html)
	win_html.html = html
    end

    win_html.show()
    win_html.html.file(name)
end

local function menu_help()
    html_view(pref.help)
end

local function menu_about()
    html_view(pref.about)
end

local function menu_log()
    log.show()
end

local function current_place()
    local get_place = atlas.open()

    function get_place:callback()
	self.close()

	if gui.choice(self.info, "No", "*Yes") == 1 then
	    pref.place = { }
	
	    pref.place.info = self.info
	    pref.place.lon = self.lon
	    pref.place.lat = self.lat
	    pref.place.zone = self.zone
	end
    end
end

local function beginer()
    pref.beginer = not pref.beginer
    
    local stat = pref.beginer and "on" or "off"
    
    gui.message("Beginer mode is "..stat)
end

local function cw_ccw()
    pref.proj.cw = not pref.proj.cw

    for _,win in pairs(program.window) do
	proj = win.projection()
	proj.cw = pref.proj.cw
    end
    
    program.draw()
end

-- Start

local window = ...
local options = window.menu.itemgroup("Options")

options.item(
    { label = "Log", callback = menu_log },
    { label = "Help", callback = menu_help },
    { label = "About", callback = menu_about },
    { label = "---" },
    { label = "Current place...", callback = current_place },
    { label = "Beginer on\\/off", callback = beginer },
    { label = "Zodiac CW\\/CCW", callback = cw_ccw }
)
