local function symetric(win)
    local item = win.aspect.new("symetric")

    item.name = "ContrAntis"
    item.orbis = 1.0
    item.axis = 0

    item.line{
	style = "solid",
	width = 2,
	color = "#99FFFF"
    }

    item.exclude = {
        ["NNode"] = {
            "SNode"
        },
    
	"cat:axis",
	"cat:house",
	"cat:pars",
        "cat:star/m0",
        "cat:star/m1",
        "cat:star/m2",
        "cat:star/m3",
        "cat:star/m4",
        "cat:star/m5"
    }

    item = win.aspect.new("symetric")

    item.name = "Antis"
    item.orbis = 2.0
    item.axis = 90

    item.line{
	style = "solid",
	width = 2,
	color = "#FFFF55"
    }

    item.exclude = {
        ["NNode"] = {
            "SNode"
        },
    
	"cat:axis",
	"cat:house",
	"cat:pars",
        "cat:star/m0",
        "cat:star/m1",
        "cat:star/m2",
        "cat:star/m3",
        "cat:star/m4",
        "cat:star/m5"
    }
end

local function main(win)
    if win.symetric == true then
	win.aspect.clear("symetric")
	win.symetric = false
    else
	symetric(win)
	win.symetric = true
    end
end

return {label = "Symetric on\\/off", func = main, key = "Ctrl+Alt+S"}
