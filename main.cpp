#include "shaders/shader.h"
#include "window/window.h"
#include <glm/glm.hpp>
#include <mpfr.h>

#define MIN(a, b) ((a) < (b)) ? (a) : (b)
#define MAX_ITERATIONS 300

// Init window
Window window("Mandelbrot");
int SCREEN_WIDTH = window.get_screen_width();
int SCREEN_HEIGHT = window.get_screen_height();
const double aspect = static_cast<double>(SCREEN_WIDTH) / SCREEN_HEIGHT;

double scale = 3.0f;
double mouseX, mouseY;
double centerX = 0.0f, centerY = 0.0f;
double lastMouseX = 0.0f, lastMouseY = 0.0f;
glm::dvec2 complex = glm::dvec2(0.0f, 0.0f);
bool orbitDirty = true;

float vertices[] = {
    -1.0f, -1.0f, // lower left side
    1.0f,  -1.0f, // lower right side
    1.0f,  1.0f,  // top right side
    -1.0f, 1.0f   // top left side
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
std::vector<glm::vec2> computeReferenceOrbitMPFR(double c_re, double c_im, int max_iter, int precision_bits);

int main() {

    int precision_bits = 64;
    if (scale < 1e-4)
        precision_bits = 128;
    if (scale < 1e-6)
        precision_bits = 256;
    if (scale < 1e-8)
        precision_bits = 512;
    if (scale < 1e-10)
        precision_bits = 1024;
    if (scale < 1e-12)
        precision_bits = 2048;

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

    // Init Shader Storage Buffer Object (SSBO)
    GLuint SSBO;
    glGenBuffers(1, &SSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);

    if (orbitDirty) {
        std::vector<glm::vec2> orbit = computeReferenceOrbitMPFR(centerX, centerY, MAX_ITERATIONS, precision_bits);
        glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_ITERATIONS * sizeof(glm::dvec2), orbit.data(), GL_DYNAMIC_DRAW);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, MAX_ITERATIONS * sizeof(glm::dvec2), 0);
        // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        orbitDirty = false;
    }

    // Init shaders
    Shader shader("shaders/shader.vert", "shaders/shader.frag");

    // Callbacks
    glfwSetFramebufferSizeCallback(window.get(), framebuffer_size_callback);
    glfwSetScrollCallback(window.get(), scroll_callback);
    glfwSetCursorPosCallback(window.get(), mouse_callback);

    // Window
    while (!glfwWindowShouldClose(window.get())) {

        // ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplGlfw3_NewFrame();
        // ImGui::CreateContext();
        // ImGui::NewFrame();

        // ESC or Q to close the program
        if (glfwGetKey(window.get(), GLFW_KEY_ESCAPE) == GLFW_PRESS ||
            glfwGetKey(window.get(), GLFW_KEY_Q) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window.get(), true);
        }

        // Using W and S arrow keys to control
        if (glfwGetKey(window.get(), GLFW_KEY_W) == GLFW_PRESS) {
            scroll_callback(window.get(), 0, 1);
        }
        if (glfwGetKey(window.get(), GLFW_KEY_S) == GLFW_PRESS) {
            scroll_callback(window.get(), 0, -1);
        }

        // ImGui::Begin("Text");
        // ImGui::Begin("TextText");
        // ImGui::End();

        // Using shaders
        shader.use();

        // Clamp
        scale = MIN(scale, 3.0f);

        // Uniforms
        glUniform1d(glGetUniformLocation(shader.getID(), "scale"), scale);
        glUniform1i(glGetUniformLocation(shader.getID(), "max_iterations"), MAX_ITERATIONS);
        glUniform1f(glGetUniformLocation(shader.getID(), "time"), glfwGetTime());
        glUniform1d(glGetUniformLocation(shader.getID(), "centerX"), centerX);
        glUniform1d(glGetUniformLocation(shader.getID(), "centerY"), centerY);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        // glEnable(GL_TEXTURE_2D);
        // ImGui::Render();
        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

    double normX = mouseX / static_cast<double>(SCREEN_WIDTH) - 0.5f;
    double normY = mouseY / static_cast<double>(SCREEN_HEIGHT) - 0.5f;

    double preX = centerX + normX * scale * aspect;
    double preY = centerY + normY * scale * aspect;

    scale *= zoom;
    std::cout << "Scale: " << scale << std::endl;
    // MAX_ITERATIONS += (150 * log(1 / zoom));
    // std::cout << "MaxIter: " << MAX_ITERATIONS << std::endl;
    orbitDirty = true;

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

        centerX -= dx / SCREEN_WIDTH * scale * aspect;
        centerY -= dy / SCREEN_HEIGHT * scale;
        orbitDirty = true;

        lastMouseX = xpos;
        lastMouseY = ypos;
    }

    wasPressed = isPressed;

    mouseX = xpos;
    mouseY = ypos;
}

std::vector<glm::vec2> computeReferenceOrbitMPFR(double c_re, double c_im, int max_iter, int precision_bits) {
    mpfr_t z_re, z_im, z_re2, z_im2, C_re, C_im, four;

    mpfr_init2(z_re, precision_bits);
    mpfr_init2(z_im, precision_bits);
    mpfr_init2(z_re2, precision_bits);
    mpfr_init2(z_im2, precision_bits);
    mpfr_init2(C_re, precision_bits);
    mpfr_init2(C_im, precision_bits);
    mpfr_init2(four, precision_bits);

    mpfr_set_d(C_re, c_re, MPFR_RNDN);
    mpfr_set_d(C_im, c_im, MPFR_RNDN);
    mpfr_set_d(z_re, 0.0, MPFR_RNDN);
    mpfr_set_d(z_im, 0.0, MPFR_RNDN);
    mpfr_set_d(four, 4.0, MPFR_RNDN);

    std::vector<glm::vec2> orbit(max_iter);

    for (int i = 0; i < max_iter; i++) {
        orbit[i].x = mpfr_get_d(z_re, MPFR_RNDN);
        orbit[i].y = mpfr_get_d(z_im, MPFR_RNDN);

        // z = z^2 + c
        // tmp_re = z_re^2 - z_im^2
        mpfr_mul(z_re2, z_re, z_re, MPFR_RNDN);
        mpfr_mul(z_im2, z_im, z_im, MPFR_RNDN);
        mpfr_sub(z_re2, z_re2, z_im2, MPFR_RNDN);

        // z_im2 = 2 * z_re * z_im
        mpfr_mul(z_im2, z_re, z_im, MPFR_RNDN);
        mpfr_mul_ui(z_im2, z_im2, 2, MPFR_RNDN);

        // + c
        mpfr_add(z_re, z_re2, C_re, MPFR_RNDN);
        mpfr_add(z_im, z_im2, C_im, MPFR_RNDN);

        mpfr_mul(z_re2, z_re, z_re, MPFR_RNDN);
        mpfr_mul(z_im2, z_im, z_im, MPFR_RNDN);
        mpfr_add(z_re2, z_re2, z_im2, MPFR_RNDN);

        if (mpfr_cmp(z_re2, four) > 0) {
            for (int j = i + 1; j < max_iter; j++) {
                orbit[j] = orbit[i];
            }
            break;
        }
    }

    mpfr_clears(z_re, z_im, z_re2, z_im2, C_re, C_im, four, nullptr);

    return orbit;
}
