#pragma once
#include <functional>
#include <ftxui/component/component.hpp>
#include "app/App.hpp"

// Creates the batch-edit dialog component.
// The dialog shows all tag fields with checkboxes; only checked fields are
// written to the selected files when the user confirms.
ftxui::Component make_batch_dialog(App* app);
