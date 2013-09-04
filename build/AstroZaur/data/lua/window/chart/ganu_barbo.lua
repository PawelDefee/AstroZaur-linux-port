local function ganu_barbo(chart)
    local point = {}
    local ganu = 0
    local barbo = 0
    local n = 0
	
    for i, n in pairs(chart.point) do
        if n.info then
    	    table.insert(point, n)
        end
    end
	
    for p1 = 1, #point, 1 do
        for p2 = p1 + 1, #point, 1 do
	    local d = astro.angle(point[p1].lon, point[p2].lon)
	    local v = point[p1].speed - point[p2].speed
	    
	    n = n + 1
	    barbo = barbo + math.abs(d)
		
	    if ((d > 0 and v > 0) or (d < 0 and v < 0)) then
	        ganu = ganu + math.abs(d)
	    else
	        ganu = ganu - math.abs(d)
	    end
	end
    end
	
    log.print(string.format("Ganu: %.2f<br>Barbo: %.2f<br>", ganu/n, barbo/n))
end

local function main(self)
    local chart = self.window.active

    if chart then
	if chart.INFO["ganu_barbo"] then
	    chart.INFO["ganu_barbo"] = nil
	else
	    chart.INFO["ganu_barbo"] = ganu_barbo
	end
	program.draw()
    else
	gui.alert("No active chart")
    end
end

return {label = "Ganu-Barbo", callback = main, key = "Ctrl+G"}
