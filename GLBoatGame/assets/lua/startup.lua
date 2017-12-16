require 'player'

function bind(t, k)
    return function(...) return t[k](t, ...) end
end

print("ello lua")

local player = Player:new()
createBody(player.bodyDef)

local player2 = Player:new()
player2.bodyDef.position.x = 2.0
createBody(player2.bodyDef)

