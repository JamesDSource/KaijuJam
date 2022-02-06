#include <math.h>

#include"main.h"
#include"draw.h"
#include"plane.h"
#include"projectiles.h"

const uint32_t SCREEN_RES_W = 640;
const uint32_t SCREEN_RES_H = 480;
const uint32_t SCREEN_SIZE_W = 1280;
const uint32_t SCREEN_SIZE_H = 720;

const uint32_t LEVEL_HEIGHT = 1280;
const uint32_t LEVEL_WIDTH = 5120;
const uint32_t OCEAN_HEIGHT = 80;
const uint32_t OCEAN_BACKGROUND = 60;

const SDL_Color OCEAN_COLOR = {66, 148, 173, 255};
const SDL_Color SKY_COLOR = {49, 123, 123, 255};
const SDL_Color OCEAN_COLOR_BACKGROUND = {74, 115, 148, 255};
const SDL_Color OCEAN_COLOR_SPARKLE = {156, 189, 181, 255};

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

Planes* planes;

float camera_x = LEVEL_WIDTH/2.0 - SCREEN_RES_W/2.0, camera_y = LEVEL_HEIGHT/2.0 - SCREEN_RES_H/2.0;

uint32_t player_id;

bool left_mouse_down = false;
bool space_bar_down = false;

SDL_Texture* player_texture;

int noise(int x, int y) {
	return (int)((((float)y)*12.9898 + ((float)x)*4.1414) * 43758.5453)%100;
}

void draw_planes() {
	for(uint32_t i = 0; i < planes->count; ++i) {
		switch(planes->types[i]) {
			case PLANE_TYPE_PLAYER:
				;Vec2 pos = planes->positions[i];

				SDL_RendererFlip flip = SDL_FLIP_NONE;
				if(planes->velocities[i].dir > 90 && planes->velocities[i].dir < 270) {
					flip = SDL_FLIP_VERTICAL;
				}

				draw_texture(player_texture, pos.x - camera_x, pos.y - camera_y, "cc", planes->velocities[i].dir, flip);
				break;
		}
	}
}

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
			case SDL_KEYDOWN:
				if(event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
					space_bar_down = true;
				}
				break;
			case SDL_KEYUP:
				if(event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
					space_bar_down = false;
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

		uint32_t player_index = plane_get(planes, player_id);
		if(left_mouse_down) {
			planes->flags[player_index] &= ~PLANE_STATUS_NOTHRUST;
		} else {
			planes->flags[player_index] |= PLANE_STATUS_NOTHRUST;
		}
		planes->targets[player_index] = (Vec2){mouse_x, mouse_y};	
		planes_move(planes);

		// Moving the camera twords the player
		Vec2 pos = planes->positions[player_index];
		camera_x += (pos.x - (camera_x + SCREEN_RES_W/2.0))/10;
		camera_y += (pos.y - (camera_y + SCREEN_RES_H/2.0))/10;
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
	SDL_SetRenderDrawColor(
		renderer,
		SKY_COLOR.r,
		SKY_COLOR.g, 
		SKY_COLOR.b, 
		255);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, application_surface);
	SDL_RenderClear(renderer);

	// Drawing the ocean background
	bool can_see_ocean = camera_y >= LEVEL_HEIGHT - SCREEN_RES_H - OCEAN_HEIGHT - OCEAN_BACKGROUND; 
	if(can_see_ocean) {
		SDL_Rect ocean_background_rect = {
			.x = 0,
			.y = LEVEL_HEIGHT - OCEAN_HEIGHT - OCEAN_BACKGROUND - camera_y,
			.w = SCREEN_RES_W,
			.h = OCEAN_BACKGROUND
		};

		SDL_SetRenderDrawColor(
				renderer, 
				OCEAN_COLOR_BACKGROUND.r, 
				OCEAN_COLOR_BACKGROUND.g, 
				OCEAN_COLOR_BACKGROUND.b, 
				255);
		SDL_RenderFillRect(renderer, &ocean_background_rect);

		// Draw the sparkles
		SDL_SetRenderDrawColor(
				renderer, 
				OCEAN_COLOR.r, 
				OCEAN_COLOR.g, 
				OCEAN_COLOR.b, 
				255);
		float true_height = SCREEN_RES_H - (LEVEL_HEIGHT - OCEAN_HEIGHT - OCEAN_BACKGROUND - camera_y); 
		float height = true_height > OCEAN_BACKGROUND ? OCEAN_BACKGROUND : true_height;
		for(uint32_t i = 0; i < SCREEN_RES_W*height; ++i) {
			int x = i%SCREEN_RES_W;	
			int y = i/SCREEN_RES_W;	
			int n = noise((x + (int)camera_x)%2000, y);
			if(n > 97) {
				SDL_RenderDrawPoint(renderer, x, y + SCREEN_RES_H - true_height);
			}
		}
	}

	draw_planes();

	// Drawing the ocean
	if(can_see_ocean) {
		SDL_Rect ocean_rect = {
			.x = 0,
			.y = LEVEL_HEIGHT - OCEAN_HEIGHT - camera_y,
			.w = SCREEN_RES_W,
			.h = OCEAN_HEIGHT
		};
		SDL_SetRenderDrawColor(
				renderer, 
				OCEAN_COLOR.r, 
				OCEAN_COLOR.g, 
				OCEAN_COLOR.b, 
				255);
		SDL_RenderFillRect(renderer, &ocean_rect);

		// Draw the sparkles
		SDL_SetRenderDrawColor(
				renderer, 
				OCEAN_COLOR_SPARKLE.r, 
				OCEAN_COLOR_SPARKLE.g, 
				OCEAN_COLOR_SPARKLE.b, 
				255);
		float height = SCREEN_RES_H - (LEVEL_HEIGHT - OCEAN_HEIGHT - camera_y); 
		for(uint32_t i = 0; i < SCREEN_RES_W*height; ++i) {
			int x = i%SCREEN_RES_W;	
			int y = i/SCREEN_RES_W;	
			int n = noise((x + (int)camera_x)%2000, y);
			if(n > 98) {
				SDL_RenderDrawPoint(renderer, x, y + SCREEN_RES_H - height);
			}
		}
	}
	
	// Drawing the menu
	if(!game_started) {
		draw_text("UNTITLED PROJECT", font_lrg, SCREEN_RES_W/2, SCREEN_RES_H/4, "cc", NULL);
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
	proj_init();
}

void unload_assets() {
	TTF_CloseFont(font_sml);
	TTF_CloseFont(font_med);
	TTF_CloseFont(font_lrg);

	SDL_DestroyTexture(player_texture);
	proj_free();
}

int main() {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();	
	IMG_Init(IMG_INIT_PNG);

	window = SDL_CreateWindow(
			"UNTITLED PROJECT", 
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

	planes = planes_init();
	uint32_t player_index;
	player_id = plane_add(planes, &player_index);
	planes->positions[player_index] = (Vec2){camera_x + SCREEN_RES_W/2.0, camera_y + SCREEN_RES_H/2.0};
	planes->types[player_index] = PLANE_TYPE_PLAYER;

	emscripten_set_main_loop_arg(loop, NULL, 60, 1);
	planes_free(planes);
	
	unload_assets();
	TTF_Quit();
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(application_surface);
	SDL_Quit();
	return 0;
}
