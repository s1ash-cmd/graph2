#include <GLFW/glfw3.h>
#include <Windows.h>
#include <math.h>
#include <stdio.h>

struct Vec3 {
    float x, y, z;
};

Vec3 pos = { 0, 0, 0 };

float vert[] = { 1, 1, 0, 1, -1, 0, -1, -1, 0, -1, 1, 0 };
float xAlfa = 1;
float zAlfa = 0;

float theta = 0;

void draw_parallelepiped(float width, float depth, float height, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);

    glBegin(GL_QUADS);

    glVertex3f(0, 0, height);
    glVertex3f(width, 0, height);
    glVertex3f(width, depth, height);
    glVertex3f(0, depth, height);

    glVertex3f(0, 0, 0);
    glVertex3f(0, depth, 0);
    glVertex3f(width, depth, 0);
    glVertex3f(width, 0, 0);

    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, height);
    glVertex3f(0, depth, height);
    glVertex3f(0, depth, 0);

    glVertex3f(width, 0, 0);
    glVertex3f(width, depth, 0);
    glVertex3f(width, depth, height);
    glVertex3f(width, 0, height);

    glVertex3f(0, depth, 0);
    glVertex3f(0, depth, height);
    glVertex3f(width, depth, height);
    glVertex3f(width, depth, 0);

    glVertex3f(0, 0, 0);
    glVertex3f(width, 0, 0);
    glVertex3f(width, 0, height);
    glVertex3f(0, 0, height);

    glEnd();
}


void game_init() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.1);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);

    float light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    float light_position[] = { 1.0, 1.0, 3.0, 0.0 };
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
}


void show_world() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vert);

    float normal_grid[] = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1 };
    glNormalPointer(GL_FLOAT, 0, normal_grid);

    for (int i = -10; i < 10; i++) {
        for (int j = -10; j < 10; j++) {
            glPushMatrix();
            glColor3f(0.5, 0.5, 0.5);
            glTranslatef(i * 2, j * 2, 0);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glPopMatrix();
        }
    }

    glPushMatrix();
    glTranslatef(-5, 0, 0);
    draw_parallelepiped(10, 2, 4, 0.89, 0.867, 0.725, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5, -2.7, 0);
    glRotatef(90, 0, 0, 1);
    draw_parallelepiped(7, 2.25, 4, 0.89, 0.867, 0.725, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-5, -2.7, 0);
    glRotatef(90, 0, 0, 1);
    draw_parallelepiped(7, 2.25, 4, 0.89, 0.867, 0.725, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -2.7, 0);
    glRotatef(90, 0, 0, 1);
    draw_parallelepiped(3, 2.25, 2, 0.91, 0.678, 0.431, 1);
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

    if (!glfwInit()) {
        return -1;
    }

    window = glfwCreateWindow(1600, 900, "52 bratuha", NULL, NULL);
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

    game_init();

    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPushMatrix();
        move_camera();
        show_world();
        glPopMatrix();

        glfwSwapBuffers(window);
        theta += 1;
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}