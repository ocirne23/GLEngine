require 'circlebodydef'
require 'input'

Player = {}
function Player:new()
	local newObj = {}
	newObj.entity = createEntity()
	newObj.bodyDef = CircleBodyDef:new(newObj.entity)
	
	input.addListener(EKey_W, newObj.entity, bind(newObj, "onInput"))
	input.addListener(EKey_A, newObj.entity, bind(newObj, "onInput"))
	input.addListener(EKey_S, newObj.entity, bind(newObj, "onInput"))
	input.addListener(EKey_D, newObj.entity, bind(newObj, "onInput"))
	
	self.__index = self
	return setmetatable(newObj, self)
end

function Player:onInput(key, down)
	print("player: " .. self.entity.index .. " input: " .. key .. ' ' .. down)
end

