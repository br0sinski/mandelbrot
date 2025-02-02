#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>

const int WIDTH = 800, HEIGHT = 600;

const char* vertShader = R"glsl(
#version 330 core
layout(location=0) in vec2 aPos;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)glsl";

const char* fragShader = R"glsl(
#version 330 core
uniform vec2 center;
uniform float zoom;
out vec4 fragColor;

void main() {
    vec2 c = (gl_FragCoord.xy / vec2(800,600) - 0.5) * zoom + center;
    vec2 z = vec2(0.0);
    int iter;
    for(iter=0; iter<256 && dot(z,z)<4.0; iter++) {
        z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;
    }
    float col = float(iter)/256.0;
    fragColor = vec4(col, col, col, 1.0);
}
)glsl";

GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    // Error checking here
    return shader;
}


int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Mandelbrot", 0, 0, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    glewInit();


    GLuint program = glCreateProgram();
    glAttachShader(program, compileShader(GL_VERTEX_SHADER, vertShader));
    glAttachShader(program, compileShader(GL_FRAGMENT_SHADER, fragShader));
    glLinkProgram(program);
    glUseProgram(program);
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    float vertices[] = {-1,-1, 1,-1, -1,1, 1,1};
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    GLint centerLoc = glGetUniformLocation(program, "center");
    GLint zoomLoc = glGetUniformLocation(program, "zoom");

    glm::vec2 center(0.0f);
    float zoom = 1.0f;
    bool running = true;

    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) running = false;
            
            const Uint8* keys = SDL_GetKeyboardState(NULL);
            float speed = 0.1f/zoom;
            if(keys[SDL_SCANCODE_W]) center.y += speed;
            if(keys[SDL_SCANCODE_S]) center.y -= speed;
            if(keys[SDL_SCANCODE_A]) center.x -= speed;
            if(keys[SDL_SCANCODE_D]) center.x += speed;
            if(keys[SDL_SCANCODE_Q]) zoom *= 1.1f;
            if(keys[SDL_SCANCODE_E]) zoom /= 1.1f;
        }


        glUniform2f(centerLoc, center.x, center.y);
        glUniform1f(zoomLoc, zoom);
    //render
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}