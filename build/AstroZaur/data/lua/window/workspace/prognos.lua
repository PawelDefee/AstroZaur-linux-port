local function main()
    program.clear()
    
    --
    -- Charts
    --
    
    local natal = program.newchart("natal", "Natal")
    local date = program.newchart("natal", "Date")
    
    local progres = program.newchart("progres", "Progression")
    progres.parent = natal
    progres.date = date.date
    progres.place = natal.place
    
    local direct_natal = program.newchart("natal", "Direction natal")
    direct_natal.date = natal.date
    direct_natal.place = natal.place

    local zodiac_direct = program.newchart("zodiac direct", "Zodiac direction")
    zodiac_direct.parent = direct_natal
    zodiac_direct.date = date.date

    local equator_direct = program.newchart("equator direct", "Equator direction")
    equator_direct.parent = direct_natal
    equator_direct.date = date.date

    local lunar = program.newchart("revolution", "Lunar")
    lunar.date = date.date
    lunar.place = date.place
    lunar.revolution = natal.point.Moon
    lunar.backward = true

    local solar = program.newchart("revolution", "Solar")
    solar.date = date.date
    solar.place = date.place
    solar.revolution = natal.point.Sun
    solar.backward = true

    local inmundo_natal = program.newchart("natal", "Inmundo natal")
    inmundo_natal.date = natal.date
    inmundo_natal.place = natal.place
    inmundo_natal.newtransform("inmundo")

    local primary = program.newchart("progres", "Primary direction")
    primary.parent = inmundo_natal
    primary.date = date.date
    primary.place = natal.place
    primary.newtransform("inmundo")
    primary.k1 = 1
    primary.k2 = 360
    
    --
    -- Windows
    --
    
    local w, h = gui.monitor()
    
    w = w/3 - 8
    h = w + 20
    
    -- Transit
    
    local w1 = program.newwindow("Transit")
    w1.window.position(4, 24, w, h)
    
    w1.show(natal, true)
    w1.show(date, true)
    w1.active = natal
    w1.projection().start = natal.point.Asc
    
    aspects.Transit(w1)
    w1.aspected = { natal, date }
    
    -- Progression
    
    local w2 = program.newwindow("Progression")
    w2.window.position(w1.window.r()+8, w1.window.y(), w, h)
    
    w2.show(progres, true)
    w2.active = progres
    w2.projection().start = progres.point.Asc

    w2.aspected = { progres }
    
    -- Directions
    
    local w3 = program.newwindow("Directions")
    w3.window.position(w2.window.r()+8, w2.window.y(), w, h)
    
    w3.show(direct_natal, true)
    w3.show(zodiac_direct, true)
    w3.show(equator_direct, true)
    w3.active = direct_natal
    w3.projection().start = direct_natal.point.Asc
    
    aspects.Direction(w3)
    w3.aspected = { direct_natal, zodiac_direct }

    -- Lunar
    
    local w4 = program.newwindow("Lunar")
    w4.window.position(w1.window.x(), w1.window.b()+28, w, h)

    w4.show(lunar, true)
    w4.active = lunar
    w4.projection().start = lunar.point.Asc
    w4.aspected = { lunar }

    -- Solar

    local w5 = program.newwindow("Solar")
    w5.window.position(w4.window.r()+8, w4.window.y(), w, h)

    w5.show(solar, true)
    w5.active = solar
    w5.projection().start = solar.point.Asc
    w5.aspected = { solar }

    -- Primary (inmundo)
    
    local w6 = program.newwindow("Primary direction")
    w6.window.position(w5.window.r()+8, w5.window.y(), w, h)

    w6.show(inmundo_natal, true)
    w6.show(primary, true)
    
    w6.active = inmundo_natal
    w6.projection().start = inmundo_natal.point.Asc

    aspects.Transit(w6)
    w6.aspected = { inmundo_natal, primary }

    local proj = w6.projection("dial")

    proj.type = "dial"
    proj.color = "#D0EFFF"
    proj.size = 360
    proj.step = 30
    proj.fontsize = 10

    proj.width = pref.proj.width
    proj.cw = pref.proj.cw

    --

    if pref.beginer then 
	natal.edit()
	natal.editor.window.position(w1.window.x() + 4, w1.window.y() + 24 + w1.menu.h())
    end
    
    program.calc()
    program.draw()
end

return {label = "Prognos", callback = main}
