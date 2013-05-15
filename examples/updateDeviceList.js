var yoctopuce = require('../');
var util = require('util');

util.print("Use Ctrl-C to quit.\n");

var updateDeviceListInterval = setInterval(function ()
{
    yoctopuce.updateDeviceList();
    util.log("device list updated.");
}, 5000);

process.on('SIGINT', function ()
{
    clearInterval(updateDeviceListInterval);
    util.log("node-yoctopuce has shutdown.");
});