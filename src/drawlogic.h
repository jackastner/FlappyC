#ifndef DRAW_LOGIC_H
#define DRAW_LOGIC_H

#include "gamelogic.h"
#include <vector>

#ifdef __ANDROID__
#include <SDL.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#endif



class DrawConfig {
    public:
        enum Button {
            PLAY,
            QUIT,
            BUTTONS_COUNT
        };

    private:
		enum Text {
            TEXT_PLAY,
            TEXT_QUIT,
            TEXT_GAME_OVER,
            TEXT_COUNT,
            TEXT_NOT_RENDERED
        };

        GameData *data;

        SDL_Renderer* renderer;
        SDL_Window* window;
    
        SDL_Texture* bird_sprite_sheet;
        std::vector<SDL_Rect> bird_texture_array;
        SDL_Texture* background_texture;
        SDL_Texture* pipe_texture;
        SDL_Texture* pipe_top_texture;
        SDL_Texture* button_texture;
    
        SDL_Texture* prerendered_text[TEXT_COUNT];
        SDL_Texture* prerendered_outline[TEXT_COUNT];
    
        TTF_Font* game_font;
    
        Uint32 bird_tick_length;
        Uint32 last_bird_change;
        std::vector<SDL_Rect>::size_type bird_index;
        int animate_bird;
    
        int window_width;
        int window_height;
    
        std::vector<SDL_Rect> button_rects;

        void render_background();
        void render_all_pipes();
        void render_score();
        void render_bird();
        void render_gameover_message();
        void render_clear();
        void render_outline(char const* str, int center_x, int center_y);
        void render_string(char const* str, int center_x, int center_y);
        void render_known_string(Text str_id, int center_x, int center_y);
        void render_button(Text str_id, Button button_id, int center_x, int center_y);
        void texture_of_string(SDL_Color color, char const* str, SDL_Texture** texture);

        void render_pipe(Pipe* pipe);
        int scale_x_to_userspace(int x);
        int scale_y_to_userspace(int y);

    public:
        DrawConfig(GameData* data);
        ~DrawConfig();
        std::vector<SDL_Rect> get_button_rects();
        void render_game();
};
#endif
