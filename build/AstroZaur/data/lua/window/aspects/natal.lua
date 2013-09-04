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

    win.aspect.clear("eqliptic")
    eql = win.aspect.new("eqliptic")

    --
    -- Lon items
    --

    -- 0

    lon = eql.lon("[font color=green]0[/font]")

    lon.view = true
    lon.angle = 0.0
    
    lon.orbis = {
        ["0"] = 10.0,
        ["1"] = 8.0,
        ["2"] = 6.0,
        ["3"] = 5.0,
    	
        ["star m0"] = 1.0,
        ["star m1"] = 0.5,
        ["star m2"] = 0.25,
        ["star m3"] = 0.12,
        ["star m4"] = 0.12,
        ["star m5"] = 0.12
    }
    
    lon.line_style = "solid"
    lon.color = green

    -- 45

    lon = eql.lon("[font color=black]45[/font]")
    
    lon.view = false
    lon.angle = 45.0
    
    lon.orbis = {
        ["0"] = 1.0,
        ["1"] = 1.0,
        ["2"] = 1.0,
        ["3"] = 1.0,
    	
        ["star m0"] = 1.0,
        ["star m1"] = 0.5,
        ["star m2"] = 0.25,
        ["star m3"] = 0.12,
        ["star m4"] = 0.12,
        ["star m5"] = 0.12
    }
    
    lon.line_style = "dash"
    lon.color = black

    -- 60

    lon = eql.lon("[font color=red]60[/font]")
    
    lon.view = true
    lon.angle = 60.0
    
    lon.orbis = {
        ["0"] = 6.0,
        ["1"] = 5.0,
        ["2"] = 4.0,
        ["3"] = 4.0,
    	
        ["star m0"] = 1.0,
        ["star m1"] = 0.5,
        ["star m2"] = 0.25,
        ["star m3"] = 0.12,
        ["star m4"] = 0.12,
        ["star m5"] = 0.12
    }
    
    lon.line_style = "solid"
    lon.color = red

    -- 90

    lon = eql.lon("[font color=black]90[/font]")
    
    lon.view = true
    lon.angle = 90.0
    
    lon.orbis = {
        ["0"] = 8.0,
        ["1"] = 6.0,
        ["2"] = 5.0,
        ["3"] = 4.0,
	
        ["star m0"] = 1.0,
        ["star m1"] = 0.5,
        ["star m2"] = 0.25,
        ["star m3"] = 0.12,
        ["star m4"] = 0.12,
        ["star m5"] = 0.12
    }
    
    lon.line_style = "solid"
    lon.color = black

    -- 120

    lon = eql.lon("[font color=red]120[/font]")
    
    lon.view = true
    lon.angle = 120.0
    
    lon.orbis = {
        ["0"] = 8.0,
        ["1"] = 6.0,
        ["2"] = 5.0,
        ["3"] = 4.0,
	
        ["star m0"] = 1.0,
        ["star m1"] = 0.5,
        ["star m2"] = 0.25,
        ["star m3"] = 0.12,
        ["star m4"] = 0.12,
        ["star m5"] = 0.12
    }
    
    lon.line_style = "solid"
    lon.color = red

    -- 150

    lon = eql.lon("[font color=red]150[/font]")
    
    lon.view = false
    lon.angle = 150.0
        
    lon.orbis = {
        ["0"] = 2.0,
        ["1"] = 2.0,
        ["2"] = 2.0,
        ["3"] = 2.0,
	
        ["star m0"] = 1.0,
        ["star m1"] = 0.5,
        ["star m2"] = 0.25,
        ["star m3"] = 0.12,
        ["star m4"] = 0.12,
        ["star m5"] = 0.12
    }
    
    lon.line_style = "dash"
    lon.color = red

    -- 180

    lon = eql.lon("[font color=black]180[/font]")
    
    lon.view = true
    lon.angle = 180.0
    
    lon.orbis = {
        ["0"] = 10.0,
        ["1"] = 8.0,
        ["2"] = 6.0,
        ["3"] = 5.0,
	
        ["star m0"] = 1.0,
        ["star m1"] = 0.5,
        ["star m2"] = 0.25,
        ["star m3"] = 0.12,
        ["star m4"] = 0.12,
        ["star m5"] = 0.12
    }
    
    lon.line_style = "solid"
    lon.color = black

    --
    -- Orbises
    --

    eql.orbises = {
        ["*"] = "3",
        
        ["Sun"] = "0",
        ["Moon"] = "0",
	
        ["Mercury"] = "1",
        ["Venus"] = "1",
        ["Mars"] = "1",
	
        ["Jupiter"] = "2",
        ["Saturn"] = "2",
		
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

return {label = "Natal", func = main, key = "Ctrl+Alt+N"}
