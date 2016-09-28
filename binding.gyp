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
        }],
        ['OS=="mac"',{
            "targets": [
                {
                    "target_name": "MediaDetect",
                    "sources": [
                            "src/Common.h",
                            "src/MediaDetect.h",
                            "src/osx/MediaDetect.mm",
                            "src/OsxMediaDetect.h",
                            "src/OsxMediaDetect.mm",
                            "src/osx/Safari.h",
                            "src/osx/Chrome.h",
                            "src/osx/Init.mm",
                    ],
                    "link_settings": {
                        "libraries": [
                            'Foundation.framework',
                            'AppKit.framework',
                            'ScriptingBridge.framework'
                        ]
                    },
                    "cflags": [
                        "-Wall"
                    ],
                    "defines!":[
                        '_HAS_EXPECTIONS=0',
                        "CHIIKA_MACOSX=1"
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
