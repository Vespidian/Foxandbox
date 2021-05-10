#include "global.h"
#include "event.h"
#include "textures.h"
#include "entities/item.h"
#include "world/block.h"
#include "world/sandbox.h"
#include "renderer/tilesheet.h"
#include "renderer/renderer.h"
#include "renderer/render_text.h"
#include "ui/ui.h"
#include "ui/start_screen.h"


#include "animation/animation.h"
#include "ui/load_screen.h"

#include "world/sandbox_generation.h"
#include "world/generation/perlin.h"
#include "utility.h"

int noise_array1D[32];
int output_array1D[32];
int noise_array2D[256][256];
int output_array2D[256][256];
int array_size = 256;
int rand_range = 50;

int global_bias = 1;
int global_octaves = 4;

void FillArrayRandom1D(){
	for(int x = 0; x < array_size; x++){
		noise_array1D[x] = Rand(0, 1000);
	}
}
int offset = 0;
void FillArrayRandom2D(){
    for(int y = 0; y < array_size; y++){
	    for(int x = 0; x < array_size; x++){
		    // noise_array2D[y][x] = Rand(0, rand_range);
		    noise_array2D[y][x] = WhiteNoise(x + offset, y + offset);
        }
	}
}

void GeneratePerlin1D(){
	float bias = 1;
	int octaves = 4;
	int count = array_size;

	for(int x = 0; x < array_size; x++){
		float noise = 0.0f;
		float scale = 1.0f;
		float scale_accumulator = 0.0f;

		for(int o = 0; o < octaves; o++){
			int pitch = count >> o;
			int sample1 = (x / pitch) * pitch;
			int sample2 = (sample1 + pitch) % count;

			float blend = (float)(x - sample1) / (float)pitch;

			float sample = (1.0f - blend) * noise_array1D[sample1] + blend * noise_array1D[sample2];

			scale_accumulator += scale;
			noise += sample * scale;
			scale = scale / bias;
		}
		output_array1D[x] = noise / scale_accumulator;
	}
}

void GeneratePerlin2D(){
    float bias = global_bias;
	int octaves = global_octaves;
	int count = array_size;

    for(int y = 0; y < array_size; y++){
        for(int x = 0; x < array_size; x++){
            float noise = 0.0f;
            float scale = 2.0f;
            float scale_accumulator = 0.0f;
            for(int o = 0; o < octaves; o++){
			    int pitch = count >> o;
                int sampleX1 = (x / pitch) * pitch;
                int sampleY1 = (y / pitch) * pitch;

                int sampleX2 = (sampleX1 + pitch) % count;
                int sampleY2 = (sampleY1 + pitch) % count;

                float blendX = (float)(x - sampleX1) / (float)pitch;
                float blendY = (float)(y - sampleY1) / (float)pitch;

                float sampleT = (1.0f - blendX) * noise_array2D[sampleY1][sampleX1] + blendX * noise_array2D[sampleY1][sampleX2];
                float sampleB = (1.0f - blendX) * noise_array2D[sampleY2][sampleX1] + blendX * noise_array2D[sampleY2][sampleX2];

                scale_accumulator += scale;
                noise += (blendY * (sampleB - sampleT) + sampleT) * scale;
                scale = scale / bias;
            }
            output_array2D[y][x] = noise / scale_accumulator;
        }
    }
}

int mode = 1;
void ProceduralTesting(){
	//Rendering
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    //1D perlin noise
    if(mode == 0){
        for(int x = 1; x < array_size; x++){
            SDL_RenderDrawLine(renderer, (x - 1) * SCREEN_WIDTH / array_size, (output_array1D[x - 1] * SCREEN_HEIGHT / 1000), x * SCREEN_WIDTH / array_size, (output_array1D[x] * SCREEN_HEIGHT / 1000));
        }
    }

    //2D perlin noise
    if(mode == 1){
        int rect_size = SCREEN_WIDTH / array_size;
        SDL_Rect rect = {0, 0, 0, 0};
        for(int y = 0; y < array_size; y++){
            for(int x = 0; x < array_size; x++){
                int value = output_array2D[y][x];
                rect = (SDL_Rect){x * rect_size, y * rect_size, rect_size, rect_size};
	            SDL_SetRenderDrawColor(renderer, value, value, value, 255);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

	SDL_RenderPresent(renderer);
}



void Function(){
    // FillArrayRandom1D();
    // GeneratePerlin1D();
    // FillArrayRandom2D();
    // GeneratePerlin2D();
    for(int y = 0; y < array_size; y++){
        for(int x = 0; x < array_size; x++){
            output_array2D[y][x] = Perlin_Get2d(x + offset, y + offset, 0.1, 4) * 255;
        }
    }
}
void Move(){
    offset--;
}
/*void IncreaseBias(){
    global_bias++;
    GeneratePerlin2D();
}
void DecreaseBias(){
    global_bias--;
    GeneratePerlin2D();
}
void IncreaseOctaves(){
    global_octaves++;
    GeneratePerlin2D();
}
void DecreaseOctaves(){
    global_octaves--;
    GeneratePerlin2D();
}*/
void SetupProcedural(){
	// BindKeyEvent(Function, 'g', SDL_KEYDOWN);
	// BindKeyEvent(Move, 'm', SDL_KEYDOWN);
	/*BindKeyEvent(IncreaseBias, '=', SDL_KEYDOWN);
	BindKeyEvent(DecreaseBias, '-', SDL_KEYDOWN);
	BindKeyEvent(IncreaseOctaves, ']', SDL_KEYDOWN);
	BindKeyEvent(DecreaseOctaves, '[', SDL_KEYDOWN);*/
    // FillArrayRandom1D();
    // GeneratePerlin1D();
    // FillArrayRandom2D();
    // GeneratePerlin2D();
    for(int y = 0; y < array_size; y++){
        for(int x = 0; x < array_size; x++){
            output_array2D[y][x] = Perlin_Get2d(x, y, 0.15, 2) * 255;
            // printf("%d\n", (int)(Perlin_Get2d(x, y, 0.1, 4) * 255));
        }
    }
}
