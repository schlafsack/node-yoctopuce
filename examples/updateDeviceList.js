var yoctopuce = require('../');
var util = require('util');

util.print("Use Ctrl-C to quit.\n");
setInterval(function ()
{
    yoctopuce.updateDeviceList();
    util.log("Device list updated.");
}, 2000);