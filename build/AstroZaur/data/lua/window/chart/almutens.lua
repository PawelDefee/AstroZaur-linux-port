require"data/lua/almutens"

local function main(self)
    local chart = self.window.active

    if chart then
	if chart.INFO["almutens"] then
	    chart.INFO["almutens"] = nil
	else
	    chart.INFO["almutens"] = almutens
	end
	program.draw()
    else
	gui.alert("No active chart")
    end
end

return {label = "Almutens", callback = main, key = "Ctrl+A"}
