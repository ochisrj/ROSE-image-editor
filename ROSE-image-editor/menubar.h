#pragma once

#include "filemenu.h"
#include "editmenu.h"
#include "Imagemenu.h"
#include "layermenu.h"
#include "selectmenu.h"
#include "filtermenu.h"
#include "viewmenu.h"
#include "windowmenu.h"
#include "helpmenu.h"

struct GLFWwindow;

class MenuBar
{
public:
    // Full menu bar pipeline: draw menus, draw menu-owned windows/dialogs,
    // process global shortcuts and dispatch pending commands.
    static void Draw(GLFWwindow* window);
    static void RenderMainMenuBar(GLFWwindow* window);
    static void RenderMenuWindows();
};