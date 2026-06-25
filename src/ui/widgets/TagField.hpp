#pragma once
#include <string>
#include <functional>
#include <ftxui/component/component.hpp>

// A single labelled input row: "Label  : [ value ]"
// label_width controls the fixed column for the label.
// digits_only: if true, rejects non-digit characters.
ftxui::Component make_tag_field(
    const std::string& label,
    std::string*       value,
    bool               digits_only  = false,
    int                label_width  = 10);
