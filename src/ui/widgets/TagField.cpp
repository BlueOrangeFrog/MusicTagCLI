#include "TagField.hpp"
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

ftxui::Component make_tag_field(const std::string& label,
                                 std::string* value,
                                 bool digits_only,
                                 int  label_width)
{
    InputOption opt;
    if (digits_only) {
        opt.on_change = [value]() {
            std::string filtered;
            for (char c : *value)
                if (std::isdigit(static_cast<unsigned char>(c)))
                    filtered += c;
            *value = std::move(filtered);
        };
    }

    auto input = Input(value, "", opt);

    // Wrap the input in a Renderer that adds the label
    return Renderer(input, [label, label_width, input]() {
        return hbox({
            text(label) | size(WIDTH, EQUAL, label_width),
            text(" : "),
            input->Render() | flex,
        });
    });
}
