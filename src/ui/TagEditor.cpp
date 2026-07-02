#include "TagEditor.hpp"
#include "core/TagWriter.hpp"
#include "ui/widgets/TagField.hpp"
#include "app/i18n.hpp"
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <sstream>

using namespace ftxui;

ftxui::Component make_tag_editor(App* app) {
    auto year_str  = std::make_shared<std::string>();
    auto track_str = std::make_shared<std::string>();

    // Writes uint32_t fields → string buffers. Called only when the loaded
    // file changes or when an external source (online search) updates the
    // tags. NOT called on every render — that would overwrite mid-edit text.
    auto sync_strings = [app, year_str, track_str]() {
        *year_str  = app->edited_tag.year  ? std::to_string(app->edited_tag.year)  : "";
        *track_str = app->edited_tag.track ? std::to_string(app->edited_tag.track) : "";
    };

    // Called by on_change of year/track inputs AFTER the value is updated.
    // This is the only place that writes string → uint32_t during editing.
    auto dirty_check = [app]() {
        app->dirty = (app->edited_tag != app->loaded_tag);
    };
    auto on_year_change = [app, year_str, dirty_check]() {
        try { app->edited_tag.year = static_cast<uint32_t>(std::stoul(*year_str)); }
        catch (...) { app->edited_tag.year = 0; }
        dirty_check();
    };
    auto on_track_change = [app, track_str, dirty_check]() {
        try { app->edited_tag.track = static_cast<uint32_t>(std::stoul(*track_str)); }
        catch (...) { app->edited_tag.track = 0; }
        dirty_check();
    };

    auto f_title   = make_tag_field(t("Title"),   &app->edited_tag.title);
    auto f_artist  = make_tag_field(t("Artist"),  &app->edited_tag.artist);
    auto f_album   = make_tag_field(t("Album"),   &app->edited_tag.album);
    auto f_year    = make_tag_field(t("Year"),    year_str.get(),  true, 10, on_year_change);
    auto f_track   = make_tag_field(t("Track"),   track_str.get(), true, 10, on_track_change);
    auto f_genre   = make_tag_field(t("Genre"),   &app->edited_tag.genre);
    auto f_comment = make_tag_field(t("Comment"), &app->edited_tag.comment);

    auto fields = Container::Vertical({
        f_title, f_artist, f_album, f_year, f_track, f_genre, f_comment,
    });

    // CatchEvent only marks dirty for non-digit fields (their edited_tag.*
    // are live string pointers updated directly by Input). Year/track dirty
    // is handled by on_year_change / on_track_change above.
    auto fields_with_dirty = CatchEvent(fields, [app](Event) {
        app->dirty = (app->edited_tag != app->loaded_tag);
        return false;
    });

    auto component = CatchEvent(fields_with_dirty, [app, sync_strings](Event ev) {
        if (ev == Event::Tab) {
            app->focus     = AppFocus::Browser;
            app->focus_tab = 0;
            return true;
        }
        if (ev == Event::F2) {
            if (app->current_file.empty()) {
                app->set_status(t("No file loaded."));
                return true;
            }
            if (TagWriter::write(app->edited_tag, app->current_file)) {
                app->loaded_tag = app->edited_tag;
                app->dirty      = false;
                app->set_status(t("Saved: ") + app->current_file.filename().string());
            } else {
                app->set_status(t("ERROR: could not write tags to file."));
            }
            return true;
        }
        if (ev == Event::F6) {
            app->show_online_dialog = true;
            return true;
        }
        if (ev == Event::Escape) {
            if (app->dirty) {
                app->confirm_message = t("Discard unsaved changes?");
                app->confirm_yes = [app, sync_strings]() {
                    app->edited_tag = app->loaded_tag;
                    app->dirty = false;
                    sync_strings();
                };
                app->show_confirm_dialog = true;
            }
            return true;
        }
        if (ev == Event::F1) {
            app->show_help_dialog = true;
            return true;
        }
        return false;
    });

    // Track the last file for which we synced the string buffers.
    auto last_synced_file = std::make_shared<std::filesystem::path>();

    return Renderer(component, [app, component, sync_strings, last_synced_file]() {
        // Re-sync string buffers when the file changes or an external source
        // (e.g. online search) signals a resync is needed.
        if (app->current_file != *last_synced_file || app->editor_resync) {
            sync_strings();
            *last_synced_file    = app->current_file;
            app->editor_resync   = false;
        }

        bool focused = (app->focus == AppFocus::Editor);

        std::string header = " ";
        if (!app->current_file.empty()) {
            header += app->current_file.filename().string();
            if (app->dirty) header += " [*]";
        } else {
            header += t("(no file selected)");
        }

        std::string cover_info;
        if (!app->edited_tag.cover_bytes.empty()) {
            cover_info = app->edited_tag.cover_mime
                       + " (" + std::to_string(app->edited_tag.cover_bytes.size() / 1024) + " KB)";
        } else {
            cover_info = t("(no cover art)");
        }

        return vbox({
            text(header) | bold | color(Color::Cyan),
            separator(),
            component->Render(),
            separator(),
            hbox({
                text(t("Cover  : ")) | size(WIDTH, EQUAL, 12),
                text(" : "),
                text(cover_info) | color(app->edited_tag.cover_bytes.empty()
                                         ? Color::GrayDark : Color::Green),
                text(t("  [F5=CoverArt]")) | color(Color::GrayLight),
            }),
        }) | border | (focused ? color(Color::White) : color(Color::GrayDark));
    });
}
