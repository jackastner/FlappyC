#include "drawlogic.h"
#include "gamelogic.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef __ANDROID__
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

enum Text {TEXT_PLAY, TEXT_QUIT, TEXT_GAME_OVER, TEXT_COUNT, TEXT_NOT_RENDERED};

static char* prerendered_strs[] = {
    "Play", // TEXT_PLAY
    "Quit", // TEXT_QUIT
    "You lost with a score of ", //TEXT_GAME_OVER
};

struct DrawConfig {
    SDL_Renderer* renderer;
    SDL_Window* window;

    SDL_Texture* bird_sprite_sheet;
    SDL_Rect bird_texture_array[BIRD_SPRITE_NUM];
    SDL_Texture* background_texture;
    SDL_Texture* pipe_texture;
    SDL_Texture* pipe_top_texture;
    SDL_Texture* button_texture;

    SDL_Texture* prerendered_text[TEXT_COUNT];
    SDL_Texture* prerendered_outline[TEXT_COUNT];

    TTF_Font* game_font;

    Uint32 bird_tick_length;
    Uint32 last_bird_change;
    int bird_index;
    int animate_bird;

    int window_width;
    int window_height;

    SDL_Rect** button_rects;
};

DrawConfig *create_DrawConfig(){
    DrawConfig* config = malloc(sizeof(DrawConfig));

    config->bird_tick_length = 100;
    config->last_bird_change = 0;
    config->bird_index = 0;
    config->animate_bird = 0;

    config->window_width  = 500;
    config->window_height = 500;
    

    /*
     * Initialize SDL and related libraries.
     */
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"SDL_Init failed: %s\n",SDL_GetError());
    }
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"IMG_Init for IMG_INIT_PNG failed %s\n",IMG_GetError());
    }  
    if(TTF_Init() == -1){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"TTF_Init faled %s\n",TTF_GetError());
    }

    /*
     * create and initialize SDL resources.
     */
    if(SDL_CreateWindowAndRenderer(config->window_width,config->window_height,0,&(config->window),&(config->renderer))){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Could not create window and renderer: %s\n",SDL_GetError());
    }

    /*
     * Update config params with actual sizes after window creation.
     */
    SDL_GetWindowSize(config->window, &(config->window_width), &(config->window_height));

    /*
     * Set the windows title
     */
    SDL_SetWindowTitle(config->window,"Flappy Bird");
    
    /*
     * Load image files using SDL_image.
     */
    char* bird_sheet_path = "resources/images/bird_sheet.png";
    config->bird_sprite_sheet = IMG_LoadTexture(config->renderer,bird_sheet_path);
    if(config->bird_sprite_sheet == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Image at \"%s\" failed to load: %s\n",bird_sheet_path,IMG_GetError());
    }
    for(int i = 0; i < BIRD_SPRITE_NUM; i++){
        /*32 is a magic number. Woops!*/
        config->bird_texture_array[i].x = 32 * i;
        config->bird_texture_array[i].y = 0;
        config->bird_texture_array[i].w = 32;
        config->bird_texture_array[i].h = 32;
    }

    char* background_path = "resources/images/background.png";
    config->background_texture = IMG_LoadTexture(config->renderer,background_path);
    if(config->background_texture == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Image at \"%s\" failed to load: %s\n",background_path,IMG_GetError());
    }
    
    char* pipe_path = "resources/images/pipe.png";
    config->pipe_texture = IMG_LoadTexture(config->renderer,pipe_path);
    if(config->pipe_texture == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Image at \"%s\" failed to load: %s\n",pipe_path,IMG_GetError());
    }
    
    char* pipetop_path = "resources/images/pipe-top.png";
    config->pipe_top_texture = IMG_LoadTexture(config->renderer,pipetop_path);
    if(config->pipe_top_texture == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Image at \"%s\" failed to load: %s\n",pipetop_path,IMG_GetError());
    }

    char* button_path = "resources/images/button.png";
    config->button_texture = IMG_LoadTexture(config->renderer,button_path);
    if(config->button_texture == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Image at \"%s\" failed to load: %s\n",button_path,IMG_GetError());
    }

    float defualt_dpi = 72;
    float dpi;
    if(SDL_GetDisplayDPI(0, &dpi, NULL, NULL) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Could not obtain display dpi. Using defualt value (72). %s\n",SDL_GetError());
        dpi = defualt_dpi;
    }
    int unscaled_pt = 10;
    int scaled_pt = (int) ((dpi / defualt_dpi) * unscaled_pt);

    /*
     * Load font using TTF_Font
     */
    char* font_path = "resources/fonts/VeraMono.ttf"; 
    config->game_font = TTF_OpenFont(font_path,scaled_pt);
    if(config->game_font == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Font at \"%s\" failed to load: %s\n",font_path,TTF_GetError());
    }

    /*
     * Render some constant strings ahead of time
     */

    SDL_Color color={255,255,255};
    for(int i = 0; i < TEXT_COUNT; i++){
        texture_of_string(config, color, prerendered_strs[i], &(config->prerendered_text[i]));
    }

    SDL_Color outline_color = {0, 0, 0};
    TTF_SetFontOutline(config->game_font, 2);
    for(int i = 0; i < TEXT_COUNT; i++){
        texture_of_string(config, outline_color, prerendered_strs[i], &(config->prerendered_outline[i]));
    }
    TTF_SetFontOutline(config->game_font, 0);

    config->button_rects = calloc(BUTTONS_COUNT, sizeof(SDL_Rect*));
    return config;
}

SDL_Rect** get_button_rects(DrawConfig* config){
    return config->button_rects;
}

void destroy_DrawConfig(DrawConfig* config){
    /*
     * Destroy SDL resources and quit SDL + related libraries.
     */

    SDL_DestroyTexture(config->bird_sprite_sheet);
    SDL_DestroyTexture(config->background_texture);
    SDL_DestroyTexture(config->pipe_texture);
    SDL_DestroyTexture(config->pipe_top_texture);

    for(int i = 0; i < TEXT_COUNT; i++){
        SDL_DestroyTexture(config->prerendered_text[i]);
        SDL_DestroyTexture(config->prerendered_outline[i]);
    }

    SDL_DestroyRenderer(config->renderer);
    SDL_DestroyWindow(config->window);

    for(int i = 0; i < BUTTONS_COUNT; i ++){
        if(config->button_rects[i] != NULL){
            free(config->button_rects[i]);
        }
    }
    free(config->button_rects);

    TTF_CloseFont(config->game_font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    free(config);
}

void render_background(GameData* data, DrawConfig* config){
    /* prevent vertical stretching of background texture by limiting src height
     * to the height of the background texture */
    int bg_height;
    SDL_QueryTexture(config->background_texture, NULL, NULL, NULL, &bg_height);
    bg_height = bg_height > config->window_height ? config->window_height : bg_height;

    SDL_Rect source_rect = {
        0,
        0,
        config->window_width,
        bg_height
    };

    if(SDL_RenderCopy(config->renderer,config->background_texture,&source_rect,NULL) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering background texture: %s\n",SDL_GetError());
    }
}

void render_all_pipes(GameData* data, DrawConfig* config){
    for(int i=0; i < get_num_pipes(data); i++){
        Pipe pipe = get_pipe(data,i);
        render_pipe(data,config,&pipe);
    }
}

void render_pipe(GameData* data, DrawConfig* config, Pipe* pipe){

    /*
     * Query pipe textures for their height and width;
     */
    int pipe_top_texture_h, pipe_top_texture_w = scale_x_to_userspace(data, config, get_pipe_width(data));
    if(SDL_QueryTexture(config->pipe_top_texture, NULL, NULL, NULL, &pipe_top_texture_h) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error on query of pipe top texture: %s\n",SDL_GetError());
    }

    int pipe_texture_h, pipe_texture_w = pipe_top_texture_w;
    if(SDL_QueryTexture(config->pipe_texture, NULL, NULL, NULL, &pipe_texture_h) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error on query of pipe texture: %s\n",SDL_GetError());
    }

    /*
     * Render upper pipe
     */
    int i;
    for(i = 0; i < scale_y_to_userspace(data, config, pipe->top_end) - pipe_top_texture_h; i+= pipe_texture_h){
        SDL_Rect segment_rect = {
            scale_x_to_userspace(data,config,pipe->x) - (pipe_texture_w/2),
            i,
            pipe_texture_w,
            pipe_texture_h
        };
        
        if(SDL_RenderCopy(config->renderer,config->pipe_texture,NULL,&segment_rect) != 0){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering pipe texture: %s\n",SDL_GetError());
        }
    }
    SDL_Rect top_cap_rect = {
        scale_x_to_userspace(data,config,pipe->x) - (pipe_top_texture_w/2),
        scale_y_to_userspace(data,config,pipe->top_end) - pipe_top_texture_h,
        pipe_top_texture_w,
        pipe_top_texture_h
    };
    if(SDL_RenderCopyEx(config->renderer,config->pipe_top_texture,NULL,&top_cap_rect,0,NULL,SDL_FLIP_VERTICAL) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering pipe top  texture: %s\n",SDL_GetError());
    }

    /*
     * Render lower pipe
     */
    SDL_Rect low_cap_rect = {
        scale_x_to_userspace(data,config,pipe->x) - (pipe_top_texture_w/2),
        scale_y_to_userspace(data, config, pipe->bottom_start),
        pipe_top_texture_w,
        pipe_top_texture_h
    };
    if(SDL_RenderCopy(config->renderer,config->pipe_top_texture,NULL,&low_cap_rect) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering pipe top  texture: %s\n",SDL_GetError());
    }
    for(i = scale_y_to_userspace(data, config, pipe->bottom_start) + pipe_top_texture_h; i < config->window_height ; i+= pipe_texture_h){
        SDL_Rect segment_rect = {
            scale_x_to_userspace(data,config,pipe->x) - (pipe_texture_w/2),
            i,
            pipe_texture_w,
            pipe_texture_h
        };
        if(SDL_RenderCopy(config->renderer,config->pipe_texture,NULL,&segment_rect)){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering pipe texture: %s\n",SDL_GetError());
        }
    }
}

void render_score(GameData* data, DrawConfig* config){
    char score_str[9];
    sprintf(score_str, "%d", get_score(data));
    render_string(config, score_str, config->window_width / 2, config->window_height / 10);
}

void render_bird(GameData* data, DrawConfig* config){
    
    if(!config->animate_bird && get_bird_v(data) <= get_flap_v(data)){
        config->animate_bird = 1;
    }

    int scaled_bird_width = scale_x_to_userspace(data, config, get_bird_width(data));
    int scaled_bird_height = scale_y_to_userspace(data, config, get_bird_height(data));

    SDL_Rect source = config->bird_texture_array[config->bird_index];
    SDL_Rect dest = {
        scale_x_to_userspace(data,config,get_bird_x(data)) - scaled_bird_width / 2,
        scale_y_to_userspace(data,config,get_bird_y(data)) - scaled_bird_height/ 2,
        scaled_bird_width,
        scaled_bird_height
    };

    if(SDL_RenderCopyEx(config->renderer,config->bird_sprite_sheet,&source,&dest,0,NULL,SDL_FLIP_HORIZONTAL) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering bird texture %d: %s\n",config->bird_index, SDL_GetError());
    }


    Uint32 currentTime = SDL_GetTicks();
    if(config->animate_bird && SDL_TICKS_PASSED(currentTime,config->last_bird_change + config->bird_tick_length)){
        config->last_bird_change = currentTime;
        config->bird_index = (config->bird_index + 1);
        if(config->bird_index >= BIRD_SPRITE_NUM){
            config->bird_index = 0;
            config->animate_bird = 0;
        }
    }
}

void texture_of_string(DrawConfig* config, SDL_Color color, char* str, SDL_Texture** texture){
    SDL_Surface *str_surface;
    str_surface = TTF_RenderText_Blended(config->game_font, str, color);
    if(str_surface == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error rendering text \"%s\": %s\n", str, TTF_GetError());
    }

    *texture = SDL_CreateTextureFromSurface(config->renderer,str_surface);
    if(*texture == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error creating texture for text \"%s\": %s\n", str, SDL_GetError());
    }
}

void render_outline(DrawConfig* config, char* str, int center_x, int center_y){
    SDL_Color outline_color = {0, 0, 0};

    TTF_SetFontOutline(config->game_font, 2);

    SDL_Texture *str_texture;
    texture_of_string(config, outline_color, str, &str_texture);

    int str_width, str_height;
    if(TTF_SizeText(config->game_font, str, &str_width, &str_height) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error on query of texture for text \"%s\": %s\n", str, SDL_GetError());
    }

    TTF_SetFontOutline(config->game_font, 0);

    SDL_Rect str_rect = {
        center_x - (str_width / 2),
        center_y - (str_height / 2) - 1,
        str_width,
        str_height
    };

    if(SDL_RenderCopy(config->renderer,str_texture,NULL,&str_rect) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error rendering surface for text \"%s\": %s\n", str, SDL_GetError());
    }

    SDL_DestroyTexture(str_texture);
}

void render_string(DrawConfig* config, char* str, int center_x, int center_y){
    SDL_Color color={255,255,255};

    render_outline(config, str, center_x, center_y);

    SDL_Texture *str_texture;
    texture_of_string(config, color, str, &str_texture);

    int str_width, str_height;
    if(SDL_QueryTexture(str_texture, NULL, NULL, &str_width, &str_height) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error on query of texture for text \"%s\": %s\n", str, SDL_GetError());
    }

    SDL_Rect str_rect = {
        center_x - (str_width / 2),
        center_y - (str_height / 2),
        str_width,
        str_height
    };

    if(SDL_RenderCopy(config->renderer,str_texture,NULL,&str_rect) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error rendering surface for text \"%s\": %s\n", str, SDL_GetError());
    }

    SDL_DestroyTexture(str_texture);
}

void render_known_string(DrawConfig* config, Text str_id, int center_x, int center_y){
    SDL_Texture *str_texture = config->prerendered_text[str_id];
    SDL_Texture *str_outline = config->prerendered_outline[str_id];

    /* render stored outline */
    int out_w, out_h;
    if(SDL_QueryTexture(str_outline, NULL, NULL, &out_w, &out_h) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error querying texture: %s\n", SDL_GetError());
    }

    SDL_Rect out_rect = {
        center_x - (out_w / 2),
        center_y - (out_h / 2),
        out_w,
        out_h
    };

    if(SDL_RenderCopy(config->renderer,str_outline,NULL,&out_rect) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error rendering surface for text \"%s\": %s\n", prerendered_strs[str_id], SDL_GetError());
    }

    /* render stored fill */

    int text_w, text_h;
    if(SDL_QueryTexture(str_texture, NULL, NULL, &text_w, &text_h) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error querying texture: %s\n", SDL_GetError());
    }
    
    SDL_Rect text_rect = {
        center_x - (text_w / 2),
        center_y - (text_h / 2),
        text_w,
        text_h
    };

    if(SDL_RenderCopy(config->renderer,str_texture,NULL,&text_rect) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error rendering surface for text \"%s\": %s\n", prerendered_strs[str_id], SDL_GetError());
    }
}

void render_gameover_message(GameData* data, DrawConfig* config){
    
    render_known_string(config, TEXT_GAME_OVER, config->window_width / 2, config->window_height / 4);

    char score_str[5];
    sprintf(score_str, "%d", get_score(data));

    int text_w;
    if(SDL_QueryTexture(config->prerendered_text[TEXT_GAME_OVER], NULL, NULL, &text_w, NULL) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error querying texture: %s\n", SDL_GetError());
    }

    render_string(config, score_str, config->window_width / 2 + text_w/2, config->window_height / 4);

    render_button(config, TEXT_PLAY, PLAY, config->window_width / 4, 3 * config->window_height / 4);
    render_button(config, TEXT_QUIT, QUIT, 3 * config->window_width / 4, 3 * config->window_height / 4);
}

void render_button(DrawConfig* config, Text str_id, Button button_id, int center_x, int center_y){

    SDL_Rect* button_rect;
    if(config->button_rects[button_id] == NULL){
        int text_w, text_h;
        TTF_SizeText(config->game_font, prerendered_strs[str_id], &text_w, &text_h);

        button_rect = malloc(sizeof(SDL_Rect));
        button_rect->x = center_x - (text_w / 2) - text_h;
        button_rect->y = center_y - text_h;
        button_rect->w = text_w + 2 * text_h;
        button_rect->h = 2 * text_h;

        config->button_rects[button_id] = button_rect;
    } else {
        button_rect = config->button_rects[button_id];
    }
    
    if(SDL_RenderCopyEx(config->renderer,config->button_texture,NULL,button_rect,0,NULL,0) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering button texture: %s\n",SDL_GetError());
    }

    render_known_string(config, str_id, center_x, center_y);
}
    
void render_clear(DrawConfig* config){
    if(SDL_SetRenderDrawColor(config->renderer, 0xFF, 0xFF, 0xFF, 0xFF) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error setting render color to white (FF,FF,FF,FF): %s\n",SDL_GetError());
    }
    if(SDL_RenderClear(config->renderer) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error clearing renderer: %s\n",SDL_GetError());
    }
}

int scale_x_to_userspace(GameData* data, DrawConfig* config, int x){
    double x_scale_factor = config->window_width / get_stage_width(data);
    return (x * x_scale_factor);
}

int scale_y_to_userspace(GameData* data, DrawConfig* config, int y){
    double y_scale_factor = config->window_height / get_stage_height(data);
    return (y * y_scale_factor);
}

void render_game(GameData* data, DrawConfig* config){
    /*
     * Clear the window prior to any rendering
     */
    render_clear(config); 

    /*
     * Draw all componenets of the came
     */
    render_background(data,config);
    render_all_pipes(data,config);
    render_score(data,config);
    render_bird(data,config);

    /*
     * Render GameOver message if the user has lost
     */
    if(is_gameover(data)){
        render_gameover_message(data,config);
    }

    /*
     * Update the users view to reflect changes 
     */
    SDL_RenderPresent(config->renderer);
}
