local function window_aspects(window)
    local function aspectcompare(a1, a2)
	if a1.orbis == nil and a2.orbis == nil then 
	    return false
	end

	if a1.orbis == nil then 
	    return false
	end

	if a2.orbis == nil then 
	    return true
	end

	return a1.orbis < a2.orbis
    end

    local aspects = window.aspect.item
    
    if aspects then
	log.print("<table>")

	table.sort(aspects, aspectcompare);
	
	for i,k in ipairs(aspects) do
	    k.name = string.gsub(k.name, "<", "&lt;")
	    k.name = string.gsub(k.name, ">", "&gt;")
	    
	    log.print("<tr><td>", k.point1, "</td><td>", k.point2, "</td><td>",
			k.name, "</td><td>", anglstr(k.orbis), "</td></tr>")
	end
	
	log.print("</table>");
    end
end

-- Events

program["INFO"] = function()
    log.clear()

    for _, chart in ipairs(program.chart) do
	if next(chart.INFO) then
    	    log.print("<h3>Chart - ", chart.info, "</h3>")
	end

	chart.day = astro.angle(chart.point.Sun.lon, chart.point.Asc.lon) > 0
	
	if chart.INFO then
	    for _, func in pairs(chart.INFO) do
		func(chart)
	    end
	end
    end

    for _, window in ipairs(program.window) do
	log.print("<h3>Window - ", window.title or "(noname)", "</h3>")

	log.print("Aspected - ")
	for _, chart in pairs(window.aspected) do
	    log.print(chart.info, " ")
	end

	if window.active then
	    log.print("<br>Active - ", window.active.info, "<br>")
	end

        if window.INFO then
	    for _, func in pairs(window.INFO) do
		func(window)
	    end
        end
    end
end

program["NEW CHART"] = function(chart)
    chart.INFO = {}
    chart.width = pref.chart.width
    
    if chart.place then
	chart.place.info = pref.place.info
	chart.place.lon = pref.place.lon
	chart.place.lat = pref.place.lat
    end
    
    if chart.date then
	chart.date.zone = pref.place.zone
    end

    local points = loadfile("data/points/init.lua")
    points(chart)
    
    chart.house = pref.chart.house
end

program["NEW WINDOW"] = function(window)
    window.INFO = { window_aspects }
	    
    window.draw.border = pref.draw.border
    window.draw.fontsize = pref.draw.fontsize

    local path = "data/lua/window" 

    for file in lfs.dir(path) do
        local f = path..'/'..file
	local attr = lfs.attributes (f)
    
        if attr.mode == "file" then
	    local chunk, err = loadfile(f)
    
	    if chunk then
		print("Find plug", file)
		local stat, res = pcall(chunk, window)
		
		if not stat then
		    gui.alert("Error "..f.." "..res)
		end
	    else
		gui.alert("Error "..f.." "..err)
	    end
	end
    end
end

program["QUIT"] = function()
    local f = io.open("data/pref.lua","w")
    save(f, "pref", pref)
    f:close()

    print("== I'll be back! ==")
end
