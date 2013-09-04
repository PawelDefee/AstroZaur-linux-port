local function equator(win)
    local item = win.aspect.new("equator")

    item.orbis = 1.0

    item.parallel_line{
	style = "solid",
	width = 1,
	color = "#00FF00"
    }

    item.contrparallel_line{
	style = "solid",
	width = 1,
	color = "#0000FF"
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
    if win.equator == true then
	win.aspect.clear("equator")
	win.equator = false
    else
	equator(win)
	win.equator = true
    end
end

return {label = "Equator on\\/off", func = main, key = "Ctrl+Alt+E"}
