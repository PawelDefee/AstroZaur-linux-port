local function draw_box()
    i1 = gui.image("data/pixmaps/aries.gif")
    i2 = gui.image("data/pixmaps/libra.gif")
    
    d = tab2.drawbox(5, 5, tab2.w()-10, tab2.h()-10)
    d.x = d.w()/2
    d.y = d.h()/2

    function d:draw()
	self.color("#FF0000")
	self.line(2, 2, self.w()-2, self.h()-2)
	self.line(2, self.h()-2, self.w()-2, 2)
	
	self.color("#00FF00")
	self.rect(0, 0, 50, 50)
	self.rectfill(self.x, self.y, 10, 10)
	
	self.text(20, 20, "Text")
	
	i1.draw(10, 20)
	i2.draw(30, 20)
	
	self.color("#0000FF")
	self.addvertex(2, 2)
	self.addvertex(self.x, self.y)
	self.addvertex(self.w()-2, self.h()-2)
	self.strokepath()

	self.addcurve(2,2, 10,10, self.x,self.y, self.w()-2,self.h()-2)
	self.strokepath()

	self.addarc(2,2, self.w()-2,self.h()-2, 0,270)
	self.strokepath()
    end
    
    function d:handle(event)
	if event.click then
	    self.x = event.x
	    self.y = event.y
	    
	    self.redraw()
	    
--	    print(event.x, event.y, event.click, event.button)
	elseif event.key then
	    print(event.key)
	end
    end
    
end

local function main(self)
    for i,n in pairs(self) do
	print(i, n)
    end

    w = gui.window(320, 200, "Lua poligon")

    function load_callback(self)
	print("Load", self)
    end
    
    menu = w.menubar(0, 0, w.h(), 20)
	m1 = menu.itemgroup("File")
	    i1, i2, i3 = m1.item(
		{ label="Load", key="F3", callback = load_callback }, 
		{ label="Save", key="F2",  callback = function() print("Save") end },
		{ label="Quit", key="Alt+Q" } )
		
	    print("i = ", i1, i2, i3)

	menu.item("Options/Preferences")
	menu.item("Options/About")
    
    info = w.box(0, w.h()-24, w.w(), 24)
    info.label("Status line")
    info.labelsize(18)
    
    tabs = w.tabgroup(0, menu.b(), w.w(), w.h() - info.h() - menu.h())
    
	tab1 = tabs.group(0, 20, tabs.w(), tabs.h()-20)
	tab1.label("Controls")
    
	    b1 = tab1.button(5, 5, 50, 20, "Click")
		function b1:callback()
		    gui.message("Button click")
		end

	    l1 = tab1.input(b1.r() + 50, b1.y(), 150, 20, "Input:")
		l1.value(123)

		function l1:callback()
		    info.label(string.format("input: %s", self.value()))
		    info.redraw()
		end

	    l2 = tab1.multilineinput(b1.x(), l1.b() + 5, 150, 60, "")
		l2.value("Line 1\nLine 2")
		l2.textcolor("#00FF00")
		l2.textsize(18)

		function l2:callback()
		    print("l2: ", self.value())
		end

	    b2 = tab1.checkbutton(l2.r() + 5, l2.y(), 100, 20, "Check")

		function b2:callback()
		    info.label(string.format("check: %i", self.value()))
		    info.redraw()
		end

	    g1 = tab1.group(l2.x(), l2.b() + 5, 100, 50);

		b3 = g1.radiobutton(0, 0, g1.w(), 16, "Radio 1")

		    function b3:callback()
			info.label("radio 1")
			info.redraw()
		    end

		b4 = g1.radiobutton(0, b3.h() + 5, g1.w(), 16, "Radio 2")

		    function b4:callback()
			info.label("radio 2")
			info.redraw()
		    end

	    g2 = tab1.group(g1.r() + 5, g1.y(), 100, 50);

		b5 = g2.radiobutton(0, 0, g2.w(), 16, "Radio 3")

		    function b5:callback()
			info.label("radio 3")
			info.redraw()
		    end

		b6 = g2.radiobutton(0, b5.h() + 5, g2.w(), 16, "Radio 4")

		    function b6:callback()
			info.label("radio 4")
			info.redraw()
		    end

	    c = tab1.choice(b2.x(), b2.b() + 5, 100, 20)
		c.item(
		    { label="Test 1", key="Alt+a" }, 
		    { label="Test 2", key="Alt+s" }, 
		    "Test 3")

		function c:callback()
		    info.label(string.format("choice: %i", self.value()))
		    info.redraw()
		end
		
	    c.value(2)
	    
	    v = tab1.valueinput(c.x(), c.b() + 5, 100, 20, "")
		v.range(0, 2)
		v.step(0.3)

		function v:callback()
		    info.label(string.format("valuator: %f", self.value()))
		    info.redraw()
		end
	    
	tab2 = tabs.group(0, 20, tabs.w(), tabs.h()-20)
	tab2.label("Draw")
	    draw_box()
	    
	tab3 = tabs.group(0, 20, tabs.w(), tabs.h()-20)
	tab3.label("Browser")

	    b = tab3.browser(5, 5, tab3.w()-10, tab3.h()-10, "")
		b.indented()
	    
		b.item("Item 1")
		b.item("Item 2")
		b.item("Item 3")
		b.item("Item 4/A")
		b.item("Item 4/B")
		
		bg1 = b.itemgroup("Group")
		    bg1.item("Item 5", "Item 6")
		    bg1.checkbutton(0, 0, 0, 18, "Item 7")
		    bg1.radiobutton(0, 0, 0, 18, "Item 8")
		    bg1.radiobutton(0, 0, 0, 18, "Item 9")
		    
		function b:callback()
		    info.label(string.format("browser: %i", self.value()))
		    info.redraw()
		end

    tabs.resizable(tab1)

    w.resizable(tabs)
    w.show()
end

return {label = "Poligon", callback = main, key = "Ctrl+Alt+P"}
