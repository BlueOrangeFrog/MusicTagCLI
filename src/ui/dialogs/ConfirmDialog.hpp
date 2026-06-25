#pragma once
#include <string>
#include <functional>
#include <ftxui/component/component.hpp>

// Creates a modal Yes/No confirmation dialog component.
// show:    pointer to the bool that controls visibility (set to false on either button)
// message: text displayed in the dialog
// on_yes:  called when the user confirms
// on_no:   called when the user cancels (optional)
ftxui::Component make_confirm_dialog(
    bool*                  show,
    std::string            message,
    std::function<void()>  on_yes,
    std::function<void()>  on_no = {});
