#include <SDL2/SDL_render.h>
#include <math.h>

#include"clouds.h"
#include"main.h"
#include"draw.h"
#include"plane.h"
#include"projectiles.h"
#include"dragonfly_ai.h"

const uint32_t SCREEN_RES_W = 640;
const uint32_t SCREEN_RES_H = 360;
const uint32_t SCREEN_SIZE_W = 1280;
const uint32_t SCREEN_SIZE_H = 720;

const uint32_t LEVEL_HEIGHT = 640;
const uint32_t LEVEL_WIDTH = 1280;
const uint32_t OCEAN_HEIGHT = 80;
const uint32_t OCEAN_BACKGROUND = 40;

const SDL_Color SKY_COLOR = {107, 148, 148, 255};
const SDL_Color OCEAN_COLOR = {66, 148, 173, 255};
const SDL_Color OCEAN_COLOR_BACKGROUND = {74, 115, 148, 255};
const SDL_Color OCEAN_COLOR_SPARKLE = {156, 189, 181, 255};

const SDL_Color AMMO_BAR_COLOR = {250, 220, 55, 255};
const SDL_Color AMMO_BAR_COLOR_RIM = {74, 66, 57, 255};
const SDL_Color HEALTH_BAR_COLOR = {231, 99, 82, 255};
const SDL_Color GOOSE_HEALTH_BAR_COLOR = {206, 66, 107, 255};
const SDL_Color HEALTH_BAR_COLOR_RIM = {74, 66, 57, 255};

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* application_surface;

TTF_Font* font_sml;
TTF_Font* font_med;
TTF_Font* font_lrg;

int mouse_window_x, mouse_window_y;
int mouse_screen_x, mouse_screen_y;

bool game_started;

Cloud* clouds;
Planes* planes;
Projectiles* projectiles;

float camera_x, camera_y;

uint32_t player_id;

bool left_mouse_down;
bool space_bar_down;
bool r_pressed;

SDL_Texture** player_textures;
const uint32_t PLAYER_FRAMES = 4;
const uint32_t PLAYER_HEALTH = 20;
float player_current_frame = 0.0f;

const uint32_t PLAYER_MAX_AMMO = 50;
float player_ammo_left = PLAYER_MAX_AMMO;
int player_last_x, player_last_y;

SDL_Texture** dragonfly_textures;
const uint32_t DRAGONFLY_FRAMES = 2;

DragonflyActors* dragonfly_actors;
uint32_t dragonfly_count = 0;
const uint32_t MAX_DRAGONFLY = 8;

SDL_Texture** goose_body;
SDL_Texture** goose_head_right;
SDL_Texture** goose_head_middle;
SDL_Texture** goose_head_left;
const uint32_t GOOSE_ANIM_FRAMES = 2;
float goose_frame = 0;
const uint32_t GOOSE_MAX_HEALTH = 80;
uint32_t goose_health;
float goose_x;
const int GOOSE_HEIGHT = 60;
const Vec2 GOOSE_HURT_BOX = {140, 300};
int goose_y_offset = 0;
const uint32_t GOOSE_COOLDOWN = 120;
uint32_t goose_cooldown_left = 120;

int noise(int x, int y) {
	return (int)((((float)y)*12.9898 + ((float)x)*4.1414) * 43758.5453)%100;
}

void update_planes() {
	for(uint32_t i = planes->count - 1; i != UINT32_MAX; --i) {
		switch(planes->types[i]) {
			case PLANE_TYPE_PLAYER:
				;int mouse_x = mouse_screen_x + camera_x;
				int mouse_y = mouse_screen_y + camera_y;

				if(left_mouse_down) {
					planes->flags[i] &= ~PLANE_STATUS_NOTHRUST;
				} else {
					planes->flags[i] |= PLANE_STATUS_NOTHRUST;
				}
				planes->targets[i] = (Vec2){mouse_x, mouse_y};	

				Vec2 pos = planes->positions[i];
				camera_x += (pos.x - (camera_x + SCREEN_RES_W/2.0))/10;
				camera_y += (pos.y - (camera_y + SCREEN_RES_H/2.0))/10;

				static uint32_t player_cooldown = 0; 
				if(player_cooldown > 0) {
					player_cooldown--;
				}

				if(space_bar_down && player_cooldown == 0 && player_ammo_left >= 1) {
					proj_add(projectiles, PROJ_TYPE_PLAYER, pos, planes->dirs[i]*(M_PI/180), 12);
					player_cooldown = 10;
					player_ammo_left--;
				} else if(!space_bar_down) {
					player_ammo_left += 0.05;
					if(player_ammo_left > PLAYER_MAX_AMMO) {
						player_ammo_left = PLAYER_MAX_AMMO;
					}
				}

				player_last_x = pos.x;
				player_last_y = pos.y;
				break;
			case PLANE_TYPE_DRAGONFLY:
				planes->targets[i] = (Vec2){player_last_x, player_last_y};

				Vec2 df_pos = planes->positions[i];

				uint32_t df_actor_index = dragonfly_actors_get_index(dragonfly_actors, planes->ids[i]);	
				if(df_actor_index == UINT32_MAX) {
					continue;
				}
				uint32_t* df_fire_cooldown = dragonfly_actors->fireball_cooldowns + df_actor_index;
				if(*df_fire_cooldown > 0) {
					(*df_fire_cooldown)--;
				} else if(vec2_length((Vec2){player_last_x - df_pos.x, player_last_y - df_pos.y}) < 300) {
					proj_add(projectiles, PROJ_TYPE_DRAGFLY_FIRE, df_pos, atan2(player_last_y - df_pos.y, player_last_x - df_pos.x), 4);
					*df_fire_cooldown = DF_FIREBALL_COOLDOWN;
				}
				break;
		}

		// Check collision with the ocean
		if(planes->positions[i].y > LEVEL_HEIGHT - OCEAN_HEIGHT) {
			plane_remove(planes, i);
			continue;
		}
	}
}

void draw_planes() {
	for(uint32_t i = 0; i < planes->count; ++i) {
		switch(planes->types[i]) {
			case PLANE_TYPE_PLAYER:
				;Vec2 pos = planes->positions[i];

				SDL_RendererFlip flip = SDL_FLIP_NONE;
				if(planes->dirs[i] > 90 && planes->dirs[i] < 270) {
					flip = SDL_FLIP_VERTICAL;
				}

				uint32_t frame_index = (uint32_t)player_current_frame;
				draw_texture(player_textures[frame_index], pos.x - camera_x, pos.y - camera_y, "cc", planes->dirs[i], flip);
				player_current_frame += 0.4;
				if(player_current_frame > PLAYER_FRAMES) {
					player_current_frame -= PLAYER_FRAMES;
				}
				break;
			case PLANE_TYPE_DRAGONFLY:
				;uint32_t df_actor_index = dragonfly_actors_get_index(dragonfly_actors, planes->ids[i]);	
				if(df_actor_index == UINT32_MAX) {
					continue;
				}
				Vec2 df_pos = planes->positions[i];

				SDL_RendererFlip df_flip = SDL_FLIP_NONE;
				if(planes->dirs[i] > 90 && planes->dirs[i] < 270) {
					df_flip = SDL_FLIP_HORIZONTAL;
				}
				draw_texture(dragonfly_textures[(uint32_t)dragonfly_actors->current_frames[df_actor_index]], df_pos.x - camera_x, df_pos.y - camera_y, "cc", 0, df_flip);
				dragonfly_actors->current_frames[df_actor_index] += 0.4;
				if(dragonfly_actors->current_frames[df_actor_index] > DRAGONFLY_FRAMES) {
					dragonfly_actors->current_frames[df_actor_index] -= DRAGONFLY_FRAMES;
				}
				break;
		}
	}
}

void spawn_dragonfly(int x, int y) {
	uint32_t df_index;
	uint32_t df_id = plane_add(planes, &df_index);
	dragonfly_actors_add(dragonfly_actors, df_index);
	planes->types[df_index] = PLANE_TYPE_DRAGONFLY;
	planes->velocities[df_index].speed = 2;
	planes->velocities[df_index].max_turn = 10;
	planes->collider_sizes[df_index] = (Vec2){40, 36};
	planes->positions[df_index] = (Vec2){x, y};
	dragonfly_count++;
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
				} else if(event.key.keysym.scancode == SDL_SCANCODE_R) {
					r_pressed = true;
				}
				break;
			case SDL_KEYUP:
				if(event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
					space_bar_down = false;
				} else if(event.key.keysym.scancode == SDL_SCANCODE_R) {
					r_pressed = false;
				}
				break;
			default:
				break;
		}
	}

	if(game_started) {
		update_planes();
		planes_move(planes);
		proj_move(projectiles);
		proj_check_collision(projectiles, planes);

		if(goose_cooldown_left > 0) {
			goose_cooldown_left--;
		} else {
			int goose_y = LEVEL_HEIGHT - GOOSE_HEIGHT - 100;
			if(dragonfly_count < MAX_DRAGONFLY && rand() > RAND_MAX/2) {
				spawn_dragonfly(goose_x, goose_y);
			} else {
				uint32_t projectile_count = (((double)rand())/RAND_MAX)*16;
				const float FAN_STEP = 5.0*(M_PI/180);
				float starting_angle = atan2(player_last_y - goose_y, player_last_x - goose_x) - FAN_STEP*(projectile_count/2.0);
				for(uint32_t i = 0; i < projectile_count; ++i) {
					float angle = starting_angle + FAN_STEP*i;
					proj_add(projectiles, PROJ_TYPE_DRAGFLY_FIRE, (Vec2){goose_x, goose_y}, angle, 2);
				}
			}
			goose_cooldown_left = GOOSE_COOLDOWN;
		}
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
			
			bool lower_half = y > OCEAN_BACKGROUND/2;
			int real_x = x + camera_x/2; 
			int n = noise(real_x%2000, y);
			if((lower_half && n > 95) || (!lower_half && n > 96)) {
				SDL_RenderDrawPoint(renderer, x, y + SCREEN_RES_H - true_height);
			}
		}

	}

	proj_draw(projectiles);
	draw_planes();

	// Drawing the goose
	if(goose_health == 0) {
		goose_y_offset++;
	}
	int goose_y = LEVEL_HEIGHT - GOOSE_HEIGHT - camera_y + goose_y_offset;
	int goose_body_h;
	SDL_QueryTexture(goose_body[0], NULL, NULL, NULL, &goose_body_h);
	int goose_head_y = goose_y - goose_body_h + 10;

	int goose_head_x_offset = -30;
	int goose_anim_frame = goose_frame;
	draw_texture(goose_body[goose_anim_frame], goose_x - camera_x, goose_y, "tc", 0, SDL_FLIP_NONE);
	draw_texture(goose_head_left[goose_anim_frame], goose_x - camera_x - 30 + goose_head_x_offset, goose_head_y, "tc", 0, SDL_FLIP_NONE);
	draw_texture(goose_head_middle[!goose_anim_frame], goose_x - camera_x + goose_head_x_offset, goose_head_y, "tc", 0, SDL_FLIP_NONE);
	draw_texture(goose_head_right[goose_anim_frame], goose_x - camera_x + 40 + goose_head_x_offset, goose_head_y, "tc", 0, SDL_FLIP_NONE);
	if((goose_frame += 0.1) >= GOOSE_ANIM_FRAMES) {
		goose_frame -= GOOSE_ANIM_FRAMES;	
	}

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

	clouds_draw(clouds);
	
	// Drawing the menu
	if(!game_started) {
		draw_text("Goose Loose: Kaiju Pond", font_lrg, SCREEN_RES_W/2, SCREEN_RES_H/4, "cc", NULL);
		draw_text("Hold LEFT MOUSE to Move", font_sml, SCREEN_RES_W/2, SCREEN_RES_H/1.5 + 20, "cc", NULL);
		draw_text("Hold SPACE to Fire", font_sml, SCREEN_RES_W/2, SCREEN_RES_H/1.5 + 38, "cc", NULL);
		static bool draw_start = false;
		if(t % 30 == 0) {
			draw_start = !draw_start;
		}
		if(draw_start) {
			draw_text("Click to Start", font_med, SCREEN_RES_W/2, SCREEN_RES_H/1.5, "cc", NULL);
		}
	}

	// Draw gui elements
	uint32_t player_index = plane_get(planes, player_id);
	float health = player_index == UINT32_MAX ? 0 : planes->health[player_index];
	
	if(goose_health == 0) {
		draw_text("Goose Dead!: Kaiju Grave", font_lrg, SCREEN_RES_W/2, SCREEN_RES_H/4, "cc", NULL);
		draw_text("Refresh page to play again", font_med, SCREEN_RES_W/2, SCREEN_RES_H/1.5, "cc", NULL);
	} else if(health == 0) {
		draw_text("YOU DIED", font_lrg, SCREEN_RES_W/2, SCREEN_RES_H/4, "cc", NULL);
		draw_text("Refresh page to try again", font_med, SCREEN_RES_W/2, SCREEN_RES_H/1.5, "cc", NULL);
	}

	if(game_started) {
		int margins = 10;
		const int BAR_WIDTH = 64;
		SDL_Rect ammo_rect = {
			.x = SCREEN_RES_W - margins - BAR_WIDTH,
			.y = margins,
			.w = BAR_WIDTH*(player_ammo_left/PLAYER_MAX_AMMO),
			.h = 12
		};
		SDL_Rect ammo_rect_rim = {
			.x = SCREEN_RES_W - margins - BAR_WIDTH,
			.y = margins,
			.w = BAR_WIDTH,
			.h = 12
		};
		SDL_Rect ammo_rect_shadow = ammo_rect_rim;
		ammo_rect_shadow.y++; ammo_rect_shadow.x++;

		SDL_SetRenderDrawColor(
			renderer,
			AMMO_BAR_COLOR.r,
			AMMO_BAR_COLOR.g,
			AMMO_BAR_COLOR.b,
			AMMO_BAR_COLOR.a
		);
		SDL_RenderFillRect(renderer, &ammo_rect);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(renderer, &ammo_rect_shadow);

		SDL_SetRenderDrawColor(
			renderer,
			AMMO_BAR_COLOR_RIM.r,
			AMMO_BAR_COLOR_RIM.g,
			AMMO_BAR_COLOR_RIM.b,
			AMMO_BAR_COLOR_RIM.a
		);
		SDL_RenderDrawRect(renderer, &ammo_rect_rim);
		

		SDL_Rect health_rect = {
			.x = margins,
			.y = margins,
			.w = BAR_WIDTH*(health/PLAYER_HEALTH),
			.h = 12
		};
		SDL_Rect health_rect_rim = {
			.x = margins,
			.y = margins,
			.w = BAR_WIDTH,
			.h = 12
		};
		SDL_Rect health_rect_shadow = health_rect_rim;
		health_rect_shadow.y++; health_rect_shadow.x++;

		SDL_SetRenderDrawColor(
			renderer,
			HEALTH_BAR_COLOR.r,
			HEALTH_BAR_COLOR.g,
			HEALTH_BAR_COLOR.b,
			HEALTH_BAR_COLOR.a
		);
		SDL_RenderFillRect(renderer, &health_rect);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(renderer, &health_rect_shadow);

		SDL_SetRenderDrawColor(
			renderer,
			HEALTH_BAR_COLOR_RIM.r,
			HEALTH_BAR_COLOR_RIM.g,
			HEALTH_BAR_COLOR_RIM.b,
			HEALTH_BAR_COLOR_RIM.a
		);
		SDL_RenderDrawRect(renderer, &health_rect_rim);

		const int GOOSE_BAR_WIDTH = 200;
		SDL_Rect goose_health_rect = {
			.x = SCREEN_RES_W/2 - GOOSE_BAR_WIDTH/2,
			.y = margins,
			.w = GOOSE_BAR_WIDTH*((double)goose_health/GOOSE_MAX_HEALTH),
			.h = 12
		};
		SDL_Rect goose_health_rect_rim = {
			.x = SCREEN_RES_W/2 - GOOSE_BAR_WIDTH/2,
			.y = margins,
			.w = GOOSE_BAR_WIDTH,
			.h = 12
		};
		SDL_Rect goose_health_rect_shadow = goose_health_rect_rim;
		goose_health_rect_shadow.y++; goose_health_rect_shadow.x++;

		SDL_SetRenderDrawColor(
			renderer,
			GOOSE_HEALTH_BAR_COLOR.r,
			GOOSE_HEALTH_BAR_COLOR.g,
			GOOSE_HEALTH_BAR_COLOR.b,
			GOOSE_HEALTH_BAR_COLOR.a
		);
		SDL_RenderFillRect(renderer, &goose_health_rect);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(renderer, &goose_health_rect_shadow);

		SDL_SetRenderDrawColor(
			renderer,
			HEALTH_BAR_COLOR_RIM.r,
			HEALTH_BAR_COLOR_RIM.g,
			HEALTH_BAR_COLOR_RIM.b,
			HEALTH_BAR_COLOR_RIM.a
		);
		SDL_RenderDrawRect(renderer, &goose_health_rect_rim);
	}

	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderCopy(renderer, application_surface, NULL, NULL);
	SDL_RenderPresent(renderer);

	t++;
}

void load_assets() {
	font_sml = TTF_OpenFont("res/FreePixel.ttf", 18);
	font_med = TTF_OpenFont("res/FreePixel.ttf", 32);
	font_lrg = TTF_OpenFont("res/FreePixel.ttf", 48);
  TTF_SetFontStyle(font_lrg, TTF_STYLE_BOLD);

	player_textures = load_texture_strip("res/KaijuPlane.png", PLAYER_FRAMES);
	dragonfly_textures = load_texture_strip("res/Dragonfly.png", DRAGONFLY_FRAMES);
	goose_body = load_texture_strip("res/GooseBody.png", GOOSE_ANIM_FRAMES);
	goose_head_right = load_texture_strip("res/HeadRight.png", GOOSE_ANIM_FRAMES);
	goose_head_middle = load_texture_strip("res/HeadMiddle.png", GOOSE_ANIM_FRAMES);
	goose_head_left = load_texture_strip("res/HeadLeft.png", GOOSE_ANIM_FRAMES);

	cloud_textures_init();
	proj_init();
}

void unload_assets() {
	TTF_CloseFont(font_sml);
	TTF_CloseFont(font_med);
	TTF_CloseFont(font_lrg);

	free_texture_strip(player_textures, PLAYER_FRAMES);
	free_texture_strip(dragonfly_textures, DRAGONFLY_FRAMES);
	free_texture_strip(goose_body, GOOSE_ANIM_FRAMES);
	free_texture_strip(goose_head_right, GOOSE_ANIM_FRAMES);
	free_texture_strip(goose_head_middle, GOOSE_ANIM_FRAMES);
	free_texture_strip(goose_head_left, GOOSE_ANIM_FRAMES);
	cloud_textures_free();
	proj_free();
}

int main() {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();	
	IMG_Init(IMG_INIT_PNG);

	window = SDL_CreateWindow(
			"Goose Loose", 
			SDL_WINDOWPOS_UNDEFINED, 
			SDL_WINDOWPOS_UNDEFINED, 
			SCREEN_SIZE_W,
			SCREEN_SIZE_H, 
			SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	application_surface = SDL_CreateTexture(
		renderer, 
		SDL_PIXELFORMAT_RGBA32, 
		SDL_TEXTUREACCESS_TARGET, 
		SCREEN_RES_W, 
		SCREEN_RES_H);

	game_started = false;
	camera_x = LEVEL_WIDTH/2.0 - SCREEN_RES_W/2.0;
	camera_y = LEVEL_HEIGHT/2.0 - SCREEN_RES_H/2.0 - 50;
	left_mouse_down = false;
	space_bar_down = false;
	r_pressed = false;
	player_ammo_left = PLAYER_MAX_AMMO;
	goose_health = GOOSE_MAX_HEALTH;
	goose_x = LEVEL_WIDTH/2.0;
	
	load_assets();

	clouds = clouds_init();

	proj_init();
	projectiles = proj_create();

	planes = planes_init();
	dragonfly_actors = dragonfly_actors_create();

	uint32_t player_index;
	player_id = plane_add(planes, &player_index);
	planes->positions[player_index] = (Vec2){camera_x + SCREEN_RES_W/2.0, camera_y + SCREEN_RES_H/2.0};
	planes->types[player_index] = PLANE_TYPE_PLAYER;
	planes->health[player_index] = PLAYER_HEALTH;

	emscripten_set_main_loop_arg(loop, NULL, 60, 1);
	dragonfly_actors_destroy(dragonfly_actors);
	planes_free(planes);
	proj_destroy(projectiles);
	proj_free();

	clouds_free(clouds);

	unload_assets();
	IMG_Quit();
	TTF_Quit();
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(application_surface);
	SDL_Quit();
	return 0;
}
