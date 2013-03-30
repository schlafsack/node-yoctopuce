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
            "defines": [
                "_LARGEFILE_SOURCE",
                "_FILE_OFFSET_BITS=64"
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
            ],
            "cflags": [
                "-g"
            ],
            "cflags!": [
                "-ansi"
            ]
        },
        {
            "target_name": "yoctopuce",
            "sources": [
                "src/yoctopuce.cc"
            ],
            "dependencies": [
                "yapi"
            ],
            "defines": [
                "_LARGEFILE_SOURCE",
                "_FILE_OFFSET_BITS=64"
            ],
            "cflags!": [
                "-ansi",
                "-fno-exceptions"
            ],
            "cflags_cc!": [
                "-fno-exceptions"
            ],
            "cflags": [
                "-g",
                "-exceptions"
            ],
            "cflags_cc": [
                "-g",
                "-exceptions"
            ],
            "configurations": {
                "Release": {
                    "msvs_settings": {
                        "VCCLCompilerTool": {
                            "ExceptionHandling": 1
                        }
                    }
                }
            }
        }
    ]
}