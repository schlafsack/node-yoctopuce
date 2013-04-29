{
	"targets":[
		{
			"target_name":"yapi",
			"type":"static_library",
			"sources":[
				"src/yapi/yapi.h",
				"src/yapi/ydef.h",
				"src/yapi/yfifo.h",
				"src/yapi/yhash.h",
				"src/yapi/yjson.h",
				"src/yapi/ykey.h",
				"src/yapi/ymemory.h",
				"src/yapi/yprog.h",
				"src/yapi/yproto.h",
				"src/yapi/ytcp.h",
				"src/yapi/ythread.h",
				"src/yapi/yversion.h",
				"src/yapi/yapi.c",
				"src/yapi/yfifo.c",
				"src/yapi/yhash.c",
				"src/yapi/yjson.c",
				"src/yapi/ykey.c",
				"src/yapi/ymemory.c",
				"src/yapi/ypkt_lin.c",
				"src/yapi/ypkt_osx.c",
				"src/yapi/ypkt_win.c",
				"src/yapi/yprog.c",
				"src/yapi/ystream.c",
				"src/yapi/ytcp.c",
				"src/yapi/ythread.c"
			],
			"direct_dependent_settings":{
				"include_dirs":[
					"src/yapi/"
				]
			},
			"include_dirs":[
				"src/yapi/"
			],
			"conditions":[
				[
					"OS=='win'",
					{
						"defines!":[
							"_CRT_SECURE_NO_DEPRECATE"
						]
					}
				]
			]
		},
		{
			"target_name":"node_yoctopuce",
			"sources":[
				"src/node_yoctopuce.cc",
				"src/yoctopuce.h",
				"src/yoctopuce.cc",
				"src/threading.h",
				"src/async.h",
				"src/events.h"
			],
			"dependencies":[
				"yapi"
			],
			"cflags!":["-ansi", "-fno-exceptions" ],
			"cflags_cc!":[ "-fno-exceptions" ],
			"cflags":["-g", "-exceptions"],
			"cflags_cc":["-g", "-exceptions"],
			"configurations":{
				"Release":{
					"msvs_settings":{
						"VCCLCompilerTool":{
							"ExceptionHandling":1
						}
					}
				}
			}
		}
	]
}