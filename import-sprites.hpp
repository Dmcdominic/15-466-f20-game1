#pragma once

/*
 * Asset pipeline component for importing sprites as tiles and palettes.
 *
 */

#include <glm/glm.hpp>
#include "load_save_png.hpp"

#include <string>
#include <vector>
#include <stdint.h>


const std::string RAW_ASSETS_DIR_NAME = "raw_assets";
const std::string DIST_ASSETS_DIR_NAME = "dist/assets";

void import_spritesheet(std::string filename, OriginLocation origin);
std::string raw_assets_path(std::string const& suffix);