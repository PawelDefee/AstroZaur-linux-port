local window = ...
local menu = window.menu[1]

local function clear()
    program.clear()
    program.newwindow()
end

local addon = menu.itemgroup("AddOn")
load_addon(window, addon, "data/lua/window/workspace")

menu.item(
    { label = "Clear", callback = clear },
    { label = "Quit", callback =  program.exit, key = "Alt-Q" }
)
