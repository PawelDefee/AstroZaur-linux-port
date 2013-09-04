local function main(self)
    for i, chart in ipairs(program.chart) do
	chart.date.jd = chart.date.jd + 1/(24*60*60)
	print(chart.date.hour, chart.date.min, chart.date.sec)
    end
    
    program.calc()
    program.draw()
end

return {label = "Charts", callback = main, key = "Alt+T"}
