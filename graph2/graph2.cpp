#include <GLFW/glfw3.h>
#include <Windows.h>
#include <math.h>
#include <vector>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void SaveImage(const char* filepath, GLFWwindow* w) {
    int width, height;

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


float vert[] = { 1, 1, 0, 1, -1, 0, -1, -1, 0, -1, 1, 0 };
float xAlfa = 1;
float zAlfa = 0;

struct Vec3 {
    float x, y, z;
};

Vec3 pos = { 0, 0, 0 };
Vec3 lightPos = { 0, 0, 35 };

float normal[] = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1 };

float verc_cube[] = {
    -7, 0, 8, 7, 0, 8, 7, 2.5, 8, -7, 2.5, 8,
    -7, 0, 0, -7, 0, 8, 7, 0, 8, 7, 0, 0,
    7, 0, 0, 7, 2.5, 0, 7, 2.5, 8, 7, 0, 8,
    -7, 0, 0, -7, 0, 8, -7, 2.5, 8, -7, 2.5, 0,
    -7, 2.5, 0, 7, 2.5, 0, 7, 2.5, 8, -7, 2.5, 8,
};

GLuint top1[] = { 0, 1, 2, 3 };
GLuint back0[] = { 4, 5, 6, 7 };
GLuint right0[] = { 8, 9, 10, 11 };
GLuint left0[] = { 12, 13, 14, 15 };
GLuint ahead0[] = { 16, 17, 18, 19 };


float verc_trap[] = {
    -5.0, 0.0, 5.0,
     5.0, 0.0, 5.0,
     3.0, 0.0, -5.0,
    -3.0, 0.0, -5.0,

    -5.0, 3.0, 5.0,
     5.0, 3.0, 5.0,
     3.0, 3.0, -5.0,
    -3.0, 3.0, -5.0
};

GLuint trap_bottom[] = {0, 1, 2, 3};
GLuint trap_top[] = {4, 5, 6, 7};
GLuint trap_front[] = {0, 1, 5, 4};
GLuint trap_back[] = {2, 3, 7, 6};
GLuint trap_left[] = {0, 3, 7, 4};
GLuint trap_right[] = {1, 2, 6, 5};

void DrawTrapezoid() {
    glVertexPointer(3, GL_FLOAT, 0, verc_trap);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, trap_bottom);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, trap_top);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, trap_front);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, trap_back);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, trap_left);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, trap_right);
}


void DrawСube() {
    glVertexPointer(3, GL_FLOAT, 0, verc_cube);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, top1);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, back0);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, ahead0);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, right0);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, left0);
}

void DrawWindows()
{
    glPushMatrix();
    glColor4f(1, 0.45, 0.21, 1);
    glScalef(0.04, 0.01, 0.06);
    glTranslatef(0, 1.4, 4);
    DrawСube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.13, 0.73, 0.99, 0.5);
    glScalef(0.03, 0.01, 0.05);
    glTranslatef(0, 0, 5.6);
    DrawСube();
    glPopMatrix();
}

void drawLightSource(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(1, 0.78, 0);
    glBegin(GL_QUADS);
    glVertex3f(-0.2, -0.2, 0);
    glVertex3f(0.2, -0.2, 0);
    glVertex3f(0.2, 0.2, 0);
    glVertex3f(-0.2, 0.2, 0);
    glEnd();
    glPopMatrix();
}

void game_init() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.1);

    float diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    float specular[] = { 1.0, 1.0, 1.0, 1.0 };
    float ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
}

void move_light() {
    if (GetKeyState('U') < 0) lightPos.z += 0.1;
    if (GetKeyState('O') < 0) lightPos.z -= 0.1;
    if (GetKeyState('J') < 0) lightPos.x -= 0.1;
    if (GetKeyState('L') < 0) lightPos.x += 0.1;
    if (GetKeyState('I') < 0) lightPos.y += 0.1;
    if (GetKeyState('K') < 0) lightPos.y -= 0.1;
}

void show_world() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glPushMatrix();
    float lightPosArr[] = { lightPos.x, lightPos.y, lightPos.z, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosArr);
    drawLightSource(lightPos.x, lightPos.y, lightPos.z);
    glPopMatrix();

    glVertexPointer(3, GL_FLOAT, 0, vert);
    glNormalPointer(GL_FLOAT, 0, normal);

    for (int i = -20; i < 20; i++) {
        for (int j = -20; j < 20; j++) {
            glPushMatrix();
            if ((i + j) % 2 == 0) glColor3f(0.09, 0.09, 0.09);
            else glColor3f(0.2, 0.2, 0.2);

            glTranslatef(i * 2, j * 2, 0);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glPopMatrix();
        }
    }

    glPushMatrix();
    glColor4f(1, 0.77, 0.51, 1);
    glScalef(2, 3, 2);
    glTranslatef(0, 7, 0);
    DrawСube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(1, 0.77, 0.51, 1);
    glScalef(3.8, 1.5, 2);
    glRotatef(90, 0, 0, 1);
    glTranslatef(17, 3.67, 0);
    DrawСube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(1, 0.77, 0.51, 1);
    glScalef(3.8, 1.5, 2);
    glRotatef(90, 0, 0, 1);
    glTranslatef(17, -6.17, 0);
    DrawСube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(1, 0.77, 0.51, 1);
    glScalef(0.5, 2.35, 0.58);
    glTranslatef(1.8, 6.8, 0);
    DrawСube();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.9, 10.0, 0.0);
    glColor4f(1, 0.77, 0.51, 1);
    glScalef(1.6, 1.2, 1.55);
    glRotatef(90, 1, 0, 0);
    DrawTrapezoid();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.33, 0.12, 0, 1);
    glScalef(0.7, 0.1, 0.03);
    glTranslatef(1, 13+19.5, 0);
    DrawСube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.33, 0.12, 0, 1);
    glScalef(0.7, 0.1, 0.06);
    glTranslatef(1, 15.5+19.5, 0);
    DrawСube();
    glPopMatrix();

    glPushMatrix();
    glColor4f(0.33, 0.12, 0, 1);
    glScalef(0.7, 0.1, 0.09);
    glTranslatef(1, 18+19.5, 0);
    DrawСube();
    glPopMatrix();

    glPushMatrix();
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            glTranslatef(i * 0.1 - 0.05, 1.4, j * 0.1 + 4);
            glTranslatef(i * 0.1 - 0.05, 0, j * 0.1 + 5.6);
            DrawWindows();
        }
    }

    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

void move_camera() {
    if (GetKeyState(VK_UP) < 0) xAlfa = ++xAlfa > 180 ? 180 : xAlfa;
    if (GetKeyState(VK_DOWN) < 0) xAlfa = --xAlfa < 0 ? 0 : xAlfa;
    if (GetKeyState(VK_LEFT) < 0) zAlfa++;
    if (GetKeyState(VK_RIGHT) < 0) zAlfa--;

    float ugol = -zAlfa / 180 * 3.14;
    float speed = 0;
    if (GetKeyState('S') < 0) speed = 0.1;
    if (GetKeyState('W') < 0) speed = -0.1;
    if (GetKeyState('D') < 0) { speed = 0.1; ugol -= 3.14 * 0.5; }
    if (GetKeyState('A') < 0) { speed = 0.1; ugol += 3.14 * 0.5; }

    if (GetKeyState(VK_SPACE) < 0) pos.z -= 0.1;
    if (GetKeyState(VK_CAPITAL) < 0) pos.z += 0.1;
    if (pos.z >= 0) pos.z = -0.5;

    if (speed != 0) {
        pos.x += sin(ugol) * speed;
        pos.y += cos(ugol) * speed;
    }

    glRotatef(-xAlfa, 1, 0, 0);
    glRotatef(-zAlfa, 0, 0, 1);
    glTranslatef(pos.x, pos.y, pos.z);
}

int main(void) {
    GLFWwindow* window;
    if (!glfwInit()) return -1;

    window = glfwCreateWindow(1200, 1200, "52 bratuha", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.1, 0.1, -0.1, 0.1, 0.2, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    game_init();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPushMatrix();
        move_camera();
        move_light();
        show_world();
        glPopMatrix();

        //SaveImage("image.png", window);

        glfwSwapBuffers(window);
        Sleep(1);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
