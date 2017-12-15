Entity = {}
function Entity:new()
	newObj = {
		index = 0
		version = 0
	}
	self.__index = self
	return setmetatable(newObj, self)
end