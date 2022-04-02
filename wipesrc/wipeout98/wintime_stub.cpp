#include <GLFW/glfw3.h>

int32_t		Curpad = 0;

void InitialiseWinTimer(void)
{}

void EndWinTimer(void)
{}

int32_t HowLong(void)
{
    return static_cast<int32_t>(glfwGetTime() * 1000.0);
}
