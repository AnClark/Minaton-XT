# Wolf Widgets

This is Wolf Widgets, the widget toolkit of [Wolf Plugins](https://wolf-plugins.github.io). Originally it's part of [Wolf Shaper](https://github.com/wolf-plugins/wolf-shaper/), but it's suitable for other plugins as well.

This directory mainly includes Wolf menu widget, with AnClark's own improvements.

## Usage

Minaton-XT is the best reference for you. Source files with usages are described below.

### `plugin/MinatonUI.h`

The main definition of Minaton-XT UI. Follow this instruction to include Wolf menu widget's essential definitions to your own program.

Firstly, include header file, and declare namespace:

```c++
#include "MenuWidget.hpp"

using DISTRHO::MenuWidget;
```

Secondly, define essential items for Wolf menu widget, in your DPF UI definition.

For example:

```c++
START_NAMESPACE_DISTRHO

class MinatonUI : public UI,
                  public MenuWidget::Callback
{
private:
    // Menu instance. Remember to define it with DGL::ScopedPointer
    ScopedPointer<MenuWidget> fRightClickMenu;

public:

    // Callbacks of menu widget
    void menuItemSelected(const int id) override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;

    // Custom function for menu item initialization
    void initRightClickMenu();

    /* your other own definitions here... */
};

END_NAMESPACE_DISTRHO
```

### `plugin/MinatonUIMenu.cpp`

Tells you how to create menu and operate with it.

Read the file carefully, so you will know how to implement Wolf menu widget's callbacks.

## Add Wold Widgets to Your Build System

No matter what build system you use (CMake or GNU Make), make sure you:
- Add all the C++ files here to your source file list
- Add this directory to your include directory list

## License

GPL-3.0, conforming to [Wolf Shaper's license](https://github.com/wolf-plugins/wolf-shaper/blob/master/LICENSE). 
