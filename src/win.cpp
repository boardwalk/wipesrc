#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "libgte.h"
#include "libgpu.h"
#include "input.h"
#include "object.h"
#include "track.h"
#include "ships.h"
#include "pad.h"
#include "renderer.h"
#include <stdio.h>
#include <GLFW/glfw3.h>

int16_t ft = 0;
char MipMap = 0;
char* SaveDirBase;
char* DataDirBase;
char* MovieDirBase;
char globalPilot[255] = {0};
int16_t FrameState = 0;
int32_t menuTexturesIn = 0;
int32_t numoff = 0;
int32_t numsmall = 0;
int16_t screenres = 3;
int16_t trackNum = 0;
float upres = 2.0f;

static GLFWwindow* g_window = nullptr;

char* LockPilot() {
  return NULL;
}

void UnlockPilot() {
}

bool JJSJoyConfig() {
  return false;
}

void changeScreen() {
  printf("changeScreen\n");
}

void setRenderBegin() {
}

void setRenderEnd() {
}

bool SJRNewTexture(const char* filename) {
  printf("SJRNewTexture(%s)\n", filename);
  return false;
}

void centreMouse() {
}

int32_t JJSLoadSprites() {
  printf("JJSLoadSprites()\n");
  return 0;
}

int32_t LockBackBuffer() {
  return 0;
}

void UnlockBackBuffer() {
}

void WinClearScreen(char colour) {
}

void BltClearToScreen() {
}

void DDSwap() {
  rendererRender();
  glfwSwapBuffers(g_window);
}

int32_t ProcessMessages() {
  glfwPollEvents();
  return 0;
}

TimData* JJSLoad16BitTexture(const char* filename, char location) {
  printf("JJSLoad16BitTexture(%s, %d)\n", filename, location);
  return NULL;
}

int16_t randlist[2000];
int16_t* randnum;
int16_t ctr;

void initrandy() {
  int16_t i;
  time_t t;

  t = time(NULL);

  srand(t);

  for (i = 0; i < 2000; i++)
    randlist[i] = rand() & 0x7FFF;

  randnum = &randlist[0];
  ;
  ctr = 0;
}

int32_t randy() {
  int16_t num;

  num = *randnum++;

  ctr++;
  if (ctr == 2000) {
    randnum = &randlist[0];
    ;
    ctr = 0;
  }

  return num;
}

void WinQuit() {
}

void sendList() {
}

void oldmain();

extern int16_t WinWidth;
extern int16_t WinHeight;
extern int16_t WinHeightX2;

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  uint8_t scan_code;
  switch (key) {
  case GLFW_KEY_ESCAPE:
    scan_code = _ESC_KEY;
    break;
  case GLFW_KEY_TAB:
    scan_code = _TAB_KEY;
    break;
  case GLFW_KEY_P:
    scan_code = _P_KEY;
    break;
  case GLFW_KEY_ENTER:
    scan_code = _ENTER_KEY;
    break;
  case GLFW_KEY_LEFT_CONTROL:
    scan_code = _LEFT_CTRL_KEY;
    break;
  case GLFW_KEY_LEFT_SHIFT:
    scan_code = _LEFT_SHIFT_KEY;
    break;
  case GLFW_KEY_UP:
    scan_code = _UP_KEY;
    break;
  case GLFW_KEY_LEFT:
    scan_code = _LEFT_KEY;
    break;
  case GLFW_KEY_RIGHT:
    scan_code = _RIGHT_KEY;
    break;
  case GLFW_KEY_DOWN:
    scan_code = _DOWN_KEY;
    break;
  case GLFW_KEY_Z:
    scan_code = _Z_KEY;
    break;
  case GLFW_KEY_X:
    scan_code = _X_KEY;
    break;
  case GLFW_KEY_F1:
    scan_code = _F1_KEY;
    break;
  case GLFW_KEY_F2:
    scan_code = _F2_KEY;
    break;
  case GLFW_KEY_F3:
    scan_code = _F3_KEY;
    break;
  case GLFW_KEY_F4:
    scan_code = _F4_KEY;
    break;
  case GLFW_KEY_F5:
    scan_code = _F5_KEY;
    break;
  case GLFW_KEY_F6:
    scan_code = _F6_KEY;
    break;
  case GLFW_KEY_F7:
    scan_code = _F7_KEY;
    break;
  case GLFW_KEY_F8:
    scan_code = _F8_KEY;
    break;
  case GLFW_KEY_F9:
    scan_code = _F9_KEY;
    break;
  case GLFW_KEY_F10:
    scan_code = _F10_KEY;
    break;
  case GLFW_KEY_F11:
    scan_code = _F11_KEY;
    break;
  case GLFW_KEY_F12:
    scan_code = _F12_KEY;
    break;
  case GLFW_KEY_LEFT_ALT:
    scan_code = _LEFT_ALT_KEY;
    break;
  default:
    return;
  }

  if (action == GLFW_PRESS) {
    pmkey(scan_code, 1);
  } else if (action == GLFW_RELEASE) {
    pmkey(scan_code, 0);
  }
}

int main(int argc, char** argv) {
  initrandy();

  WinWidth = 640;
  WinHeight = 480;
  WinHeightX2 = WinHeight * 2;

  SaveDirBase = (char*)malloc(120);
  strcpy(SaveDirBase, ".");

  DataDirBase = (char*)malloc(120);
  strcpy(DataDirBase, ".");

  MovieDirBase = (char*)malloc(120);
  strcpy(MovieDirBase, ".");

  chdir(DataDirBase);

  glfwSetErrorCallback([] (int error_code, const char* description) {
    fprintf(stderr, "GLFW error %d: %s\n", error_code, description);
  });

  if (!glfwInit()) {
    return EXIT_FAILURE;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  g_window = glfwCreateWindow(WinWidth, WinHeight, "Wipeout", nullptr, nullptr);
  if (g_window == nullptr) {
    return EXIT_FAILURE;
  }

  glfwSetKeyCallback(g_window, keyCallback);
  glfwMakeContextCurrent(g_window);

  rendererInit();
  oldmain();
  rendererTerm();
  glfwTerminate();
  return 0;
}
