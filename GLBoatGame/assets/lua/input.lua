require 'inputkeys'

movement = {}
movement[EKey_W] = false
movement[EKey_A] = false
movement[EKey_S] = false
movement[EKey_D] = false
movement["update"] = function(deltaSec)

end

input = {}
input["keydown"] = function(key)
	if movement[key] ~= nil then 
		movement[key] = true
	end
end

input["keyup"] = function(key)
	if movement[key] ~= nil then 
		movement[key] = false
	end
end

input["update"] = function(deltaSec)
	if movement[EKey_W] then
		if movement[EKey_A] then
			print("wa")
		elseif movement[EKey_D] then
			print("wd")
		else
			print("w")
		end
	elseif movement[EKey_S] then
		if movement[EKey_A] then
			print("sa")
		elseif movement[EKey_D] then
			print("sd")
		else
			print("s")
		end
	elseif movement[EKey_A] then
		print("a")
	elseif movement[EKey_D] then
		print("d")
	end
end

