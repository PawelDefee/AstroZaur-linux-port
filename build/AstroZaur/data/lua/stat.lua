local self = ...

local chart = self.window.active
local count = 10000

if chart then
    local jd = chart.date.jd

    chart.stat = 0
    
    print("== start")
    local start = os.time()
    
    for c = 1, count do
	chart.date.jd = math.random(jd - 365*50, jd + 365*50) + math.random()
	program.calc()

	local aspects = self.window.aspect.item
    
	for _, n in ipairs(aspects) do
--	    print(n.point1, n.point2, n.type, n.name)
	
--	    if n.point1 == "Sun" and n.point2 == "Moon" and n.type == "eqliptic" then
--		chart.stat = chart.stat + 1
--	    end
	end
	
	if gui.ready() then
--	    program.draw()
	    gui.check()
	end
    end
    
    chart.date.jd = jd
    program.calc()
    program.draw()
    
    print("== stop", chart.stat/count, os.time() - start)
end
