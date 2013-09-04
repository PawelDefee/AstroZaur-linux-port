local function main()
    program.clear()
    
    --
    -- Charts
    --
    
    local natal1 = program.newchart("natal", "Natal 1")
    local natal2 = program.newchart("natal", "Natal 2")

    local middle = program.newchart("middle", "Middle")
    middle.parts = { natal1, natal2 }

    local date = program.newchart("natal", "Date")
    
    local progres1 = program.newchart("progres", "Progression 1")
    progres1.parent = natal1
    progres1.date = date.date
    progres1.place = natal1.place

    local progres2 = program.newchart("progres", "Progression 2")
    progres2.parent = natal2
    progres2.date = date.date
    progres2.place = natal1.place

    local middle_progres = program.newchart("progres", "Middle progres")
    middle_progres.parent = middle
    middle_progres.date = date.date
    middle_progres.place = middle.place
    
    --
    -- Windows
    --

    local w, h = gui.monitor()
    
    w = w/3 - 8
    h = w + 20
    
    -- Natal 1
    
    local w1 = program.newwindow("Natal 1")
    w1.window.position(4, 24, w, h)
    
    w1.show(natal1, true)
    w1.active = natal1
    w1.projection().start = natal1.point.Asc
    w1.aspected = { natal1 }

    -- Natal 2
    
    local w2 = program.newwindow("Natal 2")
    w2.window.position(w1.window.r()+8, w1.window.y(), w, h)
    
    w2.show(natal2, true)
    w2.active = natal2
    w2.projection().start = natal2.point.Asc
    w2.aspected = { natal2 }

    -- Middle
    
    local w3 = program.newwindow("Middle")
    w3.window.position(w2.window.r()+8, w2.window.y(), w, h)
    
    w3.show(middle, true)
    w3.active = middle
    w3.projection().start = middle.point.Asc
    w3.aspected = { middle }

    -- Natal imposition
    
    local w4 = program.newwindow("Natal imposition")
    w4.window.position(w1.window.x(), w1.window.b()+28, w, h)
    
    w4.show(natal1, true)
    w4.show(natal2, true)
    w4.active = natal1
    w4.projection().start = natal1.point.Asc
    
    aspects.Transit(w4)
    w4.aspected = { natal1, natal2 }

    -- Progres imposition
    
    local w5 = program.newwindow("Progres imposition")
    w5.window.position(w4.window.r()+8, w4.window.y(), w, h)
    
    w5.show(progres1, true)
    w5.show(progres2, true)
    w5.active = progres1
    w5.projection().start = progres1.point.Asc
    
    aspects.Transit(w5)
    w5.aspected = { progres1, progres2 }

    -- Middle progres
    
    local w6 = program.newwindow("Middle progres")
    w6.window.position(w5.window.r()+8, w5.window.y(), w, h)
    
    w6.show(middle_progres, true)
    w6.active = middle_progres
    w6.projection().start = middle_progres.point.Asc
    
    w6.aspected = { middle_progres }
    
    --

    if pref.beginer then 
	natal1.edit()
	natal2.edit()
	
	natal1.editor.window.position(w1.window.x() + 4, w1.window.y() + 24 + w1.menu.h())
	natal2.editor.window.position(w2.window.x() + 4, w2.window.y() + 24 + w2.menu.h())
    end
    
    program.calc()
    program.draw()
end

return {label = "Sinastry", callback = main}
