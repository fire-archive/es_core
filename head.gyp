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
                "render_main.h",
                "head_src/render.cpp",
                "head_src/render.h",
                "head_src/shared_render_state.h",
                "game_main.cpp",
                "head_src/game.cpp",
                "head_src/game.h",
                "game_main.h",
            ],
            "include_dirs": [
                ".",
                "head_src",
                "Run/include/SDL2",
				"Run/include/OGRE",
                "Run/include",
            ],
            'libraries': [
                "Run/lib/SDL2",
                "Run/lib/nanomsg",
                "Run/lib/Release/OgreMain",
            ],
        },
    ]
}
