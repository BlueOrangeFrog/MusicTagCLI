#include "FileBrowser.hpp"
#include "core/TagReader.hpp"
#include "core/TagWriter.hpp"
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

ftxui::Component make_file_browser(
    App*                                   app,
    std::function<void(const FileEntry&)>  on_file_selected,
    std::function<void()>                  request_quit)
{
    // We build entry label strings from app->entries on every render.
    // The Menu component holds a pointer to the labels vector and selected index.
    auto labels      = std::make_shared<std::vector<std::string>>();
    auto selected_idx = std::make_shared<int>(0);

    // Synchronise *selected_idx with app->browser_cursor and rebuild labels.
    auto sync = [app, labels, selected_idx]() {
        *selected_idx = app->browser_cursor;
        labels->clear();
        labels->reserve(app->entries.size());
        int i = 0;
        for (const auto& e : app->entries) {
            std::string line;
            if (e.is_dir) {
                line = "  [DIR] " + e.display_name;
            } else {
                bool sel = app->selected_indices.count(i) > 0;
                bool cur = (i == app->browser_cursor && !e.is_dir);
                bool dirty_mark = cur && app->dirty;
                line = std::string(sel ? "*" : " ")
                     + "[" + format_label(e.format) + "] "
                     + (dirty_mark ? "[*] " : "    ")
                     + e.display_name;
            }
            labels->push_back(std::move(line));
            ++i;
        }
    };

    MenuOption opt = MenuOption::Vertical();
    opt.on_change = [app, selected_idx, on_file_selected]() {
        app->browser_cursor = *selected_idx;
        if (app->browser_cursor < static_cast<int>(app->entries.size())) {
            const auto& e = app->entries[app->browser_cursor];
            if (!e.is_dir && e.is_audio) on_file_selected(e);
        }
    };

    auto menu = Menu(labels.get(), selected_idx.get(), opt);

    auto component = CatchEvent(menu, [app, labels, selected_idx, sync,
                                       on_file_selected, request_quit](Event ev) {
        sync();

        int& cursor = app->browser_cursor;
        int  n      = static_cast<int>(app->entries.size());

        // Navigation
        if (ev == Event::ArrowDown || ev == Event::Character('j')) {
            if (cursor < n - 1) { ++cursor; *selected_idx = cursor; }
            if (cursor < static_cast<int>(app->entries.size())) {
                const auto& e = app->entries[cursor];
                if (!e.is_dir && e.is_audio) on_file_selected(e);
            }
            return true;
        }
        if (ev == Event::ArrowUp || ev == Event::Character('k')) {
            if (cursor > 0) { --cursor; *selected_idx = cursor; }
            if (cursor < static_cast<int>(app->entries.size())) {
                const auto& e = app->entries[cursor];
                if (!e.is_dir && e.is_audio) on_file_selected(e);
            }
            return true;
        }

        // Enter directory or load file
        if (ev == Event::Return || ev == Event::ArrowRight) {
            if (cursor >= 0 && cursor < n) {
                const auto& e = app->entries[cursor];
                if (e.is_dir) {
                    app->load_directory(e.path);
                    sync();
                }
            }
            return true;
        }

        // Parent directory
        if (ev == Event::ArrowLeft || ev == Event::Backspace) {
            if (app->current_dir.has_parent_path()) {
                app->load_directory(app->current_dir.parent_path());
                sync();
            }
            return true;
        }

        // Select / deselect
        if (ev == Event::Character(' ')) {
            if (cursor >= 0 && cursor < n && app->entries[cursor].is_audio) {
                if (app->selected_indices.count(cursor))
                    app->selected_indices.erase(cursor);
                else
                    app->selected_indices.insert(cursor);
            }
            return true;
        }

        // Select all audio files
        if (ev == Event::Character('a')) {
            for (int i = 0; i < n; ++i)
                if (app->entries[i].is_audio) app->selected_indices.insert(i);
            return true;
        }
        if (ev == Event::Character('A')) {
            app->selected_indices.clear();
            return true;
        }

        // Switch focus to editor
        if (ev == Event::Tab) {
            app->focus = AppFocus::Editor;
            return true;
        }

        // F5: batch
        if (ev == Event::F5) {
            if (app->selected_indices.empty() && app->current_file.empty()) {
                app->set_status("No files selected for batch edit.");
            } else {
                app->show_batch_dialog = true;
            }
            return true;
        }

        // F6: online
        if (ev == Event::F6) {
            app->show_online_dialog = true;
            return true;
        }

        // F1: help
        if (ev == Event::F1) {
            app->show_help_dialog = true;
            return true;
        }

        // F10: quit
        if (ev == Event::F10) {
            if (app->dirty) {
                app->confirm_message = "Unsaved changes — quit anyway?";
                app->confirm_yes = request_quit;
                app->show_confirm_dialog = true;
            } else {
                request_quit();
            }
            return true;
        }

        return false;
    });

    return Renderer(component, [app, component, sync]() {
        sync();
        int sel_count = static_cast<int>(app->selected_indices.size());
        std::string header = " " + app->current_dir.string();
        if (sel_count > 0)
            header += "  [" + std::to_string(sel_count) + " selected]";

        bool focused = (app->focus == AppFocus::Browser);

        return vbox({
            text(header) | bold | color(Color::Cyan),
            separator(),
            component->Render() | flex,
        }) | border | (focused ? color(Color::White) : color(Color::GrayDark));
    });
}
