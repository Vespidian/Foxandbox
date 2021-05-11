#include "global.h"
#include "gl_utils.h"

#include "event.h"
#include "renderer/renderer.h"
#include "renderer/quad.h"

#include "gl_context.h"

SDL_GLContext gl_context;

mat4 projection_matrix;
int z_depth = 1000;

TilesheetObject tilesheet_texture;
TextureObject crate_tex;

static void WindowResize(EventData event);

int InitGL(){
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK){
        DebugLog(D_ERR, "Failed to initialize GLEW!");
        return EXIT_FAILURE;
    }
	SDL_GL_SetSwapInterval(1);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.5f, 0.4f, 0.3f, 1);
	glm_ortho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, -z_depth / 2, z_depth / 2, projection_matrix);

	BindEvent(EV_ACCURATE, SDL_WINDOWEVENT, WindowResize);


	InitGLUtils();

	RendererInit();


    tilesheet_texture = LoadTilesheet("../images/testingTemp/tmpTilesheet.png", 16, 16);
	crate_tex = LoadTexture("../images/crate.png");


    DebugLog(D_ACT, "Initialized OpenGL");
    GLCall;


    return 0;
}

void RenderGL(){
	SDL_Color tint = {1, 1, 1, 1};
	SDL_Rect dst = {0, 0, 128, 128};
	// SDL_Rect src = {0, 0, 16, 16};

	
	tint = (SDL_Color){0, 1, 1, 1};
	dst = (SDL_Rect){128, 128, 64, 64};
	RenderQuad(crate_tex, NULL, &dst, 1, tint, (float)(SDL_GetTicks()) / 6.3 + 35);

	// tint = (SDL_Color){1, 1, 1, 1};
	// dst = (SDL_Rect){sin(SDL_GetTicks()/300.0)*64 + 256, 64, 64, 64};
	// src = (SDL_Rect){48, 0, 16, 16};
	// RenderQuad(tilesheet_texture.texture, &src, &dst, 0, tint, (float)1000-(SDL_GetTicks()) / 6.3 + 35);

	// ParticleRender((SDL_Rect){48, 48, 16, 16}, (SDL_Color){sin(SDL_GetTicks() / 100.0), cos(SDL_GetTicks() / 100.0), 1, 1});
	// int tilesize = 32;
	// for(int y = 0; y < 8; y++){
	// 	for(int x = 0; x < 6; x++){
	// 		dst = (SDL_Rect){x * tilesize, y * tilesize, tilesize, tilesize};
	// 		MaskedRender(tilesheet_texture, 1, tilesheet_texture, 2, y * 6 + x, dst, 0, tint);
	// 	}
	// }

	// for(int y = 0; y < 8; y++){
	// 	for(int x = 0; x < 6; x++){
	// 		dst = (SDL_Rect){x * tilesize + tilesize * 6, y * tilesize, tilesize, tilesize};
	// 		MaskedRender(tilesheet_texture, 15, tilesheet_texture, 2, y * 6 + x, dst, 0, tint);
	// 	}
	// }

	// for(int y = 0; y < 8; y++){
	// 	for(int x = 0; x < 6; x++){
	// 		dst = (SDL_Rect){x * tilesize, y * tilesize + tilesize * 8, tilesize, tilesize};
	// 		MaskedRender(tilesheet_texture, 3, tilesheet_texture, 2, y * 6 + x, dst, 0, tint);
	// 	}
	// }

	// dst = (SDL_Rect){256, 256, 64, 64};
	// TilesheetRender(tilesheet_texture, 4, dst, 1, tint);


	PushRender();
}

static void WindowResize(EventData event){
	if(event.e->window.event == SDL_WINDOWEVENT_RESIZED){
		SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glm_ortho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, -z_depth / 2, z_depth / 2, projection_matrix);
	}
}