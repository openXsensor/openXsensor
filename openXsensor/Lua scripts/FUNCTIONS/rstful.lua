-- this 'function) script is used to reset the fuel consumption on oXs (when thank is filled)
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
  if timenow - timeprev > 50 then -- more than 500 msec since previous run 
	timeprev = timenow
	local ret = sportTelemetryPush( 0x0D , 0x10 , 0x0009 , 0x12345678 ) 
  end
end

return { init=init, run=run }