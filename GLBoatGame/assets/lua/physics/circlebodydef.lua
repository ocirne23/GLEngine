require 'b2dtypes'

CircleBodyDef = {}
function CircleBodyDef:new(a_entity)
	local newObj = {
		entity = a_entity,
		bodyDef = BodyDef:new(),
		fixtureDefs = {
			FixtureDef:new(CircleShape:new()),
		},
	}
	self.__index = self
	return setmetatable(newObj, self)
end

