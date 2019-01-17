#include "drawlogic.h"
#include "gamelogic.h"
#include <cstdio>
#include <vector>

#ifdef __ANDROID__
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

static char const* prerendered_strs[] = {
    "Play", // TEXT_PLAY
    "Quit", // TEXT_QUIT
    "You lost with a score of ", //TEXT_GAME_OVER
};

DrawConfig::DrawConfig(GameData *data) :
    data(data),
    bird_texture_array(4),
    bird_tick_length(100),
    last_bird_change(0),
    bird_index(0),
    animate_bird(0),
    window_width(500),
    window_height(500),
    button_rects(BUTTONS_COUNT, {0, 0, 0, 0})
{
    /*
     * Initialize SDL and related libraries.
     */
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"SDL_Init failed: %s\n",SDL_GetError());
        exit(1);
    }
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"IMG_Init for IMG_INIT_PNG failed %s\n",IMG_GetError());
        exit(1);
    }  
    if(TTF_Init() == -1){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"TTF_Init faled %s\n",TTF_GetError());
        exit(1);
    }
 
    /*
     * create and initialize SDL resources.
     */
    if(SDL_CreateWindowAndRenderer(window_width,window_height,0,&(window),&(renderer))){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Could not create window and renderer: %s\n",SDL_GetError());
    }

    /*
     * Update params with actual sizes after window creation.
     */
    SDL_GetWindowSize(window, &window_width, &window_height);

    /*
     * Set the windows title
     */
    SDL_SetWindowTitle(window,"Flappy Bird");
    
    /*
     * Load image files using SDL_image.
     */
    char const* bird_sheet_path = "resources/images/bird_sheet.png";
    bird_sprite_sheet = IMG_LoadTexture(renderer,bird_sheet_path);
    if(bird_sprite_sheet == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Image at \"%s\" failed to load: %s\n",bird_sheet_path,IMG_GetError());
    }
    for(std::vector<SDL_Rect>::size_type i = 0; i < bird_texture_array.size(); i++){
        /*32 is a magic number. Woops!*/
        bird_texture_array[i].x = 32 * i;
        bird_texture_array[i].y = 0;
        bird_texture_array[i].w = 32;
        bird_texture_array[i].h = 32;
    }

    char const* background_path = "resources/images/background.png";
    background_texture = IMG_LoadTexture(renderer,background_path);
    if(background_texture == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Image at \"%s\" failed to load: %s\n",background_path,IMG_GetError());
    }
    
    char const* pipe_path = "resources/images/pipe.png";
    pipe_texture = IMG_LoadTexture(renderer,pipe_path);
    if(pipe_texture == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Image at \"%s\" failed to load: %s\n",pipe_path,IMG_GetError());
    }
    
    char const* pipetop_path = "resources/images/pipe-top.png";
    pipe_top_texture = IMG_LoadTexture(renderer,pipetop_path);
    if(pipe_top_texture == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Image at \"%s\" failed to load: %s\n",pipetop_path,IMG_GetError());
    }

    char const* button_path = "resources/images/button.png";
    button_texture = IMG_LoadTexture(renderer,button_path);
    if(button_texture == NULL){
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
    char const* font_path = "resources/fonts/VeraMono.ttf"; 
    game_font = TTF_OpenFont(font_path,scaled_pt);
    if(game_font == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Font at \"%s\" failed to load: %s\n",font_path,TTF_GetError());
    }

    /*
     * Render some constant strings ahead of time
     */

    SDL_Color color={255,255,255};
    for(int i = 0; i < TEXT_COUNT; i++){
        texture_of_string(color, prerendered_strs[i], &(prerendered_text[i]));
    }

    SDL_Color outline_color = {0, 0, 0};
    TTF_SetFontOutline(game_font, 2);
    for(int i = 0; i < TEXT_COUNT; i++){
        texture_of_string(outline_color, prerendered_strs[i], &(prerendered_outline[i]));
    }
    TTF_SetFontOutline(game_font, 0);
}

DrawConfig::~DrawConfig(){
    SDL_DestroyTexture(bird_sprite_sheet);
    SDL_DestroyTexture(background_texture);
    SDL_DestroyTexture(pipe_texture);
    SDL_DestroyTexture(pipe_top_texture);

    for(int i = 0; i < TEXT_COUNT; i++){
        SDL_DestroyTexture(prerendered_text[i]);
        SDL_DestroyTexture(prerendered_outline[i]);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_CloseFont(game_font);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

std::vector<SDL_Rect> DrawConfig::get_button_rects(){
    return button_rects;
}

void DrawConfig::render_background(){
    /* prevent vertical stretching of background texture by limiting src height
     * to the height of the background texture */
    int bg_height;
    SDL_QueryTexture(background_texture, NULL, NULL, NULL, &bg_height);
    bg_height = bg_height > window_height ? window_height : bg_height;

    SDL_Rect source_rect = {
        0,
        0,
        window_width,
        bg_height
    };

    if(SDL_RenderCopy(renderer,background_texture,&source_rect,NULL) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering background texture: %s\n",SDL_GetError());
    }
}

void DrawConfig::render_all_pipes(){
    for(int i=0; i < data->get_num_pipes(); i++){
        Pipe pipe = data->get_pipe(i);
        render_pipe(&pipe);
    }
}

void DrawConfig::render_pipe(Pipe* pipe){

    /*
     * Query pipe textures for their height and width;
     */
    int pipe_top_texture_h, pipe_top_texture_w = scale_x_to_userspace(data->get_pipe_width());
    if(SDL_QueryTexture(pipe_top_texture, NULL, NULL, NULL, &pipe_top_texture_h) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error on query of pipe top texture: %s\n",SDL_GetError());
    }

    int pipe_texture_h, pipe_texture_w = pipe_top_texture_w;
    if(SDL_QueryTexture(pipe_texture, NULL, NULL, NULL, &pipe_texture_h) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error on query of pipe texture: %s\n",SDL_GetError());
    }

    /*
     * Render upper pipe
     */
    int i;
    for(i = 0; i < scale_y_to_userspace(pipe->top_end) - pipe_top_texture_h; i+= pipe_texture_h){
        SDL_Rect segment_rect = {
            scale_x_to_userspace(pipe->x) - (pipe_texture_w/2),
            i,
            pipe_texture_w,
            pipe_texture_h
        };
        
        if(SDL_RenderCopy(renderer,pipe_texture,NULL,&segment_rect) != 0){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering pipe texture: %s\n",SDL_GetError());
        }
    }
    SDL_Rect top_cap_rect = {
        scale_x_to_userspace(pipe->x) - (pipe_top_texture_w/2),
        scale_y_to_userspace(pipe->top_end) - pipe_top_texture_h,
        pipe_top_texture_w,
        pipe_top_texture_h
    };
    if(SDL_RenderCopyEx(renderer,pipe_top_texture,NULL,&top_cap_rect,0,NULL,SDL_FLIP_VERTICAL) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering pipe top  texture: %s\n",SDL_GetError());
    }

    /*
     * Render lower pipe
     */
    SDL_Rect low_cap_rect = {
        scale_x_to_userspace(pipe->x) - (pipe_top_texture_w/2),
        scale_y_to_userspace(pipe->bottom_start),
        pipe_top_texture_w,
        pipe_top_texture_h
    };
    if(SDL_RenderCopy(renderer,pipe_top_texture,NULL,&low_cap_rect) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering pipe top  texture: %s\n",SDL_GetError());
    }
    for(i = scale_y_to_userspace(pipe->bottom_start) + pipe_top_texture_h; i < window_height ; i+= pipe_texture_h){
        SDL_Rect segment_rect = {
            scale_x_to_userspace(pipe->x) - (pipe_texture_w/2),
            i,
            pipe_texture_w,
            pipe_texture_h
        };
        if(SDL_RenderCopy(renderer,pipe_texture,NULL,&segment_rect)){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering pipe texture: %s\n",SDL_GetError());
        }
    }
}

void DrawConfig::render_score(){
    char score_str[9];
    std::sprintf(score_str, "%d", data->get_score());
    render_string(score_str, window_width / 2, window_height / 10);
}

void DrawConfig::render_bird(){
    
    if(!animate_bird && data->get_bird_v() <= data->get_flap_v()){
        animate_bird = 1;
    }

    int scaled_bird_width = scale_x_to_userspace(data->get_bird_width());
    int scaled_bird_height = scale_y_to_userspace(data->get_bird_height());

    SDL_Rect source = bird_texture_array[bird_index];
    SDL_Rect dest = {
        scale_x_to_userspace(data->get_bird_x()) - scaled_bird_width / 2,
        scale_y_to_userspace(data->get_bird_y()) - scaled_bird_height/ 2,
        scaled_bird_width,
        scaled_bird_height
    };

    if(SDL_RenderCopyEx(renderer,bird_sprite_sheet,&source,&dest,0,NULL,SDL_FLIP_HORIZONTAL) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering bird texture %ld: %s\n",bird_index, SDL_GetError());
    }


    Uint32 currentTime = SDL_GetTicks();
    if(animate_bird && SDL_TICKS_PASSED(currentTime,last_bird_change + bird_tick_length)){
        last_bird_change = currentTime;
        bird_index = (bird_index + 1);
        if(bird_index >= bird_texture_array.size()){
            bird_index = 0;
            animate_bird = 0;
        }
    }
}

void DrawConfig::texture_of_string(SDL_Color color, char const* str, SDL_Texture** texture){
    SDL_Surface *str_surface;
    str_surface = TTF_RenderText_Blended(game_font, str, color);
    if(str_surface == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error rendering text \"%s\": %s\n", str, TTF_GetError());
    }

    *texture = SDL_CreateTextureFromSurface(renderer,str_surface);
    if(*texture == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error creating texture for text \"%s\": %s\n", str, SDL_GetError());
    }
}

void DrawConfig::render_outline(char const* str, int center_x, int center_y){
    SDL_Color outline_color = {0, 0, 0};

    TTF_SetFontOutline(game_font, 2);

    SDL_Texture *str_texture;
    texture_of_string(outline_color, str, &str_texture);

    int str_width, str_height;
    if(TTF_SizeText(game_font, str, &str_width, &str_height) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error on query of texture for text \"%s\": %s\n", str, SDL_GetError());
    }

    TTF_SetFontOutline(game_font, 0);

    SDL_Rect str_rect = {
        center_x - (str_width / 2),
        center_y - (str_height / 2) - 1,
        str_width,
        str_height
    };

    if(SDL_RenderCopy(renderer,str_texture,NULL,&str_rect) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error rendering surface for text \"%s\": %s\n", str, SDL_GetError());
    }

    SDL_DestroyTexture(str_texture);
}

void DrawConfig::render_string(char const* str, int center_x, int center_y){
    SDL_Color color={255,255,255};

    render_outline(str, center_x, center_y);

    SDL_Texture *str_texture;
    texture_of_string(color, str, &str_texture);

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

    if(SDL_RenderCopy(renderer,str_texture,NULL,&str_rect) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error rendering surface for text \"%s\": %s\n", str, SDL_GetError());
    }

    SDL_DestroyTexture(str_texture);
}

void DrawConfig::render_known_string(Text str_id, int center_x, int center_y){
    SDL_Texture *str_texture = prerendered_text[str_id];
    SDL_Texture *str_outline = prerendered_outline[str_id];

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

    if(SDL_RenderCopy(renderer,str_outline,NULL,&out_rect) != 0){
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

    if(SDL_RenderCopy(renderer,str_texture,NULL,&text_rect) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error rendering surface for text \"%s\": %s\n", prerendered_strs[str_id], SDL_GetError());
    }
}

void DrawConfig::render_gameover_message(){
    
    render_known_string(TEXT_GAME_OVER, window_width / 2, window_height / 4);

    char score_str[5];
    std::sprintf(score_str, "%d", data->get_score());

    int text_w;
    if(SDL_QueryTexture(prerendered_text[TEXT_GAME_OVER], NULL, NULL, &text_w, NULL) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error querying texture: %s\n", SDL_GetError());
    }

    render_string(score_str, window_width / 2 + text_w/2, window_height / 4);

    render_button(TEXT_PLAY, PLAY, window_width / 4, 3 * window_height / 4);
    render_button(TEXT_QUIT, QUIT, 3 * window_width / 4, 3 * window_height / 4);
}

void DrawConfig::render_button(Text str_id, Button button_id, int center_x, int center_y){

    SDL_Rect* button_rect = &(button_rects[button_id]);
    if(SDL_RectEmpty(button_rect)){
        int text_w, text_h;
        TTF_SizeText(game_font, prerendered_strs[str_id], &text_w, &text_h);

        button_rect->x = center_x - (text_w / 2) - text_h;
        button_rect->y = center_y - text_h;
        button_rect->w = text_w + 2 * text_h;
        button_rect->h = 2 * text_h;
    } 
    
    if(SDL_RenderCopyEx(renderer,button_texture,NULL,button_rect,0,NULL, SDL_FLIP_NONE) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error Rendering button texture: %s\n",SDL_GetError());
    }

    render_known_string(str_id, center_x, center_y);
}
    
void DrawConfig::render_clear(){
    if(SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error setting render color to white (FF,FF,FF,FF): %s\n",SDL_GetError());
    }
    if(SDL_RenderClear(renderer) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error clearing renderer: %s\n",SDL_GetError());
    }
}

int DrawConfig::scale_x_to_userspace(int x){
    double x_scale_factor = window_width / data->get_stage_width();
    return (x * x_scale_factor);
}

int DrawConfig::scale_y_to_userspace(int y){
    double y_scale_factor = window_height / data->get_stage_height();
    return (y * y_scale_factor);
}

void DrawConfig::render_game(){
    /*
     * Clear the window prior to any rendering
     */
    render_clear(); 

    /*
     * Draw all componenets of the came
     */
    render_background();
    render_all_pipes();
    render_score();
    render_bird();

    /*
     * Render GameOver message if the user has lost
     */
    if(data->is_gameover()){
        render_gameover_message();
    }

    /*
     * Update the users view to reflect changes 
     */
    SDL_RenderPresent(renderer);
}
