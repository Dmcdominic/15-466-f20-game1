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
#include <array>
#include "PPU466.hpp"


const std::string RAW_ASSETS_DIR_NAME = "raw_assets";
const std::string DIST_ASSETS_DIR_NAME = "dist/assets";

const uint8_t SPRITE_W = 8;
const uint8_t SPRITE_H = 8;
const uint8_t SPRITE_PAD = 2;


struct SpriteSet {
  std::array< glm::u8vec4, 4 > palette = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	};
  std::vector< PPU466::Tile > tiles = std::vector< PPU466::Tile >();
};


void import_spritesheet(std::string filename, OriginLocation origin);
std::string raw_assets_path(std::string const& suffix);
std::string dist_assets_path(std::string const& suffix);