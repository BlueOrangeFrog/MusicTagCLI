#include "CoverArtDialog.hpp"
#include <ftxui/dom/elements.hpp>
#include <fstream>
#include <iterator>
#include <array>

using namespace ftxui;

// Detect MIME type from magic bytes
static std::string detect_mime(const std::vector<uint8_t>& bytes) {
    if (bytes.size() >= 8
        && bytes[0] == 0x89 && bytes[1] == 'P' && bytes[2] == 'N' && bytes[3] == 'G')
        return "image/png";
    if (bytes.size() >= 3
        && bytes[0] == 0xFF && bytes[1] == 0xD8 && bytes[2] == 0xFF)
        return "image/jpeg";
    return "application/octet-stream";
}

ftxui::Component make_cover_dialog(App* app) {
    auto path_str = std::make_shared<std::string>();
    auto error    = std::make_shared<std::string>();

    auto input = Input(path_str.get(), "/path/to/cover.jpg");

    auto btn_load = Button("Load", [app, path_str, error]() {
        error->clear();
        std::ifstream f(*path_str, std::ios::binary);
        if (!f) { *error = "Cannot open file."; return; }

        std::vector<uint8_t> bytes(
            std::istreambuf_iterator<char>(f),
            std::istreambuf_iterator<char>());

        if (bytes.empty()) { *error = "File is empty."; return; }

        std::string mime = detect_mime(bytes);
        if (mime == "application/octet-stream") {
            *error = "Unsupported format (expected JPEG or PNG).";
            return;
        }

        app->edited_tag.cover_bytes = std::move(bytes);
        app->edited_tag.cover_mime  = mime;
        app->dirty = (app->edited_tag != app->loaded_tag);
        app->show_cover_dialog = false;
    });

    auto btn_remove = Button("Remove", [app]() {
        app->edited_tag.cover_bytes.clear();
        app->edited_tag.cover_mime.clear();
        app->dirty = (app->edited_tag != app->loaded_tag);
        app->show_cover_dialog = false;
    });

    auto btn_cancel = Button("Cancel", [app]() {
        app->show_cover_dialog = false;
    });

    auto btns = Container::Horizontal({btn_load, btn_remove, btn_cancel});
    auto all  = Container::Vertical({input, btns});

    return Renderer(all, [all, error]() {
        Elements rows;
        rows.push_back(text("Cover Art") | bold | center);
        rows.push_back(separator());
        rows.push_back(text("Image path:"));
        rows.push_back(all->Render());
        if (!error->empty())
            rows.push_back(text(*error) | color(Color::Red));
        return vbox(rows) | border | size(WIDTH, GREATER_THAN, 50);
    });
}
