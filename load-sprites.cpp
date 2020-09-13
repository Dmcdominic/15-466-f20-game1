#include "load-sprites.hpp"

#include <glm/glm.hpp>
#include "Load.hpp"
#include "data_path.hpp"
#include "read_write_chunk.hpp"

#include <string>
#include <vector>
#include <array>
#include <fstream>


//...and for c++ standard library functions:
#include <iostream>
#include <stdexcept>
#include "PPU466.hpp"



struct LoadedSprites {
	LoadedSprites();
	~LoadedSprites();

	// Only room for 8 palettes, so we can't have more than 8 spritesheets
	const std::array< std::string, 1 > sheet_names = { "test spritesheet" };
	const std::array< std::string, 1 > magics = { "sprt" };

	std::array< PPU466::Palette, 8 > palette_table;
	std::array< PPU466::Tile, 16 * 16 > tile_table;

	//Textures bindings:
	//TEXTURE0 - the tile table (as a 128x128 R8UI texture)
	//TEXTURE1 - the palette table (as a 4x8 RGBA8 texture)
};



// Loaded Sprites constructor & destructor
LoadedSprites::LoadedSprites() {
	std::cout << std::endl << "-------- LoadedSprites() called ---------" << std::endl;
	// Init palette and tile tables (based on PPU466())
	for (auto& palette : palette_table) {
		palette[0] = glm::u8vec4(0x00, 0x00, 0x00, 0x00);
		palette[1] = glm::u8vec4(0x00, 0x44, 0x44, 0xff);
		palette[2] = glm::u8vec4(0x99, 0x99, 0x99, 0xff);
		palette[3] = glm::u8vec4(0xff, 0xff, 0xff, 0xff);
	}

	for (auto& tile : tile_table) {
		tile.bit0 = { 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0 };
		tile.bit1 = { 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff };
	}

	// Iterate over the spritesheet chunks to load
	size_t tileCounter = 0; // Which tile goes into the tile_table next
	for (uint8_t i=0; i < sheet_names.size(); i++) {
		std::string sheet_name = sheet_names[i];

		std::vector< glm::u8vec4 > data;

		std::string chunk_filename = dist_assets_path(sheet_name) + "_CHUNK";
		std::ifstream chunk_file(chunk_filename, std::ios::binary);
		read_chunk(chunk_file, magics[i], &data);
		chunk_file.close();

		uint16_t dctr = 0;

		// Load the palette
		for (uint8_t colr = 0; colr < palette_table[i].size(); colr++) {
			palette_table[i][colr] = data[dctr];
			dctr++;
		}
		
		// Load the tiles
		for (size_t iTC = 0; (iTC + 1) * 4 + 8 <= data.size(); iTC++) {
			assert(tileCounter < tile_table.size());
			assert(dctr < data.size());

			// bit0
			for (uint16_t i = 0; i < 4; i++) {
				tile_table[tileCounter].bit0[i] = data[dctr][i];
			}
			dctr++;
			assert(dctr < data.size());
			for (uint16_t i = 4; i < 8; i++) {
				tile_table[tileCounter].bit0[i] = data[dctr][i - 4];
			}
			dctr++;
			assert(dctr < data.size());
			// bit1
			for (uint16_t i = 0; i < 4; i++) {
				tile_table[tileCounter].bit1[i] = data[dctr][i];
			}
			dctr++;
			assert(dctr < data.size());
			for (uint16_t i = 4; i < 8; i++) {
				tile_table[tileCounter].bit1[i] = data[dctr][i - 4];
			}
			dctr++;

			tileCounter++;
		}
	}
}


LoadedSprites::~LoadedSprites() {}