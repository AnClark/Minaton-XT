#include "MinatonPresets.hpp"
#include "MinatonUI.h"

enum MinatonMenuId {
    MENU_ABOUT = 1200,
    MENU_FIRST_PRESET
};

void MinatonUI::initRightClickMenu()
{
    fRightClickMenu = new MenuWidget(this);

    // Preset menu
    fRightClickMenu->addSection("Presets");
    for (uint32_t i = 0; i < fPresetManager->getEmbedPresetCount(); i++) {
        fRightClickMenu->addItem(MENU_FIRST_PRESET + i, fPresetManager->getEmbedPresetById(i).name.c_str());
    }

    fRightClickMenu->setCallback(this);
}

void MinatonUI::menuItemSelected(const int id)
{
    // Apply preset patch
    const int patchId = id - MENU_FIRST_PRESET;
    fPresetManager->loadPatchById(patchId);

    // Mark currently selected preset
    fRightClickMenu->setItemSelected(id);
}

/**
 * To process menu event, you need to override onMouse() callback.
 * Menu event processing always has the highest priority.
 */
bool MinatonUI::onMouse(const MouseEvent& ev)
{
    switch (ev.button) {
    case kMouseButtonLeft: {
        //
        // First, if menu is active, handle menu click event.
        //
        if (fRightClickMenu->isVisible()) {
            // The 2nd param is "offset". Set to (0, 0) so that I can open menu within window everywhere
            const auto window_pos = Point<int>(0, 0);
            fRightClickMenu->mouseEvent(ev, window_pos);

            // Remember to repaint, otherwise the menu will stuck on screen!
            repaint();

            return true;
        }
        //
        // Second, if no menu active, handle default UI widget controls.
        //
        else {
            return UI::onMouse(ev);
        }
    }
    case kMouseButtonRight: {
        //
        // Show popup menu on right-click
        //
        if (ev.press) {
            // Get click position and the bounds of the window
            // Set my_pos_absolute to 0, so the menu can be shown within the bound of the window.
            const auto my_pos_absolute = Point<int>(
                0,
                0);
            const auto widget_bounds = Rectangle<int>(
                0, 0,
                getWidth(), getHeight());
            fRightClickMenu->show(my_pos_absolute, ev.pos, widget_bounds);

            // Remember to repaint, otherwise when you right-click multiple times, menus will stuck on screen!
            // [This issue occured on Win32 (Wine).]
            repaint();
        }

        return true;
    }
    case kMouseButtonMiddle:
        //
        // Close any popup menu on middle-button click, then handle default UI event.
        //
        fRightClickMenu->hide();

        // Remember to repaint, otherwise the menu will stuck on screen!
        repaint();

        return UI::onMouse(ev);
    }

    return false;
}

/**
 * onMotion() handles mouse movement events (e.g. hovering).
 * To process menu event, you need to override onMouse() callback.
 * Menu on-hover highlighting is controlled by onMotion().
 */
bool MinatonUI::onMotion(const MotionEvent& ev)
{
    //
    // First, handle menu moue motion (hovering) events.
    //
    const Point<int> window_pos(0, 0);

    if (fRightClickMenu->motionEvent(ev, window_pos)) {
        repaint(); // Remember to repaint, otherwise hovered menu item won't be highlighted!
        return true;
    }

    //
    // Second, handle default UI widget controls.
    //
    return UI::onMotion(ev);
}
