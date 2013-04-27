var bindings = require('bindings')('node_yoctopuce.node');
var EventEmitter = require('events').EventEmitter;

for (var k in EventEmitter.prototype)
{
    bindings[k] = EventEmitter.prototype[k];
}

exports = module.exports = bindings;