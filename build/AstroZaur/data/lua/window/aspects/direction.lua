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
        ["0"] = 2.0,
        ["1"] = 1.5,
        ["2"] = 1.0
    }

    win.aspect.clear("eqliptic")
    eql = win.aspect.new("eqliptic")

    eql.point1_fixed = true
    eql.point2_fixed = true

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

    -- 45

    lon = eql.lon("[font color=black]45[/font]")
    
    lon.view = false
    lon.angle = 45.0
    lon.orbis = orbis
    lon.line_style = "dash"
    lon.color = black

    -- 60

    lon = eql.lon("[font color=red]60[/font]")
    
    lon.view = true
    lon.angle = 60.0
    lon.orbis = orbis
    lon.line_style = "solid"
    lon.color = red

    -- 72

    lon = eql.lon("[font color=red]72[/font]")
    
    lon.view = true
    lon.angle = 72.0
    lon.orbis = orbis
    lon.line_style = "solid"
    lon.color = gren

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

    -- 150

    lon = eql.lon("[font color=red]150[/font]")
    
    lon.view = false
    lon.angle = 150.0
    lon.orbis = orbis
    lon.line_style = "dash"
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
        ["*"] = "2",
        ["Sun"] = "0",
        ["Moon"] = "1"
    }

    --
    -- Exclude
    --

    eql.exclude = {
    	"cat:star/m0",
	"cat:star/m1",
	"cat:star/m2",
	"cat:star/m3",
	"cat:star/m4",
	"cat:star/m5",

	["cat:house"] = {
	    "cat:house",
	    "cat:axis"
	},

	["cat:axis"] = {
	    "cat:house",
	    "cat:axis"
	}
    }
end

return {label = "Direction", func = main, key = "Ctrl+Alt+D"}
