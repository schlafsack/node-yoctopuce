var bindings = require('bindings')('node_yoctopuce.node');
var EventEmitter = require('events').EventEmitter;

var deviceListMillis = 5000;
var eventsMillis = 500;

for (var k in EventEmitter.prototype) {
    bindings[k] = EventEmitter.prototype[k];
}

setInterval(function () {
    bindings.updateDeviceList();
}, deviceListMillis).unref();

setInterval(function ()
{
    bindings.handleEvents();
}, eventsMillis).unref();

exports = module.exports = bindings;