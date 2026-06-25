#include "StatusBar.hpp"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

ftxui::Component make_status_bar(App* app) {
    return Renderer([app]() {
        // Left: transient message or current file path
        std::string left = app->active_status();
        if (left.empty() && !app->current_file.empty())
            left = app->current_file.string();

        // Center: context hints
        std::string hints;
        if (app->focus == AppFocus::Browser)
            hints = "F1=Help  F2=Save  F5=Batch  F6=Online  F10=Quit";
        else
            hints = "F1=Help  F2=Save  F6=Online  ESC=Cancel  Tab=Switch";

        // Right: audio info
        std::string right = app->loaded_tag.audio_info();

        return hbox({
            text(left)   | flex | color(Color::White),
            text(hints)  | center | color(Color::GrayLight),
            text(right)  | color(Color::Cyan),
        }) | size(HEIGHT, EQUAL, 1);
    });
}
