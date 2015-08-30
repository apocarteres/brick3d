/*
 main

 Copyright 2012 Thomas Dalling - http://tomdalling.com/

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "platform.hpp"

// third-party libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// standard C++ libraries
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <list>

// tdogl classes
#include "tdogl/Program.h"
#include "tdogl/Texture.h"
#include "tdogl/Camera.h"

//game
#include "game/TetrisGame.h"
#include "game/StdLibRandomProvider.h"
#include "game/test.h"

/*
 Represents a textured geometry asset

 Contains everything necessary to draw arbitrary geometry with a single texture:

  - shaders
  - a texture
  - a VBO
  - a VAO
  - the parameters to glDrawArrays (drawType, drawStart, drawCount)
 */
struct ModelAsset {
    tdogl::Program* shaders;
    tdogl::Texture* texture;
    GLuint vbo;
    GLuint vao;
    GLenum drawType;
    GLint drawStart;
    GLint drawCount;

    ModelAsset() :
    shaders(NULL),
    texture(NULL),
    vbo(0),
    vao(0),
    drawType(GL_TRIANGLES),
    drawStart(0),
    drawCount(0) {
    }
};

/*
 Represents an instance of an `ModelAsset`

 Contains a pointer to the asset, and a model transformation matrix to be used when drawing.
 */
struct ModelInstance {
    ModelAsset* asset;
    glm::mat4 transform;

    ModelInstance() :
    asset(NULL),
    transform() {
    }
};

/*
 Represents a point light
 */
struct Light {
    glm::vec3 position;
    glm::vec3 intensities; //a.k.a. the color of the light
};

// constants
const glm::vec2 SCREEN_SIZE(800, 600);

// globals
GLFWwindow* gWindow = NULL;
double gScrollY = 0.0;
tdogl::Camera gCamera;
ModelAsset gWoodenCrate;
std::list<ModelInstance> gInstances;
GLfloat gDegreesRotated = 0.0f;
Light gLight;
std::map<int, ModelInstance*> blocks;

RandomNumberProvider* rnd_p = new StdLibRandomProvider();
TetrisGame game(rnd_p);

int old = 0;
int wait_time = 0;



// returns a new tdogl::Program created from the given vertex and fragment shader filenames

static tdogl::Program* LoadShaders(const char* vertFilename, const char* fragFilename) {
    std::vector<tdogl::Shader> shaders;
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath(vertFilename), GL_VERTEX_SHADER));
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath(fragFilename), GL_FRAGMENT_SHADER));
    return new tdogl::Program(shaders);
}


// returns a new tdogl::Texture created from the given filename

static tdogl::Texture* LoadTexture(const char* filename) {
    tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile(ResourcePath(filename));
    bmp.flipVertically();
    return new tdogl::Texture(bmp);
}


// initialises the gWoodenCrate global

static void LoadWoodenCrateAsset() {
    // set all the elements of gWoodenCrate
    gWoodenCrate.shaders = LoadShaders("vertex-shader.txt", "fragment-shader.txt");
    gWoodenCrate.drawType = GL_TRIANGLES;
    gWoodenCrate.drawStart = 0;
    gWoodenCrate.drawCount = 6 * 2 * 3;
    gWoodenCrate.texture = LoadTexture("wooden-crate.jpg");
    glGenBuffers(1, &gWoodenCrate.vbo);
    glGenVertexArrays(1, &gWoodenCrate.vao);

    // bind the VAO
    glBindVertexArray(gWoodenCrate.vao);

    // bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.vbo);

    // Make a cube out of triangles (two triangles per side)
    GLfloat vertexData[] = {
        //  X     Y     Z       U     V          Normal
        // bottom
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

        // top
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        // front
        -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        // back
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,

        // left
        -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        // right
        1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof (vertexData), vertexData, GL_STATIC_DRAW);

    // connect the xyz to the "vert" attribute of the vertex shader
    glEnableVertexAttribArray(gWoodenCrate.shaders->attrib("vert"));
    glVertexAttribPointer(gWoodenCrate.shaders->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof (GLfloat), NULL);

    // connect the uv coords to the "vertTexCoord" attribute of the vertex shader
    glEnableVertexAttribArray(gWoodenCrate.shaders->attrib("vertTexCoord"));
    glVertexAttribPointer(gWoodenCrate.shaders->attrib("vertTexCoord"), 2, GL_FLOAT, GL_TRUE, 8 * sizeof (GLfloat), (const GLvoid*) (3 * sizeof (GLfloat)));

    // connect the normal to the "vertNormal" attribute of the vertex shader
    glEnableVertexAttribArray(gWoodenCrate.shaders->attrib("vertNormal"));
    glVertexAttribPointer(gWoodenCrate.shaders->attrib("vertNormal"), 3, GL_FLOAT, GL_TRUE, 8 * sizeof (GLfloat), (const GLvoid*) (5 * sizeof (GLfloat)));

    // unbind the VAO
    glBindVertexArray(0);
}


// convenience function that returns a translation matrix

glm::mat4 translate(GLfloat x, GLfloat y, GLfloat z) {
    return glm::translate(glm::mat4(), glm::vec3(x, y, z));
}


// convenience function that returns a scaling matrix

glm::mat4 scale(GLfloat x, GLfloat y, GLfloat z) {
    return glm::scale(glm::mat4(), glm::vec3(x, y, z));
}


//create all the `instance` structs for the 3D scene, and add them to `gInstances`

static void CreateInstances() {
    ModelInstance* dot = new ModelInstance();
    dot->asset = &gWoodenCrate;
    dot->transform = glm::mat4();
    blocks[0] = dot;
}


//renders a single `ModelInstance`

static void RenderInstance(const ModelInstance& inst) {
    ModelAsset* asset = inst.asset;
    tdogl::Program* shaders = asset->shaders;

    //bind the shaders
    shaders->use();

    auto v = gCamera.matrix() * glm::lookAt(
            glm::vec3(0, 0, 1.0), // Camera in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(-1, 0, 0) // Head is up (set to 0,-1,0 to look upside-down)
            );
    v = glm::translate(v, glm::vec3(-24.0f, -7.0f, 0.0f));

    //set the shader uniforms
    shaders->setUniform("camera", v);
    shaders->setUniform("model", inst.transform);
    shaders->setUniform("tex", 0); //set to 0 because the texture will be bound to GL_TEXTURE0
    shaders->setUniform("light.position", gLight.position);
    shaders->setUniform("light.intensities", gLight.intensities);

    //bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, asset->texture->object());

    //bind VAO and draw
    glBindVertexArray(asset->vao);
    glDrawArrays(asset->drawType, asset->drawStart, asset->drawCount);

    //unbind everything
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    shaders->stopUsing();
}


// draws a single frame

static void Render() {
    // clear everything
    glClearColor(0, 0, 0, 1); // black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //    // render all the instances
    //    std::list<ModelInstance>::const_iterator it;
    //    for(it = gInstances.begin(); it != gInstances.end(); ++it){
    //        RenderInstance(*it);
    //    }

    for (int j = 0; j < GAME_FIELD_COLS; ++j) {
        for (int i = 0; i < GAME_FIELD_ROWS; ++i) {
            if (!game.is_free(i, j)) {
                glm::mat4 Model = glm::mat4(1.0f);
                Model = glm::translate(Model, glm::vec3(i, j, 0.0) * 2.0f);
                //ModelInstance* m = blocks[std::ceil(game.get_color(i, j).w)];
                ModelInstance* m = blocks[0];
                ModelInstance r;
                r.asset = m->asset;
                r.transform = Model;
                RenderInstance(r);
            }
        }
    }


    // swap the display buffers (displays what was just drawn)
    glfwSwapBuffers(gWindow);
}

void update_game_state() {
    if (GAME_OVER == game.process()) {
        std::cout << "game is over" << std::endl;
        exit(0);
    }
}

// update the scene based on the time elapsed since last update
float t = 0;

static void Update(float secondsElapsed) {

//        if (glfwGetKey(gWindow, 'W')) {
//            game.rotate();
//        } else if (glfwGetKey(gWindow, 'A')) {
//            game.move_left();
//        } else if (glfwGetKey(gWindow, 'D')) {
//            game.move_right();
//        } else if (glfwGetKey(gWindow, 'S')) {
//            game.drop();
//        }
//        
    
    
    t += secondsElapsed;
    if (t >= 1) {
        for (int i = 1; i < t; ++i) {
            update_game_state();
        }
        t = 0;
    }



    //    switch (key) {
    //        case 'a': game.move_left();
    //            break;
    //        case 'd': game.move_right();
    //            break;
    //        case 'w': game.rotate();
    //            break;
    //        case 's': game.drop();
    //    }

    //rotate the first instance in `gInstances`
    //    const GLfloat degreesPerSecond = 180.0f;
    //    gDegreesRotated += secondsElapsed * degreesPerSecond;
    //    while(gDegreesRotated > 360.0f) gDegreesRotated -= 360.0f;
    //    gInstances.front().transform = glm::rotate(glm::mat4(), glm::radians(gDegreesRotated), glm::vec3(0,1,0));
    //
    //    //move position of camera based on WASD keys, and XZ keys for up and down
    //    const float moveSpeed = 4.0; //units per second
    //    if(glfwGetKey(gWindow, 'S')){
    //        gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.forward());
    //    } else if(glfwGetKey(gWindow, 'W')){
    //        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.forward());
    //    }
    //    if(glfwGetKey(gWindow, 'A')){
    //        gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.right());
    //    } else if(glfwGetKey(gWindow, 'D')){
    //        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.right());
    //    }
    //    if(glfwGetKey(gWindow, 'Z')){
    //        gCamera.offsetPosition(secondsElapsed * moveSpeed * -glm::vec3(0,1,0));
    //    } else if(glfwGetKey(gWindow, 'X')){
    //        gCamera.offsetPosition(secondsElapsed * moveSpeed * glm::vec3(0,1,0));
    //    }
    //
    //    //move light
    //    if(glfwGetKey(gWindow, '1'))
    //        gLight.position = gCamera.position();
    //
    //    // change light color
    //    if(glfwGetKey(gWindow, '2'))
    //        gLight.intensities = glm::vec3(1,0,0); //red
    //    else if(glfwGetKey(gWindow, '3'))
    //        gLight.intensities = glm::vec3(0,1,0); //green
    //    else if(glfwGetKey(gWindow, '4'))
    //        gLight.intensities = glm::vec3(1,1,1); //white


    //rotate camera based on mouse movement
    //    const float mouseSensitivity = 0.1f;
    //    double mouseX, mouseY;
    //    glfwGetCursorPos(gWindow, &mouseX, &mouseY);
    //    gCamera.offsetOrientation(mouseSensitivity * (float)mouseY, mouseSensitivity * (float)mouseX);
    //    glfwSetCursorPos(gWindow, 0, 0); //reset the mouse, so it doesn't go out of the window

    //increase or decrease field of view based on mouse wheel
    //    const float zoomSensitivity = -0.2f;
    //    float fieldOfView = gCamera.fieldOfView() + zoomSensitivity * (float)gScrollY;
    //    if(fieldOfView < 5.0f) fieldOfView = 5.0f;
    //    if(fieldOfView > 130.0f) fieldOfView = 130.0f;
    //    gCamera.setFieldOfView(fieldOfView);
    //    gScrollY = 0;
}

void my_tex() {
    //    for (int i = 0; i < 10; ++i) {
    //        std::stringstream sstm;
    //        sstm << "res/t" << i << ".raw";
    //        GLuint texture = LoadTextureRAW(sstm.str().c_str(), 0, 64, 64);
    //        if (!texture) {
    //            std::cout << "texture is not loaded" << std::endl;
    //            exit(1);
    //        }
    //        blocks[i] = new Block(&shader, &vbo, texture);
    //    }
    //

}

void my_kp(unsigned char key, int x, int y) {
    //    switch (key) {
    //        case 'a': game.move_left();
    //            break;
    //        case 'd': game.move_right();
    //            break;
    //        case 'w': game.rotate();
    //            break;
    //        case 's': game.drop();
    //    }
}

void my_r() {
    //    int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
    //    int delta = (timeSinceStart - old);
    //    wait_time += delta;
    //    if (wait_time >= 500) {
    //        update_game_state();
    //        wait_time = 0;
    //    }
    //    old = timeSinceStart;
    //
    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //    //            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe
    //
    //    for (int i = 0; i < GAME_FIELD_ROWS; ++i) {
    //        for (int j = 0; j < GAME_FIELD_COLS; ++j) {
    //            if (!game.is_free(i, j)) {
    //                glm::mat4 Model = glm::mat4(1.0f);
    //                //                Model = glm::rotate(Model, angle, glm::vec3(1.0, -1.0, 0.0));
    //                Model = glm::translate(Model, glm::vec3(i, j, 0.0) * 2.0f);
    //                Block* block = blocks[std::ceil(game.get_color(i, j).w)];
    //                GlShader* shader = block->getShader();
    //                shader->setUniform(shader->getUniformLocation("pvm_matrix"),
    //                        Projection * glm::translate(View, glm::vec3(-24.0f, -7.0f, 0.0f)) * Model);
    //                blockRenderer.draw(*block);
    //            }
    //        }
    //    }
    //    glutSwapBuffers();

}

// records how far the y axis has been scrolled

void OnScroll(GLFWwindow* window, double deltaX, double deltaY) {
    gScrollY += deltaY;
}

void OnError(int errorCode, const char* msg) {
    throw std::runtime_error(msg);
}

void onKey(GLFWwindow* window, int keyCode, int b, int mode, int d) {
    if (mode == GLFW_PRESS){
        switch (keyCode) {
            case GLFW_KEY_A: game.move_left();
                break;
            case GLFW_KEY_D: game.move_right();
                break;
            case GLFW_KEY_W: game.rotate();
                break;
            case GLFW_KEY_S: game.drop();
                break;
        }
    }
}

// the program starts here

void AppMain() {
    // initialise GLFW
    glfwSetErrorCallback(OnError);
    if (!glfwInit())
        throw std::runtime_error("glfwInit failed");

    // open a window with GLFW
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    gWindow = glfwCreateWindow((int) SCREEN_SIZE.x, (int) SCREEN_SIZE.y, "OpenGL Tutorial", NULL, NULL);
    if (!gWindow)
        throw std::runtime_error("glfwCreateWindow failed. Can your hardware handle OpenGL 3.2?");

    // GLFW settings
    glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(gWindow, 0, 0);
    glfwSetScrollCallback(gWindow, OnScroll);
    glfwMakeContextCurrent(gWindow);
    glfwSetKeyCallback(gWindow, onKey);

    // initialise GLEW
    glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
    if (glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed");

    // GLEW throws some errors, so discard all the errors so far
    while (glGetError() != GL_NO_ERROR) {
    }

    // print out some info about the graphics drivers
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    // make sure OpenGL version 3.2 API is available
    if (!GLEW_VERSION_3_2)
        throw std::runtime_error("OpenGL 3.2 API is not available.");

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // initialise the gWoodenCrate asset
    LoadWoodenCrateAsset();

    // create all the instances in the 3D scene based on the gWoodenCrate asset
    CreateInstances();

    // setup gCamera
    gCamera.setPosition(glm::vec3(0, 0, 50));
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);
    gCamera.setNearAndFarPlanes(0.1f, 100.0f);

    // setup gLight
    gLight.position = gCamera.position();
    gLight.intensities = glm::vec3(1, 1, 1); //white

    // run while the window is open
    float lastTime = (float) glfwGetTime();
    while (!glfwWindowShouldClose(gWindow)) {
        // process pending events
        glfwPollEvents();

        // update the scene based on the time elapsed since last update
        float thisTime = (float) glfwGetTime();
        Update(thisTime - lastTime);
        lastTime = thisTime;

        // draw one frame
        Render();

        // check for errors
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
            std::cerr << "OpenGL Error " << error << std::endl;

        //exit program if escape key is pressed
        if (glfwGetKey(gWindow, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(gWindow, GL_TRUE);
    }

    // clean up and exit
    glfwTerminate();
}

int main(int argc, char *argv[]) {
    runTests();
    try {
        AppMain();
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
