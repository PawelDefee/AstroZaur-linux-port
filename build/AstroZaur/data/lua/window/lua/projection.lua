local function main(self)
    local proj = self.window.projection()
    
    proj.width = proj.width + 3
    
    program.draw()
end

return {label = "Projection", callback = main, key = "Alt+P"}
