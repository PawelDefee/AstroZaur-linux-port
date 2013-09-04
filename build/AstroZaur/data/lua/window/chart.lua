local chart_n = 1

function get_natal(win)
    local chart = win.active
    local res
    
    if chart then
	res = gui.choice("Whats about natal chart?", "Cancel", "*Active", "New")
	
	if res == 0 then 
	    return nil
	end
    end
    
    if not chart or res == 2 then
	chart = program.newchart("natal", "Natal"..chart_n)
	chart_n = chart_n + 1
	win.show(chart, true)
	win.active = chart
    end
    
    return chart
end

function new_natal(win)
    local chart = program.newchart("natal", "Natal"..chart_n)
    
    chart_n = chart_n + 1

    win.active = chart
    win.show(chart, true)
    win.aspected = { chart }

    return chart
end

function new_raw(win)
    local chart = program.newchart("raw", "Raw"..chart_n)
    
    chart_n = chart_n + 1

    win.show(chart, true)
    win.aspected = { chart }

    return chart
end

function new_eqlipse(win)
    local natal = get_natal(win)
    local chart
    
    if natal then
	chart = program.newchart("eqlipse", "Eqlipse"..chart_n)
    
	chart_n = chart_n + 1

	chart.parent = natal
	
	win.show(chart, true)
	win.aspected = { natal, chart }
    end
        
    return chart
end

function new_progres(win)
    local natal = get_natal(win)
    local chart
    
    if natal then
	chart = program.newchart("progres", "Progres"..chart_n)
    
	chart_n = chart_n + 1

	chart.parent = natal
	
	win.show(chart, true)
	win.aspected = { natal, chart }
    end
        
    return chart
end

function new_zodiac_direct(win)
    local natal = get_natal(win)
    local chart
    
    if natal then
	chart = program.newchart("zodiac direct", "Zodiac direct"..chart_n)
    
	chart_n = chart_n + 1

	chart.parent = natal
	
	win.show(chart, true)
	win.aspected = { natal, chart }
    end
        
    return chart
end

function new_equator_direct(win)
    local natal = get_natal(win)
    local chart
    
    if natal then
	chart = program.newchart("equator direct", "Equator direct"..chart_n)
    
	chart_n = chart_n + 1

	chart.parent = natal
	
	win.show(chart, true)
	win.aspected = { natal, chart }
    end
        
    return chart
end

function new_true_direct(win)
    local natal = get_natal(win)
    local chart
    
    if natal then
	chart = program.newchart("true direct", "True direct"..chart_n)
    
	chart_n = chart_n + 1

	chart.parent = natal
	
	win.show(chart, true)
	win.aspected = { natal, chart }
    end
        
    return chart
end

function new_arc_direct(win)
    local natal = get_natal(win)
    local chart
    
    if natal then
	chart = program.newchart("arc direct", "Arc direct"..chart_n)
    
	chart_n = chart_n + 1

	chart.parent = natal
	chart.arc = chart.point.Sun
	
	win.show(chart, true)
	win.aspected = { natal, chart }
    end
        
    return chart
end

function new_solar(win)
    local natal = get_natal(win)
    local chart
    
    if natal then
	chart = program.newchart("revolution", "Solar"..chart_n)
    
	chart_n = chart_n + 1

	chart.parent = natal
	chart.revolution = natal.point.Sun
	chart.backward = true
	
	win.show(chart, true)
	win.aspected = { natal, chart }
    end
        
    return chart
end

function new_lunar(win)
    local natal = get_natal(win)
    local chart
    
    if natal then
	chart = program.newchart("revolution", "Lunar"..chart_n)
    
	chart_n = chart_n + 1

	chart.parent = natal
	chart.revolution = natal.point.Moon
	chart.backward = true
	
	win.show(chart, true)
	win.aspected = { natal, chart }
    end
        
    return chart
end

-- Callbacks

local function hide(item)
    local win = item.window

    if win.active then
	win.show(win.active, not win.show(win.active))
	win.window.redraw()
    end
end

local function edit(item)
    local win = item.window

    if win.active then
	win.active.edit()
    end
end

local function start(item)
    local win = item.window
    
    win.projection().start = win.active.point[item.point]
    win.window.redraw()
end

local function new_chart(item)
    local win = item.window
    local chart = item.func(win)
    
    if chart then 
	win.projection().start = chart.point.Asc

    	program.calc()
	program.draw()
    
	if pref.beginer then 
	    chart.edit()
	end
    end
end

-- Start

local window = ...
local menu = window.menu[3]

-- New charts

local new = menu.itemgroup("New")

local i1, i2, i3, i4, i5, i6, i7, i8, i9, i10 = new.item(
    { label = "Natal", callback = new_chart },
    { label = "Raw", callback = new_chart },
    { label = "Eqlipse", callback = new_chart },
    { label = "Progression", callback = new_chart },
    { label = "Direction/Zodiacal", callback = new_chart },
    { label = "Direction/Equatorial", callback = new_chart },
    { label = "Direction/True", callback = new_chart },
    { label = "Direction/Sun arc", callback = new_chart },
    { label = "Solar", callback = new_chart },
    { label = "Lunar", callback = new_chart }
)

i1.window, i1.func = window, new_natal
i2.window, i2.func = window, new_raw
i3.window, i3.func = window, new_eqlipse

i4.window, i4.func = window, new_progres

i5.window, i5.func = window, new_zodiac_direct
i6.window, i6.func = window, new_equator_direct
i7.window, i7.func = window, new_true_direct
i8.window, i8.func = window, new_arc_direct

i9.window, i9.func = window, new_solar
i10.window, i10.func = window, new_lunar

-- Active chart actions

local i1, i2, i3, i4 = menu.item(
    { label = "Edit...", key = "F4", callback = edit },
    { label = "Hide\\/Unhide", key = " ", callback = hide },
    { label = "Start/Asc", key = "S", callback = start },
    { label = "Start/Aries", key = "A", callback = start }
)

i1.window = window
i2.window = window

i3.window, i3.point = window, "Asc"
i4.window, i4.point = window, "Aries"

-- AddOn's

menu.divider()

local addon = menu.itemgroup("AddOn")
load_addon(window, addon, "data/lua/window/chart")
