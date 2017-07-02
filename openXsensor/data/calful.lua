-- this (function) script uses 8 values stored in GVAR9 (for fase 0 to 8)
-- the first 4 values are the flow in ml/min (min = 30, max  = 800) ; the values must be in ascending order
-- the last 4 values are the correction in percent to apply (min = -100, max = 100) for each flow 
local timeprev 
local running = false
local idx = 0
local function init()
	
end

local function run()
  if not running then
      running = true
	  timeprev = getTime()
  end
  local timenow = getTime() -- 10ms tick count
  
  if timenow - timeprev > 10 then-- more than 100 msec since previous run 
	timeprev = timenow
	idx = idx + 1
	if idx > 7 then
		idx = 0
	end	
	local param = model.getGlobalVariable(8, idx )  -- get value of gvar 9 for each flight mode
	if idx < 4 then
		if param < 30 then
			param = 30
		end
		if param > 800 then
			param = 800
		end
	else
		if param < -100 then
			param = -100
		end
		if param > 100 then
			param = 100
		end
	end
    local ret =  sportTelemetryPush( 0x0D , 0X10 , idx , param ) 
  end
end

return { init=init, run=run }