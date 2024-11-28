#include <iostream>
#include <fmt/core.h>
#include <GLFW/glfw3.h>

#include "fps_counter.h"

#define WIDTH 1600
#define HEIGHT 900

const double x_min = -2;
const double x_max = 1;
const double y_min = -1;
const double y_max = 1;
int maxIteration = 100;

const int PALETTE_SIZE = 16;
const GLuint color_ramp[] = {
    0xFFFF1010, 0xFFEE0E1B, 0xFFDD0D27, 0xFFCC0C32, 0xFFBB0B3E, 0xFFAA0A4A,
    0xFF990955, 0xFF880861, 0xFF77076C, 0xFF660678, 0xFF550584, 0xFF44048F,
    0xFF33039B, 0xFF2202A6, 0xFF1101B2, 0xFF0000BE
};

static GLFWwindow* window = nullptr;
GLuint textureID;
GLuint* pixel_buffer = nullptr;

fps_counter fps;

static void mandelbrotCpu()
{
    int black = 0x000000;
    int white = 0xFFFFFF;
    pixel_buffer = new GLuint[WIDTH * HEIGHT]; // Allocate memory for pixel_buffer
/*#pragma omp parallel for num_threads(60)*/
#pragma omp parallel for num_threads(24)
    for (int px = 0; px < WIDTH; px++)
    {
        for (int py = 0; py < HEIGHT; py++)
        {
            double x0 = x_min + (x_max - x_min) * px / WIDTH;
            double y0 = y_min + (y_max - y_min) * py / HEIGHT;
            double x = 0.0;
            double y = 0.0;
            int iteration = 0;
            double R = 2.0;
            while (x * x + y * y <= R * R && iteration < maxIteration)
            {
                double tempX = x * x - y * y + x0;
                y = 2 * x * y + y0;
                x = tempX;
                iteration++;
            }
            pixel_buffer[py * WIDTH + px] = (iteration == maxIteration) ? black : color_ramp[iteration % PALETTE_SIZE];
        }
    }

    // Draw the texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_buffer);
    glBegin(GL_QUADS);
    {
        glTexCoord2f(0, 1);
        glVertex3f(-1, -1, 0);
        glTexCoord2f(0, 0);
        glVertex3f(-1, 1, 0);
        glTexCoord2f(1, 0);
        glVertex3f(1, 1, 0);
        glTexCoord2f(1, 1);
        glVertex3f(1, -1, 0);
    }
    glEnd();
}

static void paint()
{
    fps.update();
    mandelbrotCpu();
}

static void initTexteures()
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

static void loop()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        paint();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

static void init()
{
    glfwSetErrorCallback([](int error, const char* description)
    {
        std::cerr << "Error: " << description << std::endl;
    });

    if (!glfwInit())
        throw std::runtime_error("Unable to initialize GLFW");

    // Configure GLFW
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // Create the window
    window = glfwCreateWindow(WIDTH, HEIGHT, "Ejemplo 01!", NULL, NULL);
    if (!window)
        throw std::runtime_error("Failed to create the GLFW window");

    // Setup a key callback. It will be called every time a key is pressed, repeated or released.
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
            glfwSetWindowShouldClose(window, true);
    });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    });

    // Center the window
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* vidmode = glfwGetVideoMode(primaryMonitor);
    glfwSetWindowPos(window, (vidmode->width - WIDTH) / 2, (vidmode->height - HEIGHT) / 2);

    // Make the OpenGL context current
    glfwMakeContextCurrent(window);
    std::string version = (char*)glGetString(GL_VERSION);
    std::string vendor = (char*)glGetString(GL_VENDOR);
    std::string renderer = (char*)glGetString(GL_RENDERER);

    fmt::println("OpenGL Version: {}", version);
    fmt::println("OpenGL Vendor: {}", vendor);
    fmt::println("OpenGL Renderer: {}", renderer);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);

    // Enable v-sync
    glfwSwapInterval(1);

    // Make the window visible
    glfwShowWindow(window);

    initTexteures();
}

static void run()
{
    init();
    loop();
}

int main()
{
   pixel_buffer = new GLuint[WIDTH * HEIGHT];
    run();
    delete[] pixel_buffer;
    glfwTerminate();
    return 0;
}
