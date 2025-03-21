#include <GLFW/glfw3.h>
#include <Windows.h>
#include <vector>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <opencv2/opencv.hpp>

#define _USE_MATH_DEFINES


void SaveImage(const char* filepath, GLFWwindow* w) { // Сохранение изображения в файл
    int width, height;
    //glfwGetWindowSize(w, &width, &height);
    glfwGetFramebufferSize(w, &width, &height);
    GLsizei nrChannels = 3;
    GLsizei stride = nrChannels * width;
    stride += (stride % 4) ? (4 - stride % 4) : 0;
    GLsizei bufferSize = stride * height;
    std::vector<char> buffer(bufferSize);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
    stbi_flip_vertically_on_write(true);
    stbi_write_png(filepath, width, height, nrChannels, buffer.data(), stride);
}

void addNoise(const std::string& inputPath, const std::string& outputPath) { // Добавление шума с использованием библиотеки OpenCV
    cv::Mat image = cv::imread(inputPath);
    if (image.empty()) {
        std::cerr << "Could not open or find the image!\n";
        return;
    }

    cv::Mat noise(image.size(), image.type());
    cv::randn(noise, 0, 50); // Добавляем гауссовский шум
    cv::add(image, noise, image);

    cv::imwrite(outputPath, image);
}

cv::Mat applyMedianFilterManual(const cv::Mat& input, int windowSize) { // Собственная реализация медианного фильтра
    cv::Mat output = input.clone();
    int halfWindow = windowSize / 2;

    for (int i = halfWindow; i < input.rows - halfWindow; i++) {
        for (int j = halfWindow; j < input.cols - halfWindow; j++) {
            std::vector<uchar> window_red, window_green, window_blue;
            for (int ki = -halfWindow; ki <= halfWindow; ki++) {
                for (int kj = -halfWindow; kj <= halfWindow; kj++) {
                    cv::Vec3b pixel = input.at<cv::Vec3b>(i + ki, j + kj);
                    window_red.push_back(pixel[2]);   // Красный канал
                    window_green.push_back(pixel[1]); // Зеленый канал
                    window_blue.push_back(pixel[0]);  // Синий канал
                }
            }
            std::sort(window_red.begin(), window_red.end());
            std::sort(window_green.begin(), window_green.end());
            std::sort(window_blue.begin(), window_blue.end());

            output.at<cv::Vec3b>(i, j) = cv::Vec3b(
                window_blue[window_blue.size() / 2],  // Синий канал
                window_green[window_green.size() / 2], // Зеленый канал
                window_red[window_red.size() / 2]     // Красный канал
            );
        }
    }

    return output;
}

double calculateMSE(const cv::Mat& img1, const cv::Mat& img2) { // Оценка качества применённого фильтра - среднеквадратичное отклонение
    cv::Mat diff;
    cv::absdiff(img1, img2, diff);
    diff = diff.mul(diff);
    return cv::mean(diff)[0];
}

void pause() {
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}   

float vert[] = { 1, 1, 0, 1, -1, 0, -1, -1, 0, -1, 1, 0 };
float xAlfa = 60;
float zAlfa = 0;

struct Vec3
{
    float x, y, z;
};

Vec3 pos = { 0, 25, -25 };
Vec3 lightPos = { 0, 10, 35 };

float normal[] = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1 };

float verc_cube[] = {
    -7, 0, 8,  7, 0, 8,  7, 2.5, 8,  -7, 2.5, 8,
    -7, 0, 0,  -7, 0, 8,  7, 0, 8,   7, 0, 0,
    7, 0, 0,   7, 2.5, 0, 7, 2.5, 8,  7, 0, 8,
    -7, 0, 0,  -7, 0, 8,  -7, 2.5, 8, -7, 2.5, 0,
    -7, 2.5, 0, 7, 2.5, 0, 7, 2.5, 8, -7, 2.5, 8,
    -7, 0, 0,  7, 0, 0,  7, 2.5, 0,  -7, 2.5, 0
};

float cube_normals[] = {
    0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
    0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0,
    1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
    -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0,
    0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0
};

GLuint top1[] = { 0, 1, 2, 3 };
GLuint back0[] = { 4, 5, 6, 7 };
GLuint right0[] = { 8, 9, 10, 11 };
GLuint left0[] = { 12, 13, 14, 15 };
GLuint ahead0[] = { 16, 17, 18, 19 };
GLuint bottom[] = { 20, 21, 22, 23 };


float verc_trap[] = {
    -5.0, 0.0, 5.0,  5.0, 0.0, 5.0,  3.0, 0.0, -5.0,  -3.0, 0.0, -5.0,
    -5.0, 3.0, 5.0,  5.0, 3.0, 5.0,  3.0, 3.0, -5.0,  -3.0, 3.0, -5.0
};

float trap_normals[] = {
    0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0,
    0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
    0, 0.6f, 1, 0, 0.6f, 1, 0, 0.6f, 1, 0, 0.6f, 1,
    0, 0.6f, -1, 0, 0.6f, -1, 0, 0.6f, -1, 0, 0.6f, -1,
    -1, 0.6f, 0, -1, 0.6f, 0, -1, 0.6f, 0, -1, 0.6f, 0,
    1, 0.6f, 0, 1, 0.6f, 0, 1, 0.6f, 0, 1, 0.6f, 0
};

GLuint trap_bottom[] = { 0, 1, 2, 3 };
GLuint trap_top[] = { 4, 5, 6, 7 };
GLuint trap_front[] = { 0, 1, 5, 4 };
GLuint trap_back[] = { 2, 3, 7, 6 };
GLuint trap_left[] = { 0, 3, 7, 4 };
GLuint trap_right[] = { 1, 2, 6, 5 };

unsigned int texture;
float screen_vertex[] = { -1, -1, 0, 1, -1, 0, 1, 1, 0, -1, 1, 0 };
float textCoord[] = { 0, 1, 1, 1, 1, 0, 0, 0 };

void DrawTrapezoid()
{
    glVertexPointer(3, GL_FLOAT, 0, verc_trap);
    glNormalPointer(GL_FLOAT, 0, trap_normals);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, trap_bottom);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, trap_top);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, trap_front);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, trap_back);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, trap_left);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, trap_right);
}

void DrawCube()
{
    glVertexPointer(3, GL_FLOAT, 0, verc_cube);
    glNormalPointer(GL_FLOAT, 0, cube_normals);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, top1);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, back0);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, ahead0);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, right0);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, left0);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, bottom);
}

void DrawWinDoor(bool window) {
    float mainColorR = window ? 1.0f : 0.33f;
    float mainColorG = window ? 0.45f : 0.12f;
    float mainColorB = window ? 0.21f : 0.0f;

    float glassColorR = 0.13f;
    float glassColorG = window ? 0.73f : 0.1f;
    float glassColorB = 0.99f;

    glPushMatrix();
    glColor4f(mainColorR, mainColorG, mainColorB, 1.0f);
    glScalef(0.04f, 0.02f, 0.06f);
    glTranslatef(0, 1.4f, 4);
    DrawCube();
    glPopMatrix();

    for (int i = -1; i <= 1; i += 2) {
        glPushMatrix();
        glColor4f(glassColorR, glassColorG, glassColorB, 0.5f);
        glScalef(0.016f, 0.01f, 0.05f);
        glTranslatef(8 * i, 0, 5.6f);
        DrawCube();
        glPopMatrix();
    }
}

void drawLightSource(float x, float y, float z)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(1, 0.78f, 0);
    glBegin(GL_QUADS);
    glVertex3f(-0.2f, -0.2f, 0);
    glVertex3f(0.2f, -0.2f, 0);
    glVertex3f(0.2f, 0.2f, 0);
    glVertex3f(-0.2f, 0.2f, 0);
    glEnd();
    glPopMatrix();
}

void game_init()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.1f);

    GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    int width, height, cnt;
    unsigned char* data = stbi_load("av.jpg", &width, &height, &cnt, 0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

void show_screen()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glColor3f(1, 1, 1);
    glPushMatrix();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, screen_vertex);
    glTexCoordPointer(2, GL_FLOAT, 0, textCoord);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void move_light()
{
    if (GetKeyState('U') < 0) lightPos.z += 0.1f;
    if (GetKeyState('O') < 0) lightPos.z -= 0.1f;
    if (GetKeyState('J') < 0) lightPos.x -= 0.1f;
    if (GetKeyState('L') < 0) lightPos.x += 0.1f;
    if (GetKeyState('I') < 0) lightPos.y += 0.1f;
    if (GetKeyState('K') < 0) lightPos.y -= 0.1f;
}

void show_world()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glPushMatrix();
    float lightPosArr[] = { lightPos.x, lightPos.y, lightPos.z, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosArr);
    drawLightSource(lightPos.x, lightPos.y, lightPos.z);
    glPopMatrix();

    glVertexPointer(3, GL_FLOAT, 0, vert);
    glNormalPointer(GL_FLOAT, 0, normal);

    glPushMatrix();
    glTranslatef(0, 20, 0);
    for (int i = -20; i < 20; i++)
    {
        for (int j = -20; j < 20; j++)
        {
            glPushMatrix();
            glColor3f(0.29f, 0.53f, 0.29f);
            glTranslatef(i * 2, j * 2, 0);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glPopMatrix();
        }
    }
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.95f, 0.9f, 0.66f, 1);
    glScalef(2, 3, 2);
    glTranslatef(0, 7, 0);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.95f, 0.9f, 0.66f, 1);
    glScalef(3.8f, 1.5f, 2);
    glRotatef(90, 0, 0, 1);
    glTranslatef(17, 3.67f, 0);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.95f, 0.9f, 0.66f, 1);
    glScalef(3.8f, 1.5, 2);
    glRotatef(90, 0, 0, 1);
    glTranslatef(17, -6.17f, 0);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.95f, 0.9f, 0.66f, 1);
    glScalef(0.5f, 2.35f, 0.58f);
    glTranslatef(1.8f, 6.8f, 0);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.9f, 10.0, 0.0);
    glColor4f(0.95f, 0.9f, 0.66f, 1);
    glScalef(1.6f, 1.2f, 1.55f);
    glRotatef(90, 1, 0, 0);
    DrawTrapezoid();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.33f, 0.12f, 0, 1);
    glScalef(0.7f, 0.1f, 0.03f);
    glTranslatef(1, 23, 0);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.33f, 0.12f, 0, 1);
    glScalef(0.7f, 0.1f, 0.06f);
    glTranslatef(1, 25.5f, 0);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.33f, 0.12f, 0, 1);
    glScalef(0.7f, 0.1f, 0.09f);
    glTranslatef(1, 28, 0);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.33f, 0.12f, 0, 1);
    glScalef(1.1445f, 0.5f, 0.09f);
    glTranslatef(0.79f, 5.5f, 0);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-9.85f, 9.3f, -1.75f);
    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            glPushMatrix();
            glScalef(3.8f, 3.8f, 3.8f);
            glTranslatef(i * 0.8f - 0.6f, 3, j * 0.9f + 2.0f);
            DrawWinDoor(true);
            glPopMatrix();
        }
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(18, 3.3f, -8);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            glPushMatrix();
            glScalef(3.8f, 3.8f, 3.8f);
            glTranslatef(i * 0.77f - 0.6f, 3, j * 0.85f + 2.0f);
            DrawWinDoor(true);
            glPopMatrix();
        }
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-19.5f, 3.3f, -8);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            glPushMatrix();
            glScalef(3.8f, 3.8f, 3.8f);
            glTranslatef(i * 0.77f - 0.6f, 3, j * 0.85f + 2.0f);
            DrawWinDoor(true);
            glPopMatrix();
        }
    }
    glPopMatrix();

    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(-9.85f, -40.25f, -1.75f);
    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            glPushMatrix();
            glScalef(3.8f, 3.8f, 3.8f);
            glTranslatef(i * 0.8f - 0.6f, 3, j * 0.9f + 2.0f);
            DrawWinDoor(true);
            glPopMatrix();
        }
    }
    glPopMatrix();

    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(18, -47.7f, -8);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            glPushMatrix();
            glScalef(3.8f, 3.8f, 3.8f);
            glTranslatef(i * 0.77f - 0.6f, 3, j * 0.85f + 2.0f);
            DrawWinDoor(true);
            glPopMatrix();
        }
    }
    glPopMatrix();


    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(-19.5f, -47.7f, -8);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            glPushMatrix();
            glScalef(3.8f, 3.8f, 3.8f);
            glTranslatef(i * 0.77f - 0.6f, 3, j * 0.85f + 2.0f);
            DrawWinDoor(true);
            glPopMatrix();
        }
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-9.85f, 9.3f, -8.1f);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            glPushMatrix();
            glScalef(3.8f, 3.8f, 3.8f);
            glTranslatef(i * 0.8f - 0.6f, 3, j * 0.85f + 2.0f);
            DrawWinDoor(true);
            glPopMatrix();
        }
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(8.4f, 9.3f, -8.1f);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            glPushMatrix();
            glScalef(3.8f, 3.8f, 3.8f);
            glTranslatef(i * 0.8f - 0.6f, 3, j * 0.85f + 2.0f);
            DrawWinDoor(true);
            glPopMatrix();
        }
    }
    glPopMatrix();

    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(-9.85f, -40.25f, -8.1f);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            glPushMatrix();
            glScalef(3.8f, 3.8f, 3.8f);
            glTranslatef(i * 0.8f - 0.6f, 3, j * 0.85f + 2.0f);
            DrawWinDoor(true);
            glPopMatrix();
        }
    }
    glPopMatrix();

    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(8.4f, -40.25f, -8.1f);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            glPushMatrix();
            glScalef(3.8f, 3.8f, 3.8f);
            glTranslatef(i * 0.8f - 0.6f, 3, j * 0.85f + 2.0f);
            DrawWinDoor(true);
            glPopMatrix();
        }
    }
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.95f, 0.9f, 0.66f, 1);
    glTranslatef(0, 20.3f, 6);
    glScalef(2.2f, 3.5f, 0.05f);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.95f, 0.9f, 0.66f, 1);
    glTranslatef(24.2f, 25.5f, 6);
    glScalef(4.4f, 1.6f, 0.05f);
    glRotatef(90, 0, 0, 1);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.95f, 0.9f, 0.66f, 1);
    glTranslatef(-13.3f, 25.5f, 6);
    glScalef(4.4f, 1.6f, 0.05f);
    glRotatef(90, 0, 0, 1);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.95f, 0.9f, 0.66f, 1);
    glTranslatef(0, 20.3f, 15.999f);
    glScalef(2.2f, 3.5f, 0.05f);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.95f, 0.9f, 0.66f, 1);
    glTranslatef(24.2f, 25.5f, 16);
    glScalef(4.4f, 1.6f, 0.05f);
    glRotatef(90, 0, 0, 1);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.95f, 0.9f, 0.66f, 1);
    glTranslatef(-13.3f, 25.5f, 16);
    glScalef(4.4f, 1.6f, 0.05f);
    glRotatef(90, 0, 0, 1);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.33f, 0.12f, 0, 1);
    glTranslatef(-4, 35.5f, 0);
    glScalef(0.035f, 0.2f, 0.58f);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.33f, 0.12f, 0, 1);
    glTranslatef(3.5f, 35.5f, 0);
    glScalef(0.035f, 0.2f, 0.6f);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.33f, 0.12f, 0, 1);
    glTranslatef(3.95f, 32.5f, 4.5f);
    glScalef(3.4f, 0.6f, 0.05f);
    glRotatef(90, 0, 0, 1);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glRotatef(180, 0, 0, 1);
    glTranslatef(0.85f, -37.1f, -15.1f);
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 1; ++j)
        {
            glPushMatrix();
            glScalef(3.8f, 2.8f, 6.8f);
            glTranslatef(i * 0.8f - 0.6f, 3, j * 0.85f + 2.0f);
            DrawWinDoor(false);
            glPopMatrix();
        }
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.9f, -4.6f, -4.2f);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 1; ++j)
        {
            glPushMatrix();
            glScalef(3.2f, 2.8f, 2.2f);
            glTranslatef(i * 0.57f - 0.6f, 3, j * 0.85f + 2.0f);
            DrawWinDoor(false);
            glPopMatrix();
        }
    }
    glPopMatrix();

    glPushMatrix();
    glColor4f(0, 0, 0, 1);
    glScalef(1.1f, 0.04f, 0.28f);
    glTranslatef(0.83f, 97.5f, 7.3f);
    DrawCube();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.9f, 3.86f, 3.2f);
    glScalef(7.5f, 1, 1);
    glRotatef(90, 1, 0, 0);
    show_screen();
    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

void move_camera()
{
    if (GetKeyState(VK_UP) < 0) xAlfa = ++xAlfa > 180 ? 180 : xAlfa;
    if (GetKeyState(VK_DOWN) < 0) xAlfa = --xAlfa < 0 ? 0 : xAlfa;
    if (GetKeyState(VK_LEFT) < 0) zAlfa++;
    if (GetKeyState(VK_RIGHT) < 0) zAlfa--;

    float ugol = -zAlfa / 180 * 3.14;
    float speed = 0;
    if (GetKeyState('S') < 0) speed = 0.1f;
    if (GetKeyState('W') < 0) speed = -0.1f;
    if (GetKeyState('D') < 0)
    {
        speed = 0.1f;
        ugol -= 3.14f * 0.5f;
    }
    if (GetKeyState('A') < 0)
    {
        speed = 0.1f;
        ugol += 3.14f * 0.5f;
    }

    if (GetKeyState(VK_SPACE) < 0) pos.z -= 0.1f;
    if (GetKeyState(VK_CAPITAL) < 0) pos.z += 0.1f;
    if (pos.z >= 0) pos.z = -0.5f;

    if (speed != 0)
    {
        pos.x += sin(ugol) * speed;
        pos.y += cos(ugol) * speed;
    }

    glRotatef(-xAlfa, 1, 0, 0);
    glRotatef(-zAlfa, 0, 0, 1);
    glTranslatef(pos.x, pos.y, pos.z);
}

int main(void)
{
    GLFWwindow* window;
    if (!glfwInit()) return -1;

    float sc_w = 2048.0f;
    float sc_h = 1152.0f;
    window = glfwCreateWindow(sc_w, sc_h, "52 bratuha", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = sc_w / sc_h;
    glFrustum(-0.1f * aspect, 0.1f * aspect, -0.1f, 0.1f, 0.2f, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    game_init();

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0, 0.68f, 0.77f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPushMatrix();
        glRotatef(-xAlfa, 1, 0, 0);
        glRotatef(-zAlfa, 0, 0, 1);
        glTranslatef(pos.x, pos.y, pos.z);
        //move_camera();
        move_light();
        show_world();
        glPopMatrix();


        glfwSwapBuffers(window);
       
        static bool spaceReleased = true;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            if (spaceReleased) {
                spaceReleased = false;
                SaveImage("image.png", window);
                addNoise("image.png", "noisy_image.png"); // Вызов функции добавления шума и сохранение изображения с шумом

                cv::Mat noisyImage = cv::imread("noisy_image.png", cv::IMREAD_COLOR); // Открыть сохранённое изображение с шумом
                if (noisyImage.empty()) {
                    std::cerr << "Could not open or find the image!\n";
                    return -1;
                }

                cv::Mat filteredImage = applyMedianFilterManual(noisyImage, 3); // Применение собственного медианного фильтра
                cv::imwrite("manual_filtered_image.png", filteredImage); // Сохранить изображение с собственным медианным фильтром

                cv::Mat filteredImage2;
                cv::medianBlur(noisyImage, filteredImage2, 3); // Применение медианного фильтра OpenCV
                cv::imwrite("opencv_filtered_image.png", filteredImage2); // Сохранить изображение с медианным фильтром OpenCV

                // Оценим качество применённого фильтра
                cv::Mat original = cv::imread("image.png", cv::IMREAD_COLOR);
                cv::Mat noisy = cv::imread("noisy_image.png", cv::IMREAD_COLOR);
                cv::Mat manualFiltered = cv::imread("manual_filtered_image.png", cv::IMREAD_COLOR);
                cv::Mat opencvFiltered = cv::imread("opencv_filtered_image.png", cv::IMREAD_COLOR);

                // Сравнение шумного изображения с оригиналом
                std::cout << "Noisy vs Original:\n";
                std::cout << "MSE: " << calculateMSE(original, noisy) << "\n";

                // Сравнение ручной фильтрации с оригиналом
                std::cout << "Manual Filtered vs Original:\n";
                std::cout << "MSE: " << calculateMSE(original, manualFiltered) << "\n";

                // Сравнение OpenCV фильтрации с оригиналом
                std::cout << "OpenCV Filtered vs Original:\n";
                std::cout << "MSE: " << calculateMSE(original, opencvFiltered) << "\n";
            }
        }
        else {
            spaceReleased = true;
        }

        
        Sleep(1);

        /* Poll for and process events */
        glfwPollEvents();

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}