var yoctopuce = require('../');
var util = require('util');

util.log("yoctopuce initialized:\n" + util.inspect(yoctopuce, { showHidden:true, depth:null }));

yoctopuce.on("log", function (message)
{
    util.log(util.format("Log %s.", message));
});

yoctopuce.on("deviceLog", function (device)
{
    util.log(util.format("device log %d.", device));
});

yoctopuce.on("deviceArrival", function (device)
{
    util.log(util.format("device arrived %d.", device));
});

yoctopuce.on("deviceRemoval", function (device)
{
    util.log(util.format("device removed %d.", device));
});

yoctopuce.on("deviceChange", function (device)
{
    util.log(util.format("device changed %d.", device));
});

yoctopuce.on("functionUpdate", function (func, message)
{
    util.log(util.format("function: %d value: %s.", func, message));
});

util.print("use ctrl-c to quit.\n");

setInterval(function ()
{
    yoctopuce.updateDeviceList();
    util.log("device list updated.");
}, 5000);

setInterval(function ()
{
    yoctopuce.handleEvents();
}, 500);
