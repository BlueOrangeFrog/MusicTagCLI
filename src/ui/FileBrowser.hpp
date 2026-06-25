#pragma once
#include <functional>
#include <ftxui/component/component.hpp>
#include "app/App.hpp"

// Left panel — file system browser.
// on_file_selected: called (on the main thread) when an audio file gains focus.
ftxui::Component make_file_browser(
    App*                                        app,
    std::function<void(const FileEntry&)>       on_file_selected,
    std::function<void()>                       request_quit);
