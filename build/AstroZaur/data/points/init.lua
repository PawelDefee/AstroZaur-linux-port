local chart = ...
local point

point = chart.newpoint("swiss", "Sun")

point.avr_speed = 0.9856
point.dotsize = pref.chart.dot
point.color = "#FF5000"
point.category = "body"
point.id = 0
point.bitmap = "data/pixmaps/sun.gif"
point.info = true

point = chart.newpoint("swiss", "Moon")

point.avr_speed = 13.176
point.dotsize = pref.chart.dot
point.color = "#008000"
point.category = "body"
point.id = 1
point.bitmap = "data/pixmaps/moon.gif"
point.info = true

point = chart.newpoint("swiss", "Mercury")

point.avr_speed = 0.9856
point.dotsize = pref.chart.dot
point.color = "#00A0A0"
point.category = "major"
point.id = 2
point.bitmap = "data/pixmaps/mercury.gif"
point.info = true

point = chart.newpoint("swiss", "Venus")

point.avr_speed = 0.9856
point.dotsize = pref.chart.dot
point.color = "#00B000"
point.category = "major"
point.id = 3
point.bitmap = "data/pixmaps/venus.gif"
point.info = true

point = chart.newpoint("swiss", "Mars")

point.avr_speed = 0.524
point.dotsize = pref.chart.dot
point.color = "#FF0000"
point.category = "major"
point.id = 4
point.bitmap = "data/pixmaps/mars.gif"
point.info = true

point = chart.newpoint("swiss", "Jupiter")

point.avr_speed = 0.0829
point.dotsize = pref.chart.dot
point.color = "#0000FF"
point.category = "minor"
point.id = 5
point.bitmap = "data/pixmaps/jupiter.gif"
point.info = true

point = chart.newpoint("swiss", "Saturn")

point.avr_speed = 0.0335
point.dotsize = pref.chart.dot
point.color = "#000000"
point.category = "minor"
point.id = 6
point.bitmap = "data/pixmaps/saturn.gif"
point.info = true

--

point = chart.newpoint("swiss", "Uranus")

point.avr_speed = 0.0116
point.dotsize = pref.chart.dot
point.color = "#0080FF"
point.category = "minor"
point.id = 7
point.bitmap = "data/pixmaps/uranus.gif"
point.info = true

point = chart.newpoint("swiss", "Neptune")

point.avr_speed = 0.0059
point.dotsize = pref.chart.dot
point.color = "#0000C0"
point.category = "minor"
point.id = 8
point.bitmap = "data/pixmaps/neptune.gif"
point.info = true

point = chart.newpoint("swiss", "Pluto")

point.avr_speed = 0.0039
point.dotsize = pref.chart.dot
point.color = "#C00000"
point.category = "minor"
point.id = 9
point.bitmap = "data/pixmaps/pluto.gif"
point.info = true

-- Nodes

point = chart.newpoint("swiss", "NNode")

point.avr_speed = -0.0529
point.dotsize = pref.chart.dot
point.color = "#808080"
point.category = "fictitious"
point.id = 11
point.bitmap = "data/pixmaps/nnode.gif"
point.info = true

point = chart.newpoint("lua", "SNode")

point.avr_speed = -0.0529
point.dotsize = pref.chart.dot
point.color = "#808080"
point.category = "fictitious"
point.bitmap = "data/pixmaps/snode.gif"
point.info = true

function point:CALC(chart)
    local point = chart.point.NNode
    return point.lon + 180, point.lat, point.speed
end

-- Asc Dsc

point = chart.newpoint("axis", "Asc")

point.color = "#00B000"
point.category = "axis"
point.id = 0
point.bitmap = "data/pixmaps/asc.gif"
point.linewidth = 3
point.info = true
point.aspected = false
point.aspecting = false

point = chart.newpoint("lua", "Dsc")

point.dotsize = 0
point.color = "#00B000"
point.category = "axis"
point.linewidth = 2
point.aspected = false
point.aspecting = false

function point:CALC(chart)
    local point = chart.point.Asc
    return point.lon + 180, point.lat, point.speed
end

-- Mc Ic

point = chart.newpoint("axis", "Mc")

point.color = "#B00000"
point.category = "axis"
point.id = 1
point.bitmap = "data/pixmaps/mc.gif"
point.linewidth = 3
point.info = true
point.aspected = false
point.aspecting = false

point = chart.newpoint("lua", "Ic")

point.dotsize = 0
point.color = "#B00000"
point.category = "axis"
point.linewidth = 2
point.aspected = false
point.aspecting = false

function point:CALC(chart)
    local point = chart.point.Mc
    return point.lon + 180, point.lat, point.speed
end

-- House

local house = { 
    { name = "I",	color = "#00B000" },
    { name = "II",	color = "#555555", view = true },
    { name = "III",	color = "#555555", view = true },
    { name = "IV" ,	color = "#00B000" },
    { name = "V",	color = "#555555", view = true },
    { name = "VI",	color = "#555555", view = true },
    { name = "VII" ,	color = "#00B000" },
    { name = "VIII",	color = "#555555", view = true },
    { name = "IX",	color = "#555555", view = true },
    { name = "X",	color = "#00B000" },
    { name = "XI",	color = "#555555", view = true },
    { name = "XII",	color = "#555555", view = true }
}

for i, n in ipairs(house) do
    point = chart.newpoint("house", n.name)
    
    point.id = i
    point.color = n.color
    point.view = n.view
    point.info = false
    point.aspected = false
    point.aspecting = false
    point.category = "house"
end
