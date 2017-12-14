function switch(case)
  return function(codetable)
    local f
    f = codetable[case] or codetable.default
    if f then
      if type(f)=="function" then
        return f(case)
      else
        error("case "..tostring(case).." not a function")
      end
    end
  end
end