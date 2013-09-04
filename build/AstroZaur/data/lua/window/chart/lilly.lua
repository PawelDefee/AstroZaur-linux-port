require"data/lua/lilly"

local function main(self)
    local chart = self.window.active

    if chart then
	if chart.INFO["lilly"] then
	    chart.INFO["lilly"] = nil
	else
	    chart.INFO["lilly"] = lilly
	end
	program.draw()
    else
	gui.alert("No active chart")
    end
end

return {label = "Lilly", callback = main, key = "Ctrl+L"}
