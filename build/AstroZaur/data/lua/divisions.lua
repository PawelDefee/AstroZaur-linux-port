function divisions(chart)
    log.print("<table><tr><td>Point</td><td>Ruling</td><td>Triplicitet</td><td>Fas</td><td>Term</td></tr>")
    
    for _,name in pairs{"Sun", "Moon", "Mercury", "Venus", "Mars", "Jupiter", "Saturn",
		   "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII"} 
    do
	local point = chart.point[name]
	local lon = point.lon

	if chart.day then
	    point.ruling = astro.division("Ruling day", lon) or nil
	    point.triplicitet = astro.division("Triplicitet day", lon) or nil
	else
	    point.ruling = astro.division("Ruling night", lon) or ""
	    point.triplicitet = astro.division("Triplicitet night", lon) or nil
	end

	point.term  = astro.division("Term", lon)
	point.fas = astro.division("Fas", lon)
	
	log.print("<tr><td>", name, "</td><td>", point.ruling, "</td><td>", point.triplicitet,
		    "</td><td>", point.fas, "</td><td>", point.term, "</td></tr>")
    end
    
    log.print("</table>")
end
