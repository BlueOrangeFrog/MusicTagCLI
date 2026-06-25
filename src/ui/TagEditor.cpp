#include "TagEditor.hpp"
#include "core/TagWriter.hpp"
#include "ui/widgets/TagField.hpp"
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <sstream>

using namespace ftxui;

ftxui::Component make_tag_editor(App* app) {
    // String buffers for year and track (which are stored as uint32_t in TagData)
    auto year_str  = std::make_shared<std::string>();
    auto track_str = std::make_shared<std::string>();

    // Sync string buffers from app->edited_tag
    auto sync_strings = [app, year_str, track_str]() {
        *year_str  = app->edited_tag.year  ? std::to_string(app->edited_tag.year)  : "";
        *track_str = app->edited_tag.track ? std::to_string(app->edited_tag.track) : "";
    };

    // Parse string buffers back to edited_tag (called before save)
    auto parse_strings = [app, year_str, track_str]() {
        try { app->edited_tag.year  = static_cast<uint32_t>(std::stoul(*year_str));  }
        catch (...) { app->edited_tag.year  = 0; }
        try { app->edited_tag.track = static_cast<uint32_t>(std::stoul(*track_str)); }
        catch (...) { app->edited_tag.track = 0; }
    };

    auto f_title   = make_tag_field("Title",   &app->edited_tag.title);
    auto f_artist  = make_tag_field("Artist",  &app->edited_tag.artist);
    auto f_album   = make_tag_field("Album",   &app->edited_tag.album);
    auto f_year    = make_tag_field("Year",    year_str.get(),  true);
    auto f_track   = make_tag_field("Track",   track_str.get(), true);
    auto f_genre   = make_tag_field("Genre",   &app->edited_tag.genre);
    auto f_comment = make_tag_field("Comment", &app->edited_tag.comment);

    auto fields = Container::Vertical({
        f_title, f_artist, f_album, f_year, f_track, f_genre, f_comment,
    });

    // Mark dirty whenever any Input changes
    auto fields_with_dirty = CatchEvent(fields, [app, parse_strings](Event) {
        bool was_dirty = app->dirty;
        parse_strings();
        app->dirty = (app->edited_tag != app->loaded_tag);
        return false; // don't consume — let the child handle it
    });

    auto component = CatchEvent(fields_with_dirty, [app, sync_strings, parse_strings](Event ev) {
        // Switch focus back to browser
        if (ev == Event::Tab) {
            app->focus = AppFocus::Browser;
            return true;
        }

        // F2: save
        if (ev == Event::F2) {
            if (app->current_file.empty()) {
                app->set_status("No file loaded.");
                return true;
            }
            parse_strings();
            if (TagWriter::write(app->edited_tag, app->current_file)) {
                app->loaded_tag = app->edited_tag;
                app->dirty      = false;
                app->set_status("Saved: " + app->current_file.filename().string());
            } else {
                app->set_status("ERROR: could not write tags to file.");
            }
            return true;
        }

        // F6: online search
        if (ev == Event::F6) {
            app->show_online_dialog = true;
            return true;
        }

        // ESC: discard changes
        if (ev == Event::Escape) {
            if (app->dirty) {
                app->confirm_message = "Discard unsaved changes?";
                app->confirm_yes = [app, sync_strings]() {
                    app->edited_tag = app->loaded_tag;
                    app->dirty = false;
                    sync_strings();
                };
                app->show_confirm_dialog = true;
            }
            return true;
        }

        // F1: help
        if (ev == Event::F1) {
            app->show_help_dialog = true;
            return true;
        }

        return false;
    });

    return Renderer(component, [app, component, sync_strings]() {
        // Keep string buffers in sync when the loaded tag changes externally
        // (e.g. navigating to a new file from the browser)
        sync_strings();

        bool focused = (app->focus == AppFocus::Editor);

        // File header
        std::string header = " ";
        if (!app->current_file.empty()) {
            header += app->current_file.filename().string();
            if (app->dirty) header += " [*]";
        } else {
            header += "(no file selected)";
        }

        // Cover art info row
        std::string cover_info;
        if (!app->edited_tag.cover_bytes.empty()) {
            cover_info = app->edited_tag.cover_mime
                       + " (" + std::to_string(app->edited_tag.cover_bytes.size() / 1024) + " KB)";
        } else {
            cover_info = "(no cover art)";
        }

        return vbox({
            text(header) | bold | color(Color::Cyan),
            separator(),
            component->Render(),
            separator(),
            hbox({
                text("Cover  : ") | size(WIDTH, EQUAL, 10),
                text(" : "),
                text(cover_info) | color(app->edited_tag.cover_bytes.empty()
                                         ? Color::GrayDark : Color::Green),
                text("  [F5=CoverArt]") | color(Color::GrayLight),
            }),
        }) | border | (focused ? color(Color::White) : color(Color::GrayDark));
    });
}
