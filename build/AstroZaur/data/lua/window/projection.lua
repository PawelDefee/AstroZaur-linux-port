local function eqliptic(window)
    local proj = window.projection("eqliptic")

    proj.type = "eqliptic"
    proj.color = "#D0EFFF"
    proj.bitmap = {
	"data/pixmaps/aries.gif",
	"data/pixmaps/taurus.gif",
	"data/pixmaps/gemini.gif",
	"data/pixmaps/cancer.gif",
	"data/pixmaps/leo.gif",
	"data/pixmaps/virgo.gif",
	"data/pixmaps/libra.gif",
	"data/pixmaps/scorpio.gif",
	"data/pixmaps/sagittarius.gif",
	"data/pixmaps/capricorn.gif",
	"data/pixmaps/aquarius.gif",
	"data/pixmaps/pisces.gif"
    }

    proj.width = pref.proj.width
    proj.cw = pref.proj.cw
    
    return proj
end

local function horizont(window)
    local proj = window.projection("horizont")

    proj.type = "horizont"
    proj.color = "#AFFFB3"
    proj.bitmap = {
	"data/pixmaps/north.gif",
	"data/pixmaps/east.gif",
	"data/pixmaps/south.gif",
	"data/pixmaps/west.gif"
    }

    proj.width = pref.proj.width
    
    return proj
end

local function h1(window)
    local proj = window.projection("dial")

    proj.type = "dial"
    proj.color = "#D0EFFF"
    proj.size = 360
    proj.step = 10
    proj.fontsize = 10

    proj.width = pref.proj.width
    proj.cw = pref.proj.cw
    
    return proj
end

local function h4(window)
    local proj = window.projection("dial")

    proj.type = "dial"
    proj.color = "#D0EFFF"
    proj.size = 90
    proj.step = 5
    proj.fontsize = 10

    proj.width = pref.proj.width
    proj.cw = pref.proj.cw
    
    return proj
end

local function h8(window)
    local proj = window.projection("dial")

    proj.type = "dial"
    proj.color = "#D0EFFF"
    proj.size = 45
    proj.step = 1
    proj.fontsize = 10

    proj.width = pref.proj.width
    proj.cw = pref.proj.cw
    
    return proj
end

local function h16(window)
    local proj = window.projection("dial")

    proj.type = "dial"
    proj.color = "#D0EFFF"
    proj.size = 22.5
    proj.step = 1
    proj.fontsize = 10

    proj.width = pref.proj.width
    proj.cw = pref.proj.cw
    
    return proj
end

local function menu_callback(item)
    item.func(item.window)
    program.draw()
end

-- Start

window = ...

-- window.menu[2].divider()

local proj = window.menu[2].itemgroup("Projection")
    
local i1, i2, i3, i4, i5, i6 = proj.item(
    { label="Eqliptic", callback = menu_callback },
    { label="Horizont", callback = menu_callback },
    { label="---" },
    { label="Dial H1", callback = menu_callback },
    { label="Dial H4", callback = menu_callback },
    { label="Dial H8", callback = menu_callback },
    { label="Dial H16", callback = menu_callback }
)
    
i1.func, i1.window = eqliptic, window
i2.func, i2.window = horizont, window
i3.func, i3.window = h1, window
i4.func, i4.window = h4, window
i5.func, i5.window = h8, window
i6.func, i6.window = h16, window

eqliptic(window)
