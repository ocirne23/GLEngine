require 'playerbodydef'

print("ello lua")

player = PlayerBodyDef:new()

spawnBody(player)

for i=1, 10 do
entity = createEntity()

print(entity.index)
print(entity.version)

deleteEntity(entity)
end

