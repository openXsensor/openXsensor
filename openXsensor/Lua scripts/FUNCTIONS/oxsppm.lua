-- this (function) script allows to send over SPORT the value of a channel
-- it is foreseen to adjust some parameters used by oXs (see ppm)
-- Here it uses Channel 9 but it can be easily edited to use another channer (or another source)

local timeprev 
local running = false
local channelValue = 0

local function init()
	
end

local function run()
  if not running then
      running = true
	  timeprev = getTime()
  end
  local timenow = getTime() -- 10ms tick count
  
  if timenow - timeprev > 20 then-- more than 200 msec since previous run 
	timeprev = timenow
	channelValue = getValue('ch9')   -- change here the channel number if you want
    local ret =  sportTelemetryPush( 0x0D , 0X10 , 0x0010 , channelValue ) 
  end
end

return { init=init, run=run }