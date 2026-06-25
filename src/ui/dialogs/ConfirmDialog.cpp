#include "ConfirmDialog.hpp"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

ftxui::Component make_confirm_dialog(
    bool*                  show,
    std::string            message,
    std::function<void()>  on_yes,
    std::function<void()>  on_no)
{
    auto btn_yes = Button("  Yes  ", [show, on_yes]() {
        *show = false;
        if (on_yes) on_yes();
    });
    auto btn_no = Button("  No   ", [show, on_no]() {
        *show = false;
        if (on_no) on_no();
    });

    auto buttons = Container::Horizontal({btn_yes, btn_no});

    // Capture btn_yes/btn_no explicitly — they are not captured via `buttons`.
    return Renderer(buttons, [message, buttons, btn_yes, btn_no]() {
        return vbox(Elements{
            text(message) | bold | center,
            separator(),
            hbox(btn_yes->Render() | flex, btn_no->Render() | flex),
        }) | border | size(WIDTH, GREATER_THAN, 40);
    });
}
