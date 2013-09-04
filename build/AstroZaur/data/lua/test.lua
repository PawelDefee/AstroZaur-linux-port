house_stat = {5, 3, 1, 4, 3, -2, 4, -2, 2, 5, 4, -5}
house_name = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII"}

function lilli(chart)
    local sun = chart.point.Sun.lon
    local mars = chart.point.Mars.lon
    local saturn = chart.point.Saturn.lon
    local max_status = -999

    log.print("<table>")
    
    for i,name in pairs{"Sun", "Moon", "Mercury", "Venus", "Mars", "Jupiter", "Saturn"} do
	local point = chart.point[name]
	local lon = point.lon
	local status = 0
	
	point.peregrin = true
    
	log.print("<tr><td><img src=", point.bitmap, "></td><td>")
	
	if point.speed < point.avr_speed then
	    log.print("<font color=red>slow</font> ");
	    status = status - 2
	    point.slow = true
	else
	    log.print("<font color=green>fast</font> ");
	    status = status + 2
	    point.fast = true
	end
	
--	log.print("(", point.speed, ") ")
	
	if chart.day then
	    point.ruling = astro.division("Ruling day", lon)
	    point.triplicitet = astro.division("Triplicitet day", lon)
	else
	    point.ruling = astro.division("Ruling night", lon)
	    point.triplicitet = astro.division("Triplicitet night", lon)
	end
	
	if point.ruling == name then
	    log.print("<font color=green>ruling</font> ")
	    status = status + 5
	    point.peregrin = false
	elseif point.ruling and astro.division("Ruling night", chart.point[point.ruling].lon) == name then
	    log.print("<font color=green>ruling reception</font> ")
	    status = status + 5
	end

	if point.triplicitet == name then
	    log.print("<font color=green>trip</font> ")
	    status = status + 3
	    point.peregrin = false
	end

	point.exalt = astro.division("Exalted",lon)

	if point.exalt == name then
	    log.print("<font color=green>exalted</font> ");
	    status = status + 4
	    point.peregrin = false
	elseif exalt and astro.division("Exalted", chart.point[exalt].lon) == name then
	    log.print("<font color=green>exalted reception</font> ");
	    status = status + 4
	end

	point.term = astro.division("Term",lon)

	if point.term == name then
	    log.print("<font color=green>term</font> ");
	    status = status + 2
	    point.peregrin = false
	end

	point.fas = astro.division("Fas",lon)

	if point.fas == name then
	    log.print("<font color=green>fas</font> ");
	    status = status + 1
	    point.peregrin = false
	end

	if astro.division("Falling",lon) == name then
	    log.print("<font color=red>falling</font> ");
	    status = status - 5
	end

	if astro.division("Debilitated",lon) == name then
	    log.print("<font color=red>debilit</font> ");
	    status = status - 4
	end
	
	if point.peregrin then
	    log.print("<font color=red>peregrin</font> ");
	    status = status - 5
	end

	if name ~= "Sun" and name ~= "Moon" then
	    if point.speed < 0 then
		log.print("<font color=red>retro</font> ");
		status = status - 5
		point.retro = true
	    else
		log.print("<font color=green>direct</font> ");
		status = status + 4
		point.direct = true
	    end
	end
	
	if name ~= "Sun" then
	    local a = math.abs(astro.angle(sun, lon))
	    
	    if a < 0.2833 then
		log.print("<font color=green>kazimi</font> ");
		status = status + 5
		point.kazimi = true
	    elseif a < 8.5 then
		log.print("<font color=red>burnt</font> ");
		status = status - 5
		point.burnt = true
	    elseif a < 17 then
		log.print("<font color=red>lights</font> ");
		status = status - 4
		point.lights = true
	    end
	end
	
	--[[ REWRITE
	if math.abs(angle(mars, saturn)) < 9+7 and name ~= "Mars" and name ~= "Saturn" then
	    if between(lon, mars, saturn) then
		log.print("<font color=red>siege</font> ");
		status = status - 5
	    end	
	end
	]]

	local h = astro.between(lon,
	    chart.point.I.lon, chart.point.II.lon, chart.point.III.lon, chart.point.IV.lon,
	    chart.point.V.lon, chart.point.VI.lon, chart.point.VII.lon, chart.point.VIII.lon,
	    chart.point.IX.lon, chart.point.X.lon, chart.point.XI.lon, chart.point.XII.lon,
	    chart.point.I.lon)

	point.house = h

	status = status + house_stat[h]
	
	if house_stat[h] < 0 then
	    log.print("<font color=red>")
	else
	    log.print("<font color=green>")
	end
	
	log.print(house_name[h], "</font> ")
	
	if name == "Moon" or name == "Mercury" or name == "Venus" then
	    if astro.angle(lon, sun) < 0 then
		log.print("<font color=green>west</font> ");
		status = status + 2
		point.west = true
	    else
		log.print("<font color=red>east</font> ");
		status = status - 2
		point.east = true
	    end
	elseif name == "Mars" or name == "Saturn" or name == "Jupiter" then
	    if astro.angle(lon, sun) < 0 then
		log.print("<font color=red>west</font> ");
		status = status - 2
		point.west = true
	    else
		log.print("<font color=green>east</font> ");
		status = status + 2
		point.east = true
	    end
	end
	
	log.print("</td><td><font color=")
	
	if status < 0 then
	    log.print("red>")
	else
	    log.print("green>")
	end
	log.print(status, "</font></td></tr>")
	point.status = status
	
	if (status > max_status) or (status == max_status and point.perigrin == false) then
	    chart.lord = name
	    max_status = status
	end
    end

    log.print("</table>")
end

function almutens(chart)
    local house
    
    log.print("<table>")

    for i,house in pairs(house_name) do
	local stat = {}
	local point = chart.point[house]
	local lon = point.lon
	local name
	
	log.print("<tr><td>", house,"</td><td>")

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

function divisions(point, day)
    local lon = point.lon

    if day then
	point.ruling = astro.division("Ruling day", lon) or ""
	point.triplicitet = astro.division("Triplicitet day", lon) or ""
    else
	point.ruling = astro.division("Ruling night", lon) or ""
	point.triplicitet = astro.division("Triplicitet night", lon) or ""
    end

    point.term  = astro.division("Term", lon)
    point.fas = astro.division("Fas", lon)
end

function all_divisions(chart)
    log.print("<table><tr><td>Point</td><td>Ruling</td><td>Triplicitet</td><td>Fas</td><td>Term</td></tr>")
    
    for i,name in pairs{"Sun", "Moon", "Mercury", "Venus", "Mars", "Jupiter", "Saturn",
		   "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII"} 
    do
	local point = chart.point[name]
	
	divisions(point, chart.day)
	log.print("<tr><td>", name, "</td><td>", point.ruling, "</td><td>", point.triplicitet,
		    "</td><td>", point.fas, "</td><td>", point.term, "</td></tr>")
    end
    
    log.print("</table>")
end

temp = { 
    Fire = { heat=1, humidity=-1 };
    Earth = { heat=-1, humidity=-1 };
    Air = { heat=1, humidity=1 };
    Water = { heat=-1, humidity=1 };
    
    Sun = { 
	Aries = {heat=1+1, humidity=1-1}, 
	Leo = {heat=1+1, humidity=-1-1},
	Sagittarius = {heat=-1+1, humidity=-1-1}, 
	
	Taurus = {heat=1-1, humidity=1-1},
	Virgo = {heat=1-1, humidity=-1-1},
	Capricorn = {heat=-1-1, humidity=1-1},
	 
	Gemini = {heat=1+1, humidity=1+1}, 
	Libra = {heat=-1+1, humidity=-1+1}, 
	Aquarius = {heat=-1+1, humidity=1+1},

	Cancer = {heat=1-1, humidity=-1+1},
	Scropio = {heat=-1-1, humidity=-1+1}, 
	Pisces = {heat=-1-1, humidity=1+1}
    };
    
    Moon = { 
	[1] = {heat=1, humidity=1}, 
	[2] = {heat=1, humidity=-1},
	[3] = {heat=-1, humidity=-1}, 
	[4] = {heat=-1, humidity=1}
    };
	     
    Mercury = { 
	east = {heat=1, humidity=0},
	west = {heat=0, humidity=-1}
    };

    Venus = { 
	east = {heat=1, humidity=1}, 
	west = {heat=0, humidity=1}
    };

    Mars = { 
	east = {heat=2, humidity=-2},
	west = {heat=0, humidity=-2}
    };

    Jupiter = { 
	east = {heat=1, humidity=2},
	west = {heat=0, humidity=2}
    };

    Saturn = { 
	east = {heat=-2, humidity=-1},
	west = {heat=0, humidity=-1}
    };

    NNode = { 
	east = {heat=1, humidity=1},
	west = {heat=0, humidity=1}
    };

    SNode = { 
	east = {heat=-1, humidity=-1},
	west = {heat=0, humidity=-1}
    };
}

function temperament(chart, aspects)
    local heat = 0
    local humidity = 0
    local element
    local sun = chart.point.Sun.lon
    local color

    local function inc_param(dheat, dhumidity)
	heat = heat + dheat
	humidity = humidity + dhumidity
	log.print(dheat, "</td><td>", dhumidity, "</td></tr>")
    end

    local function calc_point(point)
	local lon = chart.point[point].lon
    
	if point == "Sun" then
	    local sign = astro.division("Sign", sun)
	    log.print("<tr bgcolor=", color, "><td>Sun in ", sign, "</td><td>")
	    inc_param(temp.Sun[sign].heat, temp.Sun[sign].humidity)
	    
	elseif point == "Moon" then
	    local phase = astro.between(lon, sun, sun+90, sun+180, sun+270, sun)
	    log.print("<tr bgcolor=", color, "><td>Moon in ", phase, " phase</td><td>")
	    inc_param(temp.Moon[phase].heat, temp.Moon[phase].humidity)

	    element = astro.division("Element", lon)
	    log.print("<tr bgcolor=", color, "><td>Moon in ", element, "</td><td>")
	    inc_param(temp[element].heat, temp[element].humidity)
	    
	elseif point == "Mercury" or point == "Venus" or point == "Mars" or 
	       point == "Jupiter" or point == "Saturn" or point == "NNode" or point == "SNode"
	then
	    element = astro.division("Element", lon)
	
	    log.print("<tr bgcolor=", color, "><td>", point, " in ", element, "</td><td>")
	    inc_param(temp[element].heat, temp[element].humidity)
	
	    if astro.angle(lon, sun) < 0 then
		log.print("<tr bgcolor=", color, "><td>", point, " west</td><td>")
		inc_param(temp[point].west.heat, temp[point].west.humidity)
	    else
		log.print("<tr bgcolor=", color, "><td>", point, " east</td><td>")
		inc_param(temp[point].east.heat, temp[point].east.humidity)
	    end
	end
    end

    local function calc_aspects(point, aspects)
	for i,k in ipairs(aspects) do
	    if k.type == "eqliptic" and k.orbis < 4 then 
		if (k.point1 == point) then
		    log.print("<tr bgcolor=", color, "><td>", 
			k.point1, " ", k.name, " ", k.point2, " (", anglstr(k.orbis), 
			")</td><td>&nbsp;</td><td>&nbsp;</td></tr>")
		    calc_point(k.point2)
		elseif (k.point2 == point) then
		    log.print("<tr bgcolor=", color, "><td>", 
			k.point2, " ", k.name, " ", k.point1, " (", anglstr(k.orbis), 
			")</td><td>&nbsp;</td><td>&nbsp;</td></tr>")
		    calc_point(k.point1)
		end
	    end
	end
    end

    log.print("<table><tr bgcolor=white><td>&nbsp;</td><td>Heat</td><td>Humidity</td></tr>")

-- ASC
    color = "gray"

    lon = chart.point.Asc.lon
    element = astro.division("Element", chart.point.Asc.lon)
    log.print("<tr bgcolor=", color, "><td>Asc in ", element, "</td><td>")
    inc_param(temp[element].heat, temp[element].humidity)
    
    calc_aspects("Asc", aspects)
    
    for i,name in pairs{"Sun", "Moon", "Mercury", "Venus", "Mars", "Jupiter", "Saturn", "NNode", "SNode"}
    do
	local point = chart.point[name]
	
	if point.house == 1 then
	    calc_point(name)
	end
    end

-- ASC dispositor
    color = "white"

    if chart.day then
	asc_d = astro.division("Ruling day", lon) or astro.division("Ruling night", lon)
    else
	asc_d = astro.division("Ruling night", lon) or astro.division("Ruling day", lon)
    end
    
    log.print("<tr bgcolor=", color, "><td>Dispositor of Asc ", asc_d, "</td><td>&nbsp;</td><td>&nbsp;</td></tr>")

    calc_point(asc_d)
    calc_aspects(asc_d, aspects)
    
-- Moon
    color = "gray"

    calc_point("Moon")
    calc_aspects("Moon", aspects)
    
-- Sun
    color = "white"

    calc_point("Sun")
    calc_aspects("Sun", aspects)

-- Lord
    color = "gray"

    calc_point(chart.lord)
    calc_aspects(chart.lord, aspects)

-- ***
    log.print("<tr bgcolor=white><td>All:</td><td>", heat, "</td><td>", humidity, "</td></tr>")
    log.print("</table>")
end

function natal(chart)
    chart.day = day(chart)
    lilli(chart)
    almutens(chart)
    all_divisions(chart)
end

function aspectcompare(a1, a2)
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

function window(window)
--    temperament(window.aspected[1], window.aspect.item)

    local aspects = window.aspect.item

    log.print("<table>")
    
    if aspects then
	table.sort(aspects, aspectcompare);
	
	for i,k in ipairs(aspects) do
	    k.name = string.gsub(k.name, "<", "&lt;")
	    k.name = string.gsub(k.name, ">", "&gt;")
	    
	    log.print("<tr><td>", k.point1, "</td><td>", k.point2, "</td><td>",
			k.name, "</td><td>", anglstr(k.orbis), "</td></tr>")
	end
    end
    log.print("</table>");
end
