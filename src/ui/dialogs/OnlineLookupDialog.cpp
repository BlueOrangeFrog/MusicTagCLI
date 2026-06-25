#include "OnlineLookupDialog.hpp"
#include "online/MusicBrainzClient.hpp"
#include "online/AcoustIdClient.hpp"
#include "online/CoverArtClient.hpp"
#include <ftxui/dom/elements.hpp>
#include <thread>
#include <atomic>
#include <fstream>

using namespace ftxui;

// Load AcoustID API key from ~/.config/musictagcli/config.ini
static std::string load_acoustid_key() {
    const char* home = std::getenv("HOME");
    if (!home) return "";
    std::ifstream f(std::string(home) + "/.config/musictagcli/config.ini");
    std::string line;
    while (std::getline(f, line)) {
        if (line.rfind("acoustid_api_key=", 0) == 0)
            return line.substr(17);
    }
    return "";
}

ftxui::Component make_online_dialog(App* app) {
    struct State {
        int  result_cursor  = 0;
        bool mode_tag       = true;   // false = fingerprint
        std::vector<std::string> result_labels;
    };
    auto st = std::make_shared<State>();

    auto btn_by_tag  = Button("Search by Tags",        [app, st]() {
        st->mode_tag = true;
        app->online_results.clear();
        app->online_error.clear();
        app->online_searching.store(true);
        std::thread([app]() {
            auto r = MusicBrainzClient::search(
                app->edited_tag.title,
                app->edited_tag.artist,
                app->edited_tag.album);
            std::lock_guard<std::mutex> lk(app->online_mutex);
            app->online_results = std::move(r);
            if (app->online_results.empty()) app->online_error = "No results found.";
            app->online_searching.store(false);
        }).detach();
    });

    auto btn_by_fp = Button("Search by Fingerprint", [app, st]() {
        if (app->current_file.empty()) {
            std::lock_guard<std::mutex> lk(app->online_mutex);
            app->online_error = "No file loaded.";
            return;
        }
        st->mode_tag = false;
        app->online_results.clear();
        app->online_error.clear();
        app->online_searching.store(true);
        std::thread([app]() {
            std::string err;
            AcoustIdClient acoustid(load_acoustid_key());
            auto r = acoustid.lookup(app->current_file, err);
            std::lock_guard<std::mutex> lk(app->online_mutex);
            app->online_results = std::move(r);
            if (app->online_results.empty() && app->online_error.empty())
                app->online_error = err.empty() ? "No results found." : err;
            app->online_searching.store(false);
        }).detach();
    });

    auto menu_results = Menu(&st->result_labels, &st->result_cursor);

    auto btn_use = Button("Use this", [app, st]() {
        std::lock_guard<std::mutex> lk(app->online_mutex);
        if (st->result_cursor < static_cast<int>(app->online_results.size())) {
            const auto& r = app->online_results[st->result_cursor];
            if (!r.title.empty())  app->edited_tag.title  = r.title;
            if (!r.artist.empty()) app->edited_tag.artist = r.artist;
            if (!r.album.empty())  app->edited_tag.album  = r.album;
            if (!r.year.empty()) {
                try { app->edited_tag.year = static_cast<uint32_t>(std::stoul(r.year)); }
                catch (...) {}
            }
            if (!r.track_num.empty()) {
                try { app->edited_tag.track = static_cast<uint32_t>(std::stoul(r.track_num)); }
                catch (...) {}
            }
            app->dirty = (app->edited_tag != app->loaded_tag);
            app->set_status("Tags pre-filled — press F2 to save.");
            app->show_online_dialog = false;
        }
    });

    auto btn_cover = Button("Download Cover", [app, st]() {
        std::lock_guard<std::mutex> lk(app->online_mutex);
        if (st->result_cursor < static_cast<int>(app->online_results.size())) {
            const std::string mbid = app->online_results[st->result_cursor].mbid;
            std::thread([app, mbid]() {
                auto bytes = CoverArtClient::download_front(mbid);
                if (bytes) {
                    app->edited_tag.cover_bytes = std::move(*bytes);
                    app->edited_tag.cover_mime  = "image/jpeg";
                    app->dirty = (app->edited_tag != app->loaded_tag);
                    app->set_status("Cover art downloaded.");
                } else {
                    app->set_status("No cover art available for this release.");
                }
            }).detach();
        }
    });

    auto btn_close = Button("Close", [app]() {
        app->show_online_dialog = false;
    });

    auto top_btns  = Container::Horizontal({btn_by_tag, btn_by_fp});
    auto act_btns  = Container::Horizontal({btn_use, btn_cover, btn_close});
    auto all       = Container::Vertical({top_btns, menu_results, act_btns});

    // Capture all sub-components by value (shared_ptr) so Render() is accessible.
    return Renderer(all, [=]() {
        // Rebuild result labels under mutex
        {
            std::lock_guard<std::mutex> lk(app->online_mutex);
            st->result_labels.clear();
            for (const auto& r : app->online_results) {
                char buf[256];
                std::snprintf(buf, sizeof(buf), "[%.0f%%] %s — %s (%s)",
                    r.score * 100, r.title.c_str(), r.artist.c_str(), r.year.c_str());
                st->result_labels.emplace_back(buf);
            }
        }

        Elements rows;
        rows.push_back(text("Online Metadata Search") | bold | center);
        rows.push_back(separator());

        rows.push_back(hbox(btn_by_tag->Render(), text("  "), btn_by_fp->Render()));

        if (app->online_searching.load()) {
            rows.push_back(text("  Searching...") | color(Color::Yellow));
        } else if (!app->online_error.empty()) {
            rows.push_back(text(app->online_error) | color(Color::Red));
        } else if (!st->result_labels.empty()) {
            rows.push_back(separator());
            rows.push_back(menu_results->Render() | size(HEIGHT, LESS_THAN, 10));
            rows.push_back(separator());
            rows.push_back(hbox(btn_use->Render(), text(" "), btn_cover->Render()));
        }

        rows.push_back(separator());
        rows.push_back(btn_close->Render() | center);

        return vbox(rows) | border | size(WIDTH, GREATER_THAN, 60);
    });
}
