local function reception(win)
    local item = win.aspect.new("reception")

    item.division = {
	"Ruling day",
	"Ruling night",
	"Exalted",
	"Fas",
	"Term",
	"Triplicitet day",
	"Triplicitet night"
    }

    item.both_line{
	style = "solid",
	width = 2,
	color = "#FF99FF"
    }

    item.both_view = true
    
    --[[
    item.both_line.style = "solid"
    item.both_line.width = 2
    item.both_line.color = "#FF99FF"
    ]]

    item.exclude = {
        ["NNode"] = {
            "SNode"
        },
    
	"cat:axis",
	"cat:house",
        "cat:star/m0",
        "cat:star/m1",
        "cat:star/m2",
        "cat:star/m3",
        "cat:star/m4",
        "cat:star/m5"
    }
end

local function main(win)
    if win.reception == true then
	win.aspect.clear("reception")
	win.reception = false
    else
	reception(win)
	win.reception = true
    end
end

return {label = "Reception on\\/off", func = main, key = "Ctrl+Alt+R"}
