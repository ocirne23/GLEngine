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
	Physics.createBody(newObj)

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
	movement[key] = (down ~= 0)
end

function Player:update(deltaSec)
	self:doMovement(deltaSec)
end

function Player:doMovement(deltaSec)
	local speed = 1.0
	local diagonalSpeed = math.sqrt(speed * 0.5)
	local forcePoint = b2Vec2:new(0.0, 0.0)
	local applyForce = b2Vec2:new(0.0, 0.0)

	if movement[EKey_W] and not movement[EKey_S] then
		if movement[EKey_A] and not movement[EKey_D] then
			applyForce.x = -diagonalSpeed
			applyForce.y = diagonalSpeed
		elseif movement[EKey_D] and not movement[EKey_A] then
			applyForce.x = diagonalSpeed
			applyForce.y = diagonalSpeed
		else
			applyForce.y = speed
		end
	elseif movement[EKey_S] and not movement[EKey_W] then
		if movement[EKey_A] and not movement[EKey_D] then
			applyForce.x = -diagonalSpeed
			applyForce.y = -diagonalSpeed
		elseif movement[EKey_D] and not movement[EKey_A] then
			applyForce.x = diagonalSpeed
			applyForce.y = -diagonalSpeed
		else
			applyForce.y = -speed
		end
	elseif movement[EKey_A] and not movement[EKey_D] then
		applyForce.x = -speed
	elseif movement[EKey_D] and not movement[EKey_A] then
		applyForce.x = speed
	end

	Physics.applyForce(self.entity, applyForce, forcePoint)
end
