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
    
    for i,name in {"Sun", "Moon", "Mercury", "Venus", "Mars", "Jupiter", "Saturn", "NNode", "SNode"}
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
