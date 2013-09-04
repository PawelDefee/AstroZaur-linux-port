require"lfs"

require"data/lua/utils"
require"data/lua/program"

require"data/division/sign"
require"data/division/triplicitet"
require"data/division/fas"
require"data/division/term"

print"=== Welcome to AstroZaur ==="

dofile"data/pref.lua"
dofile"data/text/en.lua"
	    
program.timezones(pref.timezones)
swe.path(pref.swe)
atlas.path(pref.atlas)

log.show()

local first = program.newwindow()

if pref.beginer and gui.choice("New natal?", "No", "*Yes") == 1 then
    local chart = new_natal(first)
    first.projection().start = chart.point.Asc

    program.calc()
    program.draw()
end

gui.run()
