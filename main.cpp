#include "shaders/shader.h"
#include "window/window.h"
#include <glm/glm.hpp>
#include <mpfr.h>

#define MIN(a, b) ((a) < (b)) ? (a) : (b)

// Init window
Window window("Mandelbrot");
glm::vec2 resolution = glm::vec2(window.get_screen_width(), window.get_screen_height());
const double aspect = static_cast<double>(resolution.x) / resolution.y;

int iterations = 300;
double scale = 3.0f;
double mouseX = static_cast<double>(resolution.x) / 2;
double mouseY = static_cast<double>(resolution.y) / 2;
double centerX = 0.0f; // static_cast<double>(resolution.x) / 2;
double centerY = 0.0f; // static_cast<double>(resolution.y) / 2;
double lastMouseX = 0.0f, lastMouseY = 0.0f;
// bool isOrbitDirty = true;
double last_time = glfwGetTime();
double frames = 0.0f;

float vertices[] = {
    -1.0f, -1.0f, // lower left side
    1.0f,  -1.0f, // lower right side
    1.0f,  1.0f,  // top right side
    -1.0f, 1.0f   // top left side
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
std::vector<glm::dvec2> compute_reference_orbit(double c_re, double c_im, int max_iter, int pb);
void updateFPS();

int main() {

    // TODO: подобрать формулу для плавного увеличения итераций при сильном приближении
    // (для избежания glitches)
    int pb = 2048;
    // if (scale < 1e-4)
    //     pb = 128;
    // if (scale < 1e-6)
    //     pb = 256;
    // if (scale < 1e-8)
    //     pb = 512;
    // if (scale < 1e-10)
    //     pb = 1024;
    // if (scale < 1e-12)
    //     pb = 2048;

    // Init Vertex Buffer Object (VBO)
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Init Vertex Array Object (VAO)
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    std::vector<glm::dvec2> reference_orbit = compute_reference_orbit(0.0, 0.0, iterations, pb);

    // Init Shader Storage Buffer Object (SSBO)
    GLuint SSBO;
    glGenBuffers(1, &SSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);

    // if (isOrbitDirty) {
    glBufferData(GL_SHADER_STORAGE_BUFFER, reference_orbit.size() * sizeof(glm::dvec2), reference_orbit.data(),
                 GL_DYNAMIC_DRAW);
    // glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, iterations * sizeof(glm::dvec2), 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    //     isOrbitDirty = false;
    // }

    // Init shaders
    Shader shader("shaders/shader.vert", "shaders/shader.frag");

    // Callbacks
    glfwSetFramebufferSizeCallback(window.get(), framebuffer_size_callback);
    glfwSetScrollCallback(window.get(), scroll_callback);
    glfwSetCursorPosCallback(window.get(), mouse_callback);
    glfwSetKeyCallback(window.get(), key_callback);

    // Window
    while (!glfwWindowShouldClose(window.get())) {
        // std::cout << "centerX: " << centerX << ", centerY: " << centerY << std::endl;
        // std::cout << orbit.x << " " << orbit.y << std::endl;

        // W to zoom IN
        if (glfwGetKey(window.get(), GLFW_KEY_W) == GLFW_PRESS) {
            scroll_callback(window.get(), 0, 1);
        }

        // S to zoom OUT
        if (glfwGetKey(window.get(), GLFW_KEY_S) == GLFW_PRESS) {
            scroll_callback(window.get(), 0, -1);
        }

        // Show Frames Per Second
        updateFPS();

        // Using shaders
        shader.use();

        // Clamp
        scale = MIN(scale, 3.0f);

        // Uniforms
        glUniform1d(glGetUniformLocation(shader.getID(), "u_scale"), scale);
        glUniform1i(glGetUniformLocation(shader.getID(), "u_iterations"), iterations);
        glUniform1f(glGetUniformLocation(shader.getID(), "u_time"), glfwGetTime());
        glUniform1d(glGetUniformLocation(shader.getID(), "u_centerX"), centerX);
        glUniform1d(glGetUniformLocation(shader.getID(), "u_centerY"), centerY);
        glUniform1d(glGetUniformLocation(shader.getID(), "u_aspect"), aspect);
        glUniform2f(glGetUniformLocation(shader.getID(), "u_resolution"), resolution.x, resolution.y);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window.get());
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &SSBO);

    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    double zoom = (yoffset > 0) ? 0.98f : 1.02f;
    double offset = 0.5f;

    double normX = mouseX / static_cast<double>(resolution.x) - offset;
    double normY = mouseY / static_cast<double>(resolution.y) - offset;

    double preX = centerX + normX * scale * aspect;
    double preY = centerY + normY * scale * aspect;

    scale *= zoom;
    // std::cout << "Scale: " << scale << std::endl;
    // isOrbitDirty = true;

    double postX = centerX + normX * scale * aspect;
    double postY = centerY + normY * scale * aspect;

    centerX += (preX - postX);
    centerY += (preY - postY);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    static bool wasPressed = false;
    bool isPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    if (isPressed && !wasPressed) {
        lastMouseX = xpos;
        lastMouseY = ypos;
    }

    if (isPressed && wasPressed) {
        double dx = xpos - lastMouseX;
        double dy = ypos - lastMouseY;

        centerX -= dx / resolution.x * scale; // * aspect;
        centerY -= dy / resolution.y * scale;
        // isOrbitDirty = true;

        lastMouseX = xpos;
        lastMouseY = ypos;
    }

    wasPressed = isPressed;

    mouseX = xpos;
    mouseY = ypos;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {

    // ESC or Q to close the program
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // F to set default view
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        scale = 3.0f;
        centerX = 0.0f;
        centerY = 0.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        iterations = 1;
    }

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        centerY -= scale * aspect;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        centerY += scale * aspect;
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        centerX -= scale * aspect;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        centerX += scale * aspect;
    }

    // ENTER to add one iteration
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        iterations++;
        std::cout << "Iteration: " << iterations << std::endl;
    }

    // SPACE to substract one iteration
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && iterations >= 2) {
        iterations--;
        std::cout << "Iteration: " << iterations << std::endl;
    }
}

std::vector<glm::dvec2> compute_reference_orbit(double c_re /* centerX */, double c_im /* centerY */, int iterations,
                                                int pb) {

    mpfr_t c_x, c_y, zx, zy, tmp_x, tmp_y, xn_sqr, yn_sqr, tmp_sqr;
    mpfr_t x2, y2, r2;
    mpfr_t xnyn, xnyn_dbl;

    mpfr_init2(tmp_x, pb);
    mpfr_init2(tmp_y, pb);
    mpfr_init2(xn_sqr, pb);
    mpfr_init2(yn_sqr, pb);
    mpfr_init2(tmp_sqr, pb);
    mpfr_init2(c_x, pb);
    mpfr_init2(c_y, pb);
    mpfr_init2(zx, pb);
    mpfr_init2(zy, pb);
    mpfr_init2(x2, pb);
    mpfr_init2(y2, pb);
    mpfr_init2(r2, pb);
    mpfr_init2(xnyn, pb);
    mpfr_init2(xnyn_dbl, pb);

    // z_0 computed with 2048 precision bits
    // mpfr_set_ld(z0, 0.0, MPFR_RNDN);
    mpfr_set_ld(zx, 0.0, MPFR_RNDN);
    mpfr_set_ld(zy, 0.0, MPFR_RNDN);

    // c_x and c_y computed with 2048 precision bits
    mpfr_set_ld(c_x, c_re, MPFR_RNDN);
    mpfr_set_ld(c_y, c_im, MPFR_RNDN);

    bool escaped = false;
    std::vector<glm::dvec2> reference_orbit; // сохраняем точки (x_n; y_n) n = 0,1,...,iterations
    reference_orbit.reserve(iterations);

    for (int i = 0; i < iterations; i++) {
        reference_orbit.emplace_back(mpfr_get_ld(zx, MPFR_RNDN), mpfr_get_ld(zy, MPFR_RNDN));

        // r^2 = x^2 + y^2
        mpfr_sqr(x2, zx, MPFR_RNDN);
        mpfr_sqr(y2, zy, MPFR_RNDN);
        mpfr_add(r2, x2, y2, MPFR_RNDN);

        if (mpfr_cmp_d(r2, 4) > 0) {
            escaped = true;
            break;
        }

        mpfr_sqr(xn_sqr, zx, MPFR_RNDN);
        mpfr_sqr(yn_sqr, zy, MPFR_RNDN);
        mpfr_sub(tmp_sqr, xn_sqr, yn_sqr, MPFR_RNDN);
        mpfr_add(tmp_x, tmp_sqr, c_x, MPFR_RNDN);

        mpfr_mul(xnyn, zx, zy, MPFR_RNDN);
        mpfr_mul_ui(xnyn_dbl, xnyn, 2, MPFR_RNDN);
        mpfr_add(tmp_y, xnyn_dbl, c_y, MPFR_RNDN);

        mpfr_set(zx, tmp_x, MPFR_RNDN);
        mpfr_set(zy, tmp_y, MPFR_RNDN);
    }

    if (escaped) {
        std::cout << "Reference orbit is out\n";
    }

    mpfr_clears(c_x, c_y, zx, zy, tmp_x, tmp_y, xn_sqr, yn_sqr, tmp_sqr, x2, y2, r2, xnyn, xnyn_dbl, nullptr);

    return reference_orbit;
}

void updateFPS() {
    double current_time = glfwGetTime();
    frames++;
    if (current_time - last_time >= 0.5f) {
        printf("FPS: %.2f\n", frames);
        frames = 0;
        last_time += 1;
    }
}
