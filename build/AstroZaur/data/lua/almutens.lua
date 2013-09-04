function almutens(chart)
    local house_name = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII"}
    local house
    
    log.print("<table>")

    for _, house in pairs(house_name) do
	local stat = {}
	local point = chart.point[house]
	local lon = point.lon
	local name
	
	log.print("<tr><td>", house, "</td><td>")

	if chart.day then
	    name = astro.division("Ruling day", lon)
	    if name then 
		stat[name] = (stat[name] or 0) + 5
	    end

	    name = astro.division("Triplicitet day", lon)
	    if name then
		stat[name] = (stat[name] or 0)+ 3
	    end
	else
	    name = astro.division("Ruling night", lon)
	    if name then
		stat[name] = (stat[name] or 0) + 5
	    end

	    name = astro.division("Triplicitet night", lon)
	    if name then
		stat[name] = (stat[name] or 0) + 3
	    end
	end
	
	name = astro.division("Term",lon)
	if name then
	    stat[name] = (stat[name] or 0) + 2
	end

	name = astro.division("Exalted",lon)
	if name then 
	    stat[name] = (stat[name] or 0) + 4
	end

	name = astro.division("Fas",lon)
	if name then 
	    stat[name] = (stat[name] or 0) + 1
	end

	name = astro.division("Falling",lon)
	if name then
	    stat[name] = (stat[name] or 0) - 5
	end

	name = astro.division("Debilitated",lon)
	if name then
	    stat[name] = (stat[name] or 0) - 4
	end

	local almuten
	local sum = -999

	for i,p in pairs(stat) do
	    log.print(i,"(",p,") ")
	    if p > sum then
		almuten = i
		sum = p
	    end
	end
	log.print("</td><td>", almuten, "</td></tr>")
    end
    
    log.print("</table>")
end
