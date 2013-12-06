{
    "variables": {
        "conditions": [
            [
                "OS == 'mac'",
                {
                    "target_arch%": "x64"
                },
                {
                    "target_arch%": "ia32"
                }
            ]
        ]
    },
    "target_defaults": {
        "default_configuration": "Release",
        "product_dir": "Run/bin",
        "defines": [
        ],
        'copies': [
          {
            'destination': 'Run/bin/media/',
            'files': [
              'binaries/media/',
            ],
          },
        ],
        "conditions": [
            [
                'OS == "win"',
                {
                    "libraries": [
                        "imm32.lib",
                        "winmm.lib",
                        "version.lib",
                    ]
                }
            ],
            [
                'OS == "mac"',
                {
                    "defines": [
                        "DARWIN"
                    ]
                },
                {
                    "defines": [
                        "LINUX"
                    ]
                }
            ],
            [
                "OS == 'mac' and target_arch == 'x64'",
                {
                    "xcode_settings": {
                        "ARCHS": [
                            "x86_64"
                        ]
                    }
                }
            ]
        ],
        "configurations": {
            "Debug": {
                "cflags": [
                    "-g",
                    "-O0"
                ],
                "xcode_settings": {
                    "OTHER_CFLAGS": [
                        "-g",
                        "-O0"
                    ],
                    "OTHER_CPLUSPLUSFLAGS": [
                        "-g",
                        "-O0"
                    ]
                }
            },
            "Release": {
                "cflags": [
                    "-O3"
                ],
                "defines": [
                    "NDEBUG"
                ],
                "xcode_settings": {
                    "OTHER_CFLAGS": [
                        "-O3"
                    ],
                    "OTHER_CPLUSPLUSFLAGS": [
                        "-O3",
                        "-DNDEBUG"
                    ]
                }
            }
        }
    }
}
