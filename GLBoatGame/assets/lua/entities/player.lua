require 'b2dtypes'
require 'input'
require 'update'

local movement = {}
movement[EKey_W] = false
movement[EKey_A] = false
movement[EKey_S] = false
movement[EKey_D] = false

Player = {}
function Player:new()
	local newObj = {}
	newObj.entity = createEntity()	
	newObj.bodyDef = BodyDef:new()
	newObj.fixtureDefs = {
		FixtureDef:new(CircleShape:new()),
	}
	
	Input.addListener(EKey_W, newObj.entity, bind(newObj, "onInput"))
	Input.addListener(EKey_A, newObj.entity, bind(newObj, "onInput"))
	Input.addListener(EKey_S, newObj.entity, bind(newObj, "onInput"))
	Input.addListener(EKey_D, newObj.entity, bind(newObj, "onInput"))
	
	Update.addListener(newObj.entity, bind(newObj, "update"))
	
	self.__index = self
	return setmetatable(newObj, self)
end

function Player:destroy()
	Input.removeListener(EKey_W, newObj.entity)
	Input.removeListener(EKey_A, newObj.entity)
	Input.removeListener(EKey_S, newObj.entity)
	Input.removeListener(EKey_D, newObj.entity)
	Update.removeListener(newObj.entity)
end

function Player:onInput(key, down)
	print("player: " .. self.entity.index .. " input: " .. key .. ' ' .. down)
	movement[key] = (down ~= 0)
	
	Physics.applyForce(self.entity, b2Vec2:new(1.0, 0.0), b2Vec2:new(0.0, 0.0))
end

function Player:update(deltaSec)
	print("player update")
end
