require"data/lua/divisions"

local function main(self)
    local chart = self.window.active

    if chart then
	if chart.INFO["divisions"] then
	    chart.INFO["divisions"] = nil
	else
	    chart.INFO["divisions"] = divisions
	end
	program.draw()
    else
	gui.alert("No active chart")
    end
end

return {label = "Divisions", callback = main, key = "Ctrl+D"}
