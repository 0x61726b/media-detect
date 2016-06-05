{
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
                    "src/Init.cc"
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
}
