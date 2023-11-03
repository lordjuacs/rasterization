#include <glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "camera.h"
#include "Objeto.h"
#include "glut_ply.h"
#include "stb_image.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void check_colision(vector<Objeto *> &objetos);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(20.0f, 10.0f, 20.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//escala
float escalita = 1.0f;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float rotx;
float roty;
float rotz;
// lighting
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

Piramide piramide;

std::vector<Objeto *> objs;
std::vector<Model_PLY> plys;
char *archivo = "../models/cow.ply";

int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    // build and compile our shader zprogram
    Shader lightingShader("../lighting.vs", "../lighting.fs");
    Esfera sunSphere(lightPos, 5.0f, 200, 200, 1);  // Implement Sphere class as needed
    vec3 sun = vec3(1, 1, 1);
    sunSphere.setup();
    sunSphere.nombre = "sun";

    Shader textureShader("../texture.vs",
                         "../texture.fs");  // Create a new shader program for the sphere


    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("../textures/sun_texture.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    unsigned int textureVaca;
    glGenTextures(1, &textureVaca);
    glBindTexture(GL_TEXTURE_2D, textureVaca);

    data = stbi_load("../textures/cow.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture for 'vaca'" << std::endl;
    }
    stbi_image_free(data);

// Set texture wrapping and filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    float orbital_angle = 0;

    //PLANETAS
    std::vector<pair<Objeto *, vec3>>
            planets;
    planets.emplace_back(&sunSphere, vec3(1, 1, 1));
// Scaling factor for the planets (one-tenth the size of the sun)
    float planetScale = 0.05f;
    int slices = 100, stacks = 100;
// Mercury
    Esfera mercury(vec3(0, 0, 8), 5.0f * planetScale, slices, stacks, 1);  // Radius: 2,439.7 km
    mercury.setup();
    mercury.nombre = "mercury";
    glm::vec3 mercuryColor(0.5f, 0.5f, 0.5f);  // Grayish color for Mercury
    planets.emplace_back(&mercury, mercuryColor);

// Venus
    Esfera venus(glm::vec3(13.0f, 0.0f, 0.0f), 10.0f * planetScale, slices, stacks, 0.8);  // Radius: 6,051.8 km
    venus.setup();
    venus.nombre = "venus";
    glm::vec3 venusColor(1.0f, 1.0f, 0.0f);  // Yellowish color for Venus
    planets.emplace_back(&venus, venusColor);

// Earth
    Esfera earth(glm::vec3(20.0f, 0.0f, 0.0f), 12.742f * planetScale, slices, stacks, 0.74);  // Radius: 6,371 km
    earth.setup();
    earth.nombre = "earth";
    glm::vec3 earthColor(0.0f, 0.0f, 1.0f);  // Blue color for Earth
    planets.emplace_back(&earth, earthColor);

// Mars
    Esfera mars(glm::vec3(30.0f, 0.0f, 0.0f), 6.779f * planetScale, slices, stacks, 1.15);  // Radius: 3,389.5 km
    mars.setup();
    mars.nombre = "mars";
    glm::vec3 marsColor(1.0f, 0.0f, 0.0f);  // Reddish color for Mars
    planets.emplace_back(&mars, marsColor);

// Jupiter
    Esfera jupiter(glm::vec3(45.0f, 0.0f, 0.0f), 139.555f * planetScale, slices, stacks, 0.2);  // Radius: 69,911 km
    jupiter.setup();
    jupiter.nombre = "jupiter";
    glm::vec3 jupiterColor(1.0f, 0.5f, 0.0f);  // Orange color for Jupiter
    planets.emplace_back(&jupiter, jupiterColor);

// Saturn
    Esfera saturn(glm::vec3(60.0f, 0.0f, 0.0f), 116.32f * planetScale, slices, stacks, 1.3);  // Radius: 58,232 km
    saturn.setup();
    saturn.nombre = "saturn";
    glm::vec3 saturnColor(1.0f, 1.0f, 0.0f);  // Yellow color for Saturn
    planets.emplace_back(&saturn, saturnColor);

// Uranus
    Esfera uranus(glm::vec3(75.0f, 0.0f, 0.0f), 50.762f * planetScale, slices, stacks, 0.41);  // Radius: 25,362 km
    uranus.setup();
    uranus.nombre = "uranus";
    glm::vec3 uranusColor(0.0f, 0.5f, 0.7f);  // Bluish-green color for Uranus
    planets.emplace_back(&uranus, uranusColor);

// Neptune
    Esfera neptune(glm::vec3(90.0f, 0.0f, 0.0f), 49.244f * planetScale, slices, stacks, 0.59);  // Radius: 24,622 km
    neptune.setup();
    neptune.nombre = "neptune";
    glm::vec3 neptuneColor(0.0f, 0.0f, 0.5f);  // Deep blue color for Neptune
    planets.emplace_back(&neptune, neptuneColor);

    //PLY objects
    Model_PLY mod;
    mod.Load(archivo);
    mod.nombre = "vaca";
    mod.set_traslacion(vec3(15, 15, 0));
    mod.setup();
    //objs.emplace_back(&mod);
    plys.emplace_back(mod);


    //mod.display(prueba);
    for (auto &p: planets)
        objs.emplace_back(p.first);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        check_colision(objs);
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.071, 0.075, 0.078, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto &mod: plys) {
            mod.rotx = rotx / 100;
            mod.roty = roty / 100;
            mod.rotz = rotz / 100;
            textureShader.use();
            textureShader.setInt("texture", 1);
            mod.display(textureShader);
        }
        // be sure to activate shader when setting uniforms/drawing objects
        for (auto &p: planets) {
            if (p.first->nombre == "sun")
                continue;
            lightingShader.use();
            if (p.first->choque) lightingShader.setVec3("objectColor", p.second * 0.5f);
            else lightingShader.setVec3("objectColor", p.second);
            p.first->display(lightingShader);
            p.first->move_around_point(vec3(0, 0, 0), orbital_angle);
        }
        lightingShader.setVec3("lightColor", sun.x, sun.y, sun.z);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                                100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);


        orbital_angle += 0.005 * 1 / 60;


        textureShader.use();
        textureShader.setInt("texture", 0);
        textureShader.setMat4("projection", projection);
        textureShader.setMat4("view", view);
        textureShader.setMat4("model", glm::mat4(1.0f));  // Set model matrix as needed
        textureShader.setVec3("lightColor", sun.x, sun.y, sun.z);  // Adjust color as needed

        // Bind the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureVaca);

        sunSphere.display(textureShader);  // Render the light sphere

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    /*  glDeleteVertexArrays(1, &cubeVAO);
      glDeleteVertexArrays(1, &lightCubeVAO);
      glDeleteBuffers(1, &VBO);
  */
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        rotx += 2;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        roty += 2;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        rotz += 2;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        for (auto &mod: plys) {
            mod.change_traslacion(vec3(2.0f / 1000.0f, 0, 0));
        }
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        for (auto &mod: plys) {
            mod.change_traslacion(vec3(-2.0f / 1000.0f, 0, 0));
        }
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        for (auto &mod: plys) {
            mod.change_traslacion(vec3(0, 0, -2.0f / 1000.0f));
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        for (auto &mod: plys) {
            mod.change_traslacion(vec3(0, 0, 2.0f / 1000.0f));
        }
    }

    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        for (auto &mod: plys) {
            mod.change_traslacion(vec3(0, -2.0f / 1000.0f, 0));
        }
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        for (auto &mod: plys) {
            mod.change_traslacion(vec3(0, 2.0f / 1000.0f, 0));
        }
    }

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        escalita *= 1.001f;
        for (auto &mod: plys) {
            mod.set_escala(escalita);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        escalita /= 1.001f;
        for (auto &mod: plys) {
            mod.set_escala(escalita);
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void check_colision(vector<Objeto *> &objetos) {
    if (objetos.empty()) return;
    /*for (int i = 0; i < objetos.size() - 1; ++i) {
        for (int j = i + 1; j < objetos.size(); ++j) {
            if (objetos[i]->bs.colision(objetos[j]->bs)) {
                std::cout << "COLISION\n";
                return;
            }
        }
    }*/
    for (auto &mod: plys) {
        for (auto &o: objs) {
            if (mod.bs.colision(o->bs)) {
                std::cout << "COLISION ";
                if(o->nombre == "sun")
                    mod.set_traslacion(vec3(15, 15, 0));
                std::cout << o->nombre << "\n";
                o->choque = true;
                return;
            }
            o->choque = false;
        }
    }
}