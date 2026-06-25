#pragma once
#include <functional>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "app/App.hpp"

// Assembles the top-level FTXUI component tree:
//   MenuBar
//   hbox[ FileBrowser | separator | TagEditor ]
//   StatusBar
// All modal dialogs are layered on top via ftxui::Modal().
ftxui::Component make_main_layout(App* app,
                                   ftxui::ScreenInteractive& screen);
