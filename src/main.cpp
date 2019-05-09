#include <io2d.h>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include "render.h"
#include "route_model.h"
#include "route_planner.h"

using namespace std::experimental;

float start_x, start_y, end_x, end_y;

static std::optional<std::vector<std::byte>>
ReadFile(const std::string& path) {
    std::ifstream is{path, std::ios::binary | std::ios::ate};
    if (!is)
        return std::nullopt;

    auto size = is.tellg();
    std::vector<std::byte> contents(size);

    is.seekg(0);
    is.read((char*)contents.data(), size);

    if (contents.empty())
        return std::nullopt;
    return std::move(contents);
}

void get_input_for(std::string key) {
    std::cout << "Enter the value for " << key << ": \t";

    if (key == "start_x") {
        std::cin >> start_x;
    } else if (key == "start_y") {
        std::cin >> start_y;
    } else if (key == "end_x") {
        std::cin >> end_x;
    } else if (key == "end_y") {
        std::cin >> end_y;
    }

    return;
}

int main(int argc, const char** argv) {
    std::string osm_data_file = "";
    if (argc > 1) {
        for (int i = 1; i < argc; ++i)
            if (std::string_view{argv[i]} == "-f" && ++i < argc)
                osm_data_file = argv[i];
    } else {
        std::cout << "Usage: [executable] [-f filename.osm]" << std::endl;
    }

    std::vector<std::byte> osm_data;

    if (osm_data.empty() && !osm_data_file.empty()) {
        std::cout << "Reading OpenStreetMap data from the following file: " << osm_data_file << std::endl;
        auto data = ReadFile(osm_data_file);
        if (!data)
            std::cout << "Failed to read." << std::endl;
        else
            osm_data = std::move(*data);
    }

    std::cout
        << R"HEREDOC(
We need some inputs for start and end positions.
Both position will need an X and Y.
And lastly, X and Y should be between 0 and 100.
)HEREDOC"
        << "\n";

    get_input_for("start_x");
    get_input_for("start_y");
    get_input_for("end_x");
    get_input_for("end_y");

    // Build Model.
    RouteModel model{osm_data};

    // Perform search and render results.
    RoutePlanner route_planner{model, start_x, start_y, end_x, end_y};

    route_planner.AStarSearch();
    std::cout << "\n"
              << "Distance: "
              << "\t\t\t"
              << route_planner.GetDistance()
              << " meters"
              << "\n";

    Render render{model};

    auto display = io2d::output_surface{640, 640, io2d::format::argb32, io2d::scaling::none, io2d::refresh_style::fixed, 30};
    display.size_change_callback([](io2d::output_surface& surface) {
        surface.dimensions(surface.display_dimensions());
    });
    display.draw_callback([&](io2d::output_surface& surface) {
        render.Display(surface);
    });
    display.begin_show();
}
