function save(f, name, table)
    local function iter(name, table, level)
	for key, val in pairs(table) do
	    if type(key) == "number" then
		key = '['..key..']'
	    elseif type(key) == "boolean" then
		key = key and "[true]" or "[false]"
	    end
	
	    if type(val) == "table" then
		f:write(string.format('%s%s = {\n', string.rep("\t", level), key))
		iter(key, val, level+1)
		f:write(string.format('%s},\n', string.rep("\t", level)))
	    else
		if type(val) == "string" then
		    val = string.format("%q", val)
		elseif type(val) == "boolean" then
		    val = val and "true" or "false"
		end

		f:write(string.format('%s%s = %s,\n', string.rep("\t", level), key, val))
	    end
	end
    end

    f:write(name.." = {\n")
    iter(name, table, 1)
    f:write("}\n\n")
end

function load_addon(window, menu, path)
    for file in lfs.dir(path) do
        local f = path..'/'..file
	local attr = lfs.attributes(f)
    
        if attr.mode == "file" then
	    local chunk, err = loadfile(f)
    
	    if chunk then
		local stat, res = pcall(chunk, window)
		
		if stat then
		    if res.label and res.callback then
			print("\t", file, res.label)

			item = menu.item(res)
			item.window = window
		    else
			gui.alert("Addon not contain label and callback")
		    end
		else
		    gui.alert("Error "..f.." "..res)
		end
	    else
		gui.alert("Error "..f.." "..err)
	    end
	end
    end
end

function day(chart)
    return astro.angle(chart.point.Sun.lon, chart.point.Asc.lon) > 0;
end

function anglstr(data)
    if data == nil then
	return "&nbsp;"
    end
    
    local g = math.floor(data)
    local m = (data - g) * 60
    local s = (m - math.floor(m)) * 60
    
    return string.format("%i %02i'%02i\"", g, m, s)
end

