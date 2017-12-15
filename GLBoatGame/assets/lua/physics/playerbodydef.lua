require 'b2dtypes'

PlayerBodyDef = {}
function PlayerBodyDef:new()
	newObj = {
		bodyDef = BodyDef:new(),
		fixtureDefs = { 
			FixtureDef:new(CircleShape:new()),
		}
	}
	self.__index = self
	return setmetatable(newObj, self)
end

