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


void load_spritesheet(std::string filename, glm::uvec2* size, std::vector< glm::u8vec4 >* data, OriginLocation origin);
