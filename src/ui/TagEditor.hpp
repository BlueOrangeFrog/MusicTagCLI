#pragma once
#include <ftxui/component/component.hpp>
#include "app/App.hpp"

// Right panel — tag editor.
// Displays and edits the tag fields for the currently loaded file.
ftxui::Component make_tag_editor(App* app);
