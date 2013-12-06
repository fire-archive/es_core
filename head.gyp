{
    "includes": [
        "common.gypi"
    ],
    "targets": [
        {
            "target_name": "head",
            "product_name": "head",
            "type": "executable",
            "sources": [
                "nn.hpp",
                "main.cpp",
                "render_main.cpp",
                "head_src/render.cpp",
                "game_main.cpp",
                "head_src/head.cpp",
            ],
            "include_dirs": [
                ".",
                "head_src",
                "Run/include/SDL2",
                "Run/include",
                
            ]
        },
    ]
}