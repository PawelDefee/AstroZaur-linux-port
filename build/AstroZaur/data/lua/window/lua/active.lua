local function main(self)
    --[[
    if #program.chart then
	if not self.window.n_active then
	    self.window.n_active = 0
	end
	
	self.window.n_active = self.window.n_active + 1
	
	if self.window.n_active > #program.chart then
	    self.window.n_active = 1
	end
	
	self.window.active = program.chart[self.window.n_active]
	
	program.draw() 
	
    end
    ]]
    
    --[[
    self.window.aspected = { program.chart[1], program.chart[2] }
    program.calc()
    program.draw()
    ]]

    --[[    
    local win = self.window.window
    
    win.position(win.x()-10, win.y()-10)
    win.resize(win.w()+5, win.h()+5)
    ]]
    
    for i,name in ipairs{"Sun", "Moon", "Mercury"} do
	local point = program.chart[1].point[name]
    
	point.view = false
	point.aspected = false 
	point.aspecting = false 
	point.info = false
    end
    
    program.draw()
end

return {label = "Active", callback = main, key = "/"}
