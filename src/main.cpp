#include <filesystem>
#include <cstdlib>
#include <fstream>

#include <ftxui/component/screen_interactive.hpp>

#include "app/App.hpp"
#include "ui/MainLayout.hpp"

// Write default config file if it does not exist yet.
static void ensure_config() {
    const char* home = std::getenv("HOME");
    if (!home) return;

    std::filesystem::path cfg_dir = std::string(home) + "/.config/musictagcli";
    std::error_code ec;
    std::filesystem::create_directories(cfg_dir, ec);

    std::filesystem::path cfg_file = cfg_dir / "config.ini";
    if (!std::filesystem::exists(cfg_file, ec)) {
        std::ofstream f(cfg_file);
        f << "# MusicTagCLI configuration\n"
          << "# Register a free AcoustID API key at https://acoustid.org/api-key\n"
          << "# and paste it here to enable fingerprint-based metadata lookup.\n"
          << "acoustid_api_key=YOUR_KEY_HERE\n";
    }
}

int main(int argc, char* argv[]) {
    ensure_config();

    App app;

    // Determine starting directory
    std::filesystem::path start;
    if (argc >= 2) {
        start = argv[1];
    } else {
        const char* home = std::getenv("HOME");
        start = home ? std::filesystem::path(home) : std::filesystem::current_path();
    }
    app.load_directory(std::filesystem::canonical(start));

    auto screen = ftxui::ScreenInteractive::Fullscreen();
    auto layout = make_main_layout(&app, screen);

    screen.Loop(layout);
    return 0;
}
