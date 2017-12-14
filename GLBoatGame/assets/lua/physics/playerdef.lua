require 'b2dtypes'

function Player:new()
	newObj = {
		bodydef = BodyDef:new()
		fixturedefs = { 
			FixtureDef:new(CircleShape:new()) 
		}
	}
	self.__index = self
	return setmetatable(newObj, self)
end

