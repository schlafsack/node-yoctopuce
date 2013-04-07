var bindings = require('bindings')('node_yapi.node')

Object.keys(bindings).forEach(function (name)
{
    module.exports[name] = bindings[name]
})