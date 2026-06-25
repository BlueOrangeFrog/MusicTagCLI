#pragma once
#include <ftxui/component/component.hpp>
#include "app/App.hpp"

// Dialog to load or remove the cover art of the currently edited file.
// The user types a file path; we read the bytes and store them in
// app->edited_tag.cover_bytes / cover_mime.
ftxui::Component make_cover_dialog(App* app);
