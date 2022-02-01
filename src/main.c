#include"main.h"
#include"draw.h"
#include <math.h>

const uint32_t SCREEN_RES_W = 640;
const uint32_t SCREEN_RES_H = 480;
const uint32_t SCREEN_SIZE_W = 1280;
const uint32_t SCREEN_SIZE_H = 720;

const uint32_t LEVEL_HEIGHT = 1280;
const uint32_t LEVEL_WIDTH = 5120;
const uint32_t OCEAN_HEIGHT = 180;

const float GRAVITY = 0.1;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* application_surface;

TTF_Font* font_sml;
TTF_Font* font_med;
TTF_Font* font_lrg;

int mouse_window_x, mouse_window_y;
int mouse_screen_x, mouse_screen_y;

bool isRunning = true;
bool game_started = false;

float camera_x = LEVEL_WIDTH/2.0 - SCREEN_RES_W/2.0, camera_y = LEVEL_HEIGHT/2.0 - SCREEN_RES_H/2.0;
float player_x = LEVEL_WIDTH/2.0, player_y = LEVEL_HEIGHT/2.0;
float player_velocity_x = 0, player_velocity_y = 0, player_gravity;
SDL_Texture* player_texture;

bool left_mouse_down = false;

void loop(void* arg) {
	static uint64_t t = 0;

	// Events
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_MOUSEMOTION:
				mouse_window_x = event.motion.x;
				mouse_window_y = event.motion.y;
				mouse_screen_x = mouse_window_x*(((double)SCREEN_RES_W)/SCREEN_SIZE_W);
				mouse_screen_y = mouse_window_y*(((double)SCREEN_RES_H)/SCREEN_SIZE_H);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if(!game_started) {
					game_started = true;
				}
				if(event.button.button == 1) {
					left_mouse_down = true;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if(event.button.button == 1) {
					left_mouse_down = false;
				}
				break;
			default:
				break;
		}
	}

	if(game_started) {

		// Calculate mouse position
		int mouse_x = mouse_screen_x + camera_x;
		int mouse_y = mouse_screen_y + camera_y;

		if(left_mouse_down) {
			float vel_x = mouse_x - player_x;
			float vel_y = mouse_y - player_y;

			float len = sqrt(vel_x*vel_x + vel_y*vel_y);
			if(len > 0.0000001) {
				vel_x /= len;
				vel_y /= len;
				player_velocity_x += vel_x/3;
				player_velocity_y += vel_y/3;
			}
		} else {
			player_velocity_x *= 0.99;
			player_velocity_y *= 0.99;
			player_velocity_y += GRAVITY;
		}
		player_x += player_velocity_x;
		player_y += player_velocity_y;

		// Moving the camera twords the player
		camera_x += (player_x - (camera_x + SCREEN_RES_W/2.0))/5;
		camera_y += (player_y - (camera_y + SCREEN_RES_H/2.0))/5;
	}

	// Clamping the camera
	if(camera_x < 0) {
		camera_x = 0;
	} else if(camera_x > LEVEL_WIDTH - SCREEN_RES_W) {
		camera_x = LEVEL_WIDTH - SCREEN_RES_W;
	}
	if(camera_y < 0) {
		camera_y = 0;
	} else if(camera_y > LEVEL_HEIGHT - SCREEN_RES_H) {
		camera_y = LEVEL_HEIGHT - SCREEN_RES_H;
	}

	/*
	 *
	 * DRAWING
	 *
	 */

	// Drawing the background
	SDL_SetRenderDrawColor(renderer, 66, 148, 173, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, application_surface);
	SDL_RenderClear(renderer);

	// Drawing the player
	int draw_player_x = player_x - camera_x;
	int draw_player_y = player_y - camera_y;
	if(game_started) {
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderDrawLine(renderer, draw_player_x, draw_player_y, mouse_screen_x, mouse_screen_y);
	}
	draw_texture(player_texture, draw_player_x, draw_player_y, "cc"); 

	// Drawing the ocean
	if(camera_y >= LEVEL_HEIGHT - SCREEN_RES_H - OCEAN_HEIGHT) {
		SDL_Rect ocean_rect = {
			.x = 0,
			.y = LEVEL_HEIGHT - OCEAN_HEIGHT - camera_y,
			.w = SCREEN_RES_W,
			.h = OCEAN_HEIGHT
		};

		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		SDL_RenderFillRect(renderer, &ocean_rect);
	}
	
	// Drawing the menu
	if(!game_started) {
		draw_text("ATTACK OF THE SKYJ***S", font_lrg, SCREEN_RES_W/2, SCREEN_RES_H/4, "cc", NULL);
		static bool draw_start = false;
		if(t % 30 == 0) {
			draw_start = !draw_start;
		}
		if(draw_start) {
			draw_text("Click to Start", font_med, SCREEN_RES_W/2, SCREEN_RES_H/1.5, "cc", NULL);
		}
	}

	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderCopy(renderer, application_surface, NULL, NULL);
	SDL_RenderPresent(renderer);
	if(!isRunning) {
		emscripten_cancel_main_loop();
	}

	t++;
}

void load_assets() {
	font_sml = TTF_OpenFont("res/FreePixel.ttf", 12);
	font_med = TTF_OpenFont("res/FreePixel.ttf", 32);
	font_lrg = TTF_OpenFont("res/FreePixel.ttf", 48);
  TTF_SetFontStyle(font_lrg, TTF_STYLE_BOLD);

	player_texture = load_texture("res/Placeholder.png");
}

void unload_assets() {
	TTF_CloseFont(font_sml);
	TTF_CloseFont(font_med);
	TTF_CloseFont(font_lrg);

	SDL_DestroyTexture(player_texture);
}

int main() {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();	
	IMG_Init(IMG_INIT_PNG);

	window = SDL_CreateWindow(
			"Attack of the Skyj***s", 
			SDL_WINDOWPOS_UNDEFINED, 
			SDL_WINDOWPOS_UNDEFINED, 
			SCREEN_SIZE_W,
			SCREEN_SIZE_H, 
			SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, 0);
	application_surface = SDL_CreateTexture(
		renderer, 
		SDL_PIXELFORMAT_RGBA32, 
		SDL_TEXTUREACCESS_TARGET, 
		SCREEN_RES_W, 
		SCREEN_RES_H);

	load_assets();
	emscripten_set_main_loop_arg(loop, NULL, 60, 1);
	
	unload_assets();
	TTF_Quit();
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(application_surface);
	SDL_Quit();
	return 0;
}
