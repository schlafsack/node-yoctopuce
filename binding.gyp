{
    "targets": [
        {
            "target_name": "yapi",
            "type": "static_library",
            "sources": [
                "yapi/src/yapi.h",
                "yapi/src/ydef.h",
                "yapi/src/yfifo.h",
                "yapi/src/yhash.h",
                "yapi/src/yjson.h",
                "yapi/src/ykey.h",
                "yapi/src/ymemory.h",
                "yapi/src/yprog.h",
                "yapi/src/yproto.h",
                "yapi/src/ytcp.h",
                "yapi/src/ythread.h",
                "yapi/src/yversion.h",
                "yapi/src/yapi.c",
                "yapi/src/yfifo.c",
                "yapi/src/yhash.c",
                "yapi/src/yjson.c",
                "yapi/src/ykey.c",
                "yapi/src/ymemory.c",
                "yapi/src/ypkt_lin.c",
                "yapi/src/ypkt_osx.c",
                "yapi/src/ypkt_win.c",
                "yapi/src/yprog.c",
                "yapi/src/ystream.c",
                "yapi/src/ytcp.c",
                "yapi/src/ythread.c"
            ],
            "direct_dependent_settings": {
                "include_dirs": [
                    "yapi/src"
                ]
            },
            "include_dirs": [
                "yapi/src"
            ],
            "conditions": [
                [
                    "OS=='win'",
                    {
                        "defines!": [
                            "_CRT_SECURE_NO_DEPRECATE"
                        ]
                    }
                ]
            ]
        },
        {
            "target_name": "node_yoctopuce",
            "sources": [
                "src/node_yoctopuce.cc",
				"src/yoctopuce.h",
				"src/yoctopuce.cc",
				"threading.h",
				"async.h"
            ],
            "dependencies": [
                "yapi"
            ]
        }
    ]
}