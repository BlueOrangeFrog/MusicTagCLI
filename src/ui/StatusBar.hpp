#pragma once
#include <ftxui/component/component.hpp>
#include "app/App.hpp"

// The bottom status bar: left = transient message or current file path,
// right = audio properties; context-sensitive key hints in the center.
ftxui::Component make_status_bar(App* app);
