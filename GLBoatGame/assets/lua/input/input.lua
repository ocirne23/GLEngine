require 'inputkeys'

local movement = {}
movement[EKey_W] = false
movement[EKey_A] = false
movement[EKey_S] = false
movement[EKey_D] = false

Input = {}
Input.listeners = {}

Input["addListener"] = function(key, entity, func)
	if Input.listeners[key] == nil then
		Input.listeners[key] = {}
	end
	Input.listeners[key][entity] = func;
end

Input["removeListener"] = function(key, entity)
	if Input.listeners[key] then
		Input.listeners[key][entity] = nil
	end
end

Input["keyDown"] = function(key)
	local listeners = Input.listeners[key]
	if listeners then
		for k, v in pairs(listeners) do
			v(key, 1)
		end
	end
	
	if movement[key] ~= nil then 
		movement[key] = true
	end
end

Input["keyUp"] = function(key)
	local listeners = Input.listeners[key]
	if listeners then
		for k, v in pairs(listeners) do
			v(key, 0)
		end
	end

	if movement[key] ~= nil then 
		movement[key] = false
	end
end

Input["update"] = function(deltaSec)
	if movement[EKey_W] then
		if movement[EKey_A] then
			--print("wa")
		elseif movement[EKey_D] then
			--print("wd")
		else
			--print("w")
		end
	elseif movement[EKey_S] then
		if movement[EKey_A] then
			--print("sa")
		elseif movement[EKey_D] then
			--print("sd")
		else
			--print("s")
		end
	elseif movement[EKey_A] then
		--print("a")
	elseif movement[EKey_D] then
		--print("d")
	end
end

