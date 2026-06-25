#pragma once
#include <ftxui/component/component.hpp>
#include "app/App.hpp"

// Online metadata search dialog.
// Two modes: "By Tags" (uses current title/artist/album) and
// "By Fingerprint" (uses fpcalc + AcoustID — requires fpcalc in PATH).
// Results are fetched in a background thread; a spinner is shown while loading.
ftxui::Component make_online_dialog(App* app);
