-- this (function) script uses as thank capacity a value stored in GVAR9 for fase 9
-- the value in GVAR9 is the number of 50 ml in the thank. 
-- It means e.g. that for a thank of 1000 ml, GVAR9 must be set to 20 (=1000 / 50 )

local timeprev 
local running = false

local function init()
	
end

local function run()
  if not running then
      running = true
	  timeprev = getTime()
  end
  local timenow = getTime() -- 10ms tick count
  
  if timenow - timeprev > 50 then-- more than 500 msec since previous run 
	timeprev = timenow
	local tankcapacity = 50 * model.getGlobalVariable(8, 8)  -- get value of gvar 9 for flight mode 9
    if tankcapacity < 100 then
      tankcapacity = 100
    end
    if tankcapacity > 3000 then
      tankcapacity = 3000
    end
      local ret =  sportTelemetryPush( 0x0D , 0X10 , 8, tankcapacity ) 
  end
end

return { init=init, run=run }