Update = {}
Update.listeners = {}
Update["addListener"] = function(entity, func)
	Update.listeners[entity] = func;
end

Update["removeListener"] = function(entity)
	Update.listeners[entity] = nil
end

Update["update"] = function(deltaSec)
	for k, v in pairs(Update.listeners) do
		v(deltaSec)
	end
end