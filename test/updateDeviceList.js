var yapi = require('../');
var util = require('util');

for (var x = 1; x <= 10; x++)
{
    util.log(util.format("Update %d.", x));
    yapi.updateDeviceList();
}