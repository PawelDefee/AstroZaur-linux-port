local function main(win)
    local eql
    local lon

    local green = {
        [0.15] = "#006600",
        [0.85] = "#33CC00",
        [1.0] = "#66FF99"
    }

    local black = {
        [0.15] = "#000000",
        [0.85] = "#999999",
        [1.0] = "#CCCCCC"
    }

    local red = {
        [0.15] = "#990000",
        [0.85] = "#FF0000",
        [1.0] = "#FF6666"
    }

    local orbis = {
	["Sun-Moon"] = 13.5,
	["Sun-Mercury"] = 11,
	["Sun-Venus"] = 11,
	["Sun-Mars"] = 11,
	["Sun-Jupiter"] = 12,
	["Sun-Saturn"] = 12,
	["Moon-Mercury"] = 9.5,
	["Moon-Venus"] = 9.5,
	["Moon-Mars"] = 9.5,
	["Moon-Jupiter"] = 10.5,
	["Moon-Saturn"] = 10.5,
	["Mercury-Venus"] = 7,
	["Mercury-Mars"] = 7,
	["Mercury-Jupiter"] = 8,
	["Mercury-Saturn"] = 8,
	["Venus-Mars"] = 7,
	["Venus-Jupiter"] = 8,
	["Venus-Saturn"] = 8,
	["Mars-Jupiter"] = 8,
	["Mars-Saturn"] = 8,
	["Jupiter-Saturn"] = 9,
	["Other"] = 1,
	["Axis"] = 4,
	["star m0"] = 1.0,
	["star m1"] = 0.5,
	["star m2"] = 0.25,
	["star m3"] = 0.12,
	["star m4"] = 0.12,
	["star m5"] = 0.12
    }

    win.aspect.clear("eqliptic")
    eql = win.aspect.new("eqliptic")

    --
    -- Lon items
    --

    -- 0

    lon = eql.lon("[font color=green]0[/font]")

    lon.view = true
    lon.angle = 0.0
    lon.orbis = orbis
    lon.line_style = "solid"
    lon.color = green

    -- 60

    lon = eql.lon("[font color=red]60[/font]")
    
    lon.view = true
    lon.angle = 60.0
    lon.orbis = orbis
    lon.line_style = "solid"
    lon.color = red

    -- 90

    lon = eql.lon("[font color=black]90[/font]")
    
    lon.view = true
    lon.angle = 90.0
    lon.orbis = orbis
    lon.line_style = "solid"
    lon.color = black

    -- 120

    lon = eql.lon("[font color=red]120[/font]")
    
    lon.view = true
    lon.angle = 120.0
    lon.orbis = orbis
    lon.line_style = "solid"
    lon.color = red

    -- 180

    lon = eql.lon("[font color=black]180[/font]")
    
    lon.view = true
    lon.angle = 180.0
    lon.orbis = orbis
    lon.line_style = "solid"
    lon.color = black

    --
    -- Orbises
    --

    eql.orbises = {
	["*"] = "Other",
	
	["Sun"] = {
	    ["Moon"] = "Sun-Moon",
	    ["Mercury"] = "Sun-Mercury",
	    ["Venus"] = "Sun-Venus",
	    ["Mars"] = "Sun-Mars",
	    ["Jupiter"] = "Sun-Jupiter",
	    ["Moon"] = "Sun-Saturn"
	},
		
	["Moon"] = {
	    ["Mercury"] = "Moon-Mercury",
	    ["Venus"] = "Moon-Venus",
	    ["Mars"] = "Moon-Mars",
	    ["Jupiter"] = "Moon-Jupiter",
	    ["Saturn"] = "Moon-Saturn"
	},
	
	["Mercury"] = {
	    ["Venus"] = "Mercury-Venus",
	    ["Mars"] = "Mercury-Mars",
	    ["Jupiter"] = "Mercury-Jupiter",
	    ["Saturn"] = "Mercury-Saturn"
	},
		
	["Venus"] = {
	    ["Mars"] = "Venus-Mars",
	    ["Jupiter"] = "Venus-Jupiter",
	    ["Saturn"] = "Venus-Saturn"
	},

	["Mars"] = {
	    ["Jupiter"] = "Mars-Jupiter",
	    ["Saturn"] = "Mars-Saturn"
	},
	
	["Jupiter"] = {
	    ["Saturn"] = "Jupiter-Saturn"
	},
		
	["cat:axis"] = "Axis",
        ["cat:star/m0"] = "star m0",
        ["cat:star/m1"] = "star m1",
        ["cat:star/m2"] = "star m2",
        ["cat:star/m3"] = "star m3",
        ["cat:star/m4"] = "star m4",
        ["cat:star/m5"] = "star m5"
    }

    --
    -- Exclude
    --

    eql.exclude = {
        ["NNode"] = {
            "SNode"
        },
    
        ["cat:star/m0"] = {
    	    "cat:star/m0",
	    "cat:star/m1",
	    "cat:star/m2",
	    "cat:star/m3",
	    "cat:star/m4",
	    "cat:star/m5"
	},

	["cat:star/m1"] = {
	    "cat:star/m1",
	    "cat:star/m2",
	    "cat:star/m3",
	    "cat:star/m4",
	    "cat:star/m5"
	},

	["cat:star/m2"] = {
	    "cat:star/m2",
	    "cat:star/m3",
	    "cat:star/m4",
	    "cat:star/m5"
	},

	["cat:star/m3"] = {
	    "cat:star/m3",
	    "cat:star/m4",
	    "cat:star/m5"
	},

	["cat:star/m4"] = {
	    "cat:star/m4",
	    "cat:star/m5"
	},

	["cat:star/m5"] = {
	    "cat:star/m5"
	},
    }
end

return {label = "Classic", func = main, key = "Ctrl+Alt+C"}
