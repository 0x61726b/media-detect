{
    "conditions": [
        ['OS=="win"', {
            "targets": [
                {
                    "target_name": "MediaDetect",
                    "cflags_cc": [ "-std=c++11" ],
                    "sources": [
                            "src/Common.h",
                            "src/MediaDetect.h",
                            "src/MediaDetect.cc",
                            "src/Win32MediaDetect.h",
                            "src/Win32MediaDetect.cc",
                            "src/Init.cc",
                            "src/MSAAWin32.h"
                    ],
                    "cflags": [
                        "-std=c++11"
                    ],
                    "defines!":[
                        '_HAS_EXPECTIONS=0'
                    ],
                    "variables":{
                        "CURRENT_DIR":"<!(echo %~dp0)"
                    },
                    "include_dirs": [
                        "<!(node -e \"require('nan')\")"
                    ]
                }
            ]
        }],
        ['OS=="linux"',{
            "targets": [
                {
                    "target_name": "MediaDetect",
                    "cflags_cc": [ "-std=c++11" ],
                    "link_settings": {
                        "libraries": [
                            "-lX11"
                        ]
                    },
                    "sources": [
                            "src/Common.h",
                            "src/MediaDetect.h",
                            "src/MediaDetect.cc",
                            "src/NixMediaDetect.h",
                            "src/NixMediaDetect.cc",
                            "src/Init.cc",
                    ],
                    "cflags": [
                        "-std=c++11","-Wall"
                    ],
                    "defines!":[
                        '_HAS_EXPECTIONS=0'
                    ],
                    "variables":{
                        "CURRENT_DIR":"<!(echo %~dp0)"
                    },
                    "include_dirs": [
                        "<!(node -e \"require('nan')\")"
                    ]
                }
            ]
        }]
    ]
}
