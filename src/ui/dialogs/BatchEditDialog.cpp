#include "BatchEditDialog.hpp"
#include "core/TagWriter.hpp"
#include "ui/widgets/TagField.hpp"
#include "app/i18n.hpp"
#include <ftxui/dom/elements.hpp>
#include <array>

using namespace ftxui;

ftxui::Component make_batch_dialog(App* app) {
    // State stored in shared_ptr so it outlives the lambda captures
    struct State {
        TagData draft;
        std::array<bool, 8> checked = {true,true,true,true,true,true,true,false};
        // order: title, artist, album, year, track, genre, comment, cover
    };
    auto st = std::make_shared<State>();

    static const std::array<const char*, 8> FIELD_NAMES =
        {"title","artist","album","year","track","genre","comment","cover"};
    static const std::array<const char*, 8> LABELS =
        {"Title","Artist","Album","Year","Track","Genre","Comment","Cover"};

    // Checkboxes
    std::vector<Component> checkboxes;
    for (int i = 0; i < 8; ++i) {
        checkboxes.push_back(Checkbox(t(LABELS[i]), &st->checked[i]));
    }

    // Input fields (year/track are digits-only)
    std::vector<Component> inputs;
    inputs.push_back(make_tag_field(t("Title"),   &st->draft.title));
    inputs.push_back(make_tag_field(t("Artist"),  &st->draft.artist));
    inputs.push_back(make_tag_field(t("Album"),   &st->draft.album));

    // Convert year/track to string for editing
    auto year_str  = std::make_shared<std::string>();
    auto track_str = std::make_shared<std::string>();
    inputs.push_back(make_tag_field(t("Year"),    year_str.get(),  true));
    inputs.push_back(make_tag_field(t("Track"),   track_str.get(), true));
    inputs.push_back(make_tag_field(t("Genre"),   &st->draft.genre));
    inputs.push_back(make_tag_field(t("Comment"), &st->draft.comment));

    auto btn_apply = Button(t("Apply"), [app, st, year_str, track_str]() {
        // Parse year/track
        try { st->draft.year  = static_cast<uint32_t>(std::stoul(*year_str));  } catch (...) {}
        try { st->draft.track = static_cast<uint32_t>(std::stoul(*track_str)); } catch (...) {}

        std::set<std::string> fields;
        for (int i = 0; i < 8; ++i)
            if (st->checked[i]) fields.insert(FIELD_NAMES[i]);

        auto paths = app->batch_paths();
        if (!paths.empty()) {
            auto res = TagWriter::write_batch(paths, st->draft, fields);
            app->set_status("Batch: " + std::to_string(res.ok) + " ok, "
                           + std::to_string(res.error) + t(" errors"));
        }
        app->show_batch_dialog = false;
    });

    auto btn_cancel = Button(t("Cancel"), [app]() {
        app->show_batch_dialog = false;
    });

    // Build rows: checkbox + input side by side
    std::vector<Component> rows;
    for (int i = 0; i < static_cast<int>(inputs.size()); ++i) {
        rows.push_back(Container::Horizontal({checkboxes[i], inputs[i]}));
    }
    rows.push_back(Container::Horizontal({btn_apply, btn_cancel}));

    auto all = Container::Vertical(rows);

    return Renderer(all, [app, st, all]() {
        int n = static_cast<int>(app->selected_indices.size());
        std::string title_str = t("Batch Edit \xe2\x80\x94 ")
            + (n > 0 ? std::to_string(n) + t(" files") : t("current file"));

        return vbox({
            text(title_str) | bold | center,
            separator(),
            all->Render(),
        }) | border | size(WIDTH, GREATER_THAN, 55);
    });
}
