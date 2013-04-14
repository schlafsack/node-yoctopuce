var Yoctopuce = require('../').Yoctopuce;
var util = require('util');

var yoctopuce = new Yoctopuce();

setInterval(function ()
{
    yoctopuce.updateDeviceList();
    try
    {
        yoctopuce.getDeviceInfo();
    }
    catch (e)
    {
        util.error(e);
    }
}, 500);
