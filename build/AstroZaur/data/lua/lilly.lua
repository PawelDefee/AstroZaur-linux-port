function lilly(chart)
    local house_stat = {5, 3, 1, 4, 3, -2, 4, -2, 2, 5, 4, -5}
    local house_name = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII"}
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
