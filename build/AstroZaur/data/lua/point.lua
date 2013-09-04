function harmonic(chart, name, k)
    return chart.point[name].lon * k, 0, chart.point[name].speed * k
end

function midpoint(chart, name1, name2)
    local lon1 = chart.point[name1].lon
    local lon2 = chart.point[name2].lon
	
    return lon1 + astro.angle(lon1, lon2)/2, 0, 0
end
