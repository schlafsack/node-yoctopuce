var bindings = require('bindings')('yoctopuce.node');
var EventEmitter = require('events').EventEmitter;
var util = require('util');

bindings.Yoctopuce.prototype.__proto__ = EventEmitter.prototype;

bindings.events.onLog = function (message)
{
    util.log(util.format("Log %s.", message));
}

/*bindings.events.onDeviceLog = function(message) {
 util.log(util.format("Device Log %d.", message));
 }*/

bindings.events.onDeviceArrival = function (message)
{
    util.log(util.format("Device Arrival %d.", message));
}

bindings.events.onDeviceRemoval = function (message)
{
    util.log(util.format("Device Removal %d.", message));
}

/*bindings.events.onDeviceChange = function(message) {
 util.log(util.format("Device Change %d.", message));
 }*/

exports = module.exports = bindings;