#pragma once

#include <string>


const std::string RAW_ASSETS_DIR_NAME = "raw_assets";
const std::string DIST_ASSETS_DIR_NAME = "dist/assets";


//construct a path based on the location of the currently-running executable:
// (e.g. if running /home/ix/game0/game.exe will return '/home/ix/game0/' + suffix)
std::string data_path(std::string const &suffix);

std::string raw_assets_path(std::string const& suffix);
std::string dist_assets_path(std::string const& suffix);
