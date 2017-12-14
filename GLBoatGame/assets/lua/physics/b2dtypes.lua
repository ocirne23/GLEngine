EBodyType_Static = 0
EBodyType_Kinematic = 1
EBodyType_Dynamic = 2

EShapeType_Circle = 0
EShapeType_Edge = 1
EShapeType_Polygon = 2
EShapeType_Chain = 3

function BodyDef:new()
	newObj = {
		type = EBodyType_Dynamic,
		position = {0.0, 0.0},
		angle = 0.0,
		linearVelocity = {0.0, 0.0},
		angularVelocity = 0.0,
		linearDampening = 0.0,
		angularDampening = 0.0,
		allowSleep = true,
		awake = true,
		fixedRotation = false,
		bullet = false,
		active = true,
		userdata = {}
		gravityScale = 1.0
	}
	self.__index = self
	return setmetatable(newObj, self)
end

function FixtureDef:new(a_shape)
	newObj = {
		shape = a_shape,
		userData = {},
		friction = 0.2,
		restitution = 0.0,
		density = 0.0,
		isSensor = false
	}
	self.__index = self
	return setmetatable(newObj, self)
end

function CircleShape:new()
	newObj = {
		type = EShapeType_Circle,
		position = {0.0, 0.0}
		radius = 3.0
	}
	self.__index = self
	return setmetatable(newObj, self)
end