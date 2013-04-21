var yoctopuce = require('../');
var util = require('util');

yoctopuce.events.onLog = function (message)
{
    util.log(util.format("Log %s.", message));
};

yoctopuce.events.onDeviceLog = function (message)
{
    util.log(util.format("Device Log %d.", message));
};

yoctopuce.events.onDeviceArrival = function (message)
{
    util.log(util.format("Device Arrival %d.", message));
};

yoctopuce.events.onDeviceRemoval = function (message)
{
    util.log(util.format("Device Removal %d.", message));
};

yoctopuce.events.onDeviceChange = function (message)
{
    util.log(util.format("Device Change %d.", message));
};

util.log("Yoctopuce Initialized:\n" + util.inspect(yoctopuce, { showHidden:true, depth:null }));

util.print("Use Ctrl-C to quit.\n");

setInterval(function ()
{
    yoctopuce.updateDeviceList();
    util.log("Device list updated.");
}, 5000);

setInterval(function ()
{
    yoctopuce.handleEvents();
}, 500);
