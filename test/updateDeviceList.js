var Yoctopuce = require('../').Yoctopuce;
var util = require('util');

var yoctopuce = new Yoctopuce();

for (var x = 1; x <= 10; x++)
{
    util.log(util.format("Update %d.", x));
    yoctopuce.updateDeviceList();
    yoctopuce.getDeviceInfo();
}