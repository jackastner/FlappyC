package com.kastner.flappyc;

import org.libsdl.app.SDLActivity;

public class FlappyC extends SDLActivity {
    @Override
    protected String[] getLibraries() {
        return new String[] {
                "SDL2",
                "SDL2_image",
                "SDL2_ttf",
                "main"
        };
    }
}
