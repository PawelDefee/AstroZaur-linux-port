local function main(self)
    local chunk, err = loadfile"data/lua/stat.lua"

    if (chunk) then
	chunk(self)
    else
	print("Lib error "..err)
    end
end

return {label = "Statistic", callback = main, key = "Alt+S"}
