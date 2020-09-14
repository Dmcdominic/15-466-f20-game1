#include "import-sprites.hpp"

#include <glm/glm.hpp>
#include "load_save_png.hpp"
#include "data_path.hpp"
#include "read_write_chunk.hpp"

#include <string>
#include <vector>
#include <stdint.h>
#include <array>
#include <fstream>


//...and for c++ standard library functions:
#include <iostream>
#include <stdexcept>



int main(int argc, char** argv) {
#ifdef _WIN32
	//when compiled on windows, unhandled exceptions don't have their message printed, which can make debugging simple issues difficult.
	try {
#endif
		
		//import_spritesheet("test spritesheet", UpperLeftOrigin);
		import_spritesheet("background sheet", UpperLeftOrigin);
		import_spritesheet("mem tiles", UpperLeftOrigin);
		return 0;

#ifdef _WIN32
	}
	catch (std::exception const& e) {
		std::cerr << "Unhandled exception:\n" << e.what() << std::endl;
		return 1;
	}
	catch (...) {
		std::cerr << "Unhandled exception (unknown type)." << std::endl;
		throw;
	}
#endif
}


// Loads a .png spritesheet at filename into a chunk
void import_spritesheet(std::string filename, OriginLocation origin) {
	std::cout << "import_spritesheet() called" << std::endl;
	std::string png_filename = raw_assets_path(filename) + ".png";
	std::string chunk_filename = dist_assets_path(filename) + "_CHUNK";
	// Load the png
	glm::uvec2 size;
	std::vector< glm::u8vec4 > data;
	load_png(png_filename, &size, &data, origin);
	std::cout << "SIZE: " << (size.x) << ", " << (size.y) << "\n";
	
	// Iterate over the png data, converting it to tiles and a palette
	SpriteSet s;
	uint8_t colorTotal = 0; // Tracks how many new colors we've found and placed in palette

	for (uint16_t sprite_y = 0; glm::u32(sprite_y * (SPRITE_H + SPRITE_PAD) + SPRITE_H) <= size.y; sprite_y++) {
		for (uint16_t sprite_x = 0; glm::u32(sprite_x * (SPRITE_W + SPRITE_PAD) + SPRITE_W) <= size.x; sprite_x++) {
			// Now iterate over the 64 pixels in this 8x8 sprite
			PPU466::Tile tile = PPU466::Tile();
			uint8_t sx = 0; //the x index into the 8x8 tile
			uint8_t sy = 0;
			for (uint16_t x = sprite_x * (SPRITE_W + SPRITE_PAD); x < sprite_x * (SPRITE_W + SPRITE_PAD) + SPRITE_W; x++) {
				sy = 0;
				for (uint16_t y = sprite_y * (SPRITE_H + SPRITE_PAD); y < sprite_y * (SPRITE_H + SPRITE_PAD) + SPRITE_H; y++) {
					uint16_t i = x + y * size.x;
					assert(i < size.x * size.y);

					uint8_t colr;
					for (colr = 0; colr < colorTotal && colr < s.palette.size(); colr++) {
						if (s.palette[colr].r == data.at(i).r && s.palette[colr].g == data.at(i).g && s.palette[colr].b == data.at(i).b && s.palette[colr].a == data.at(i).a) {
							break;
						}
					}

					if (colr == colorTotal) {
						if (colorTotal >= s.palette.size()) {
							std::cerr << "Exceeded " << s.palette.size() << " colors in spritesheet: " << filename << std::endl;
							throw;
						}
						s.palette[colr].r = data.at(i).r;
						s.palette[colr].g = data.at(i).g;
						s.palette[colr].b = data.at(i).b;
						s.palette[colr].a = data.at(i).a;
						colorTotal++;
					}
					// Set bit0 and bit1 for the tile
					if (colr % 2 == 1) {
						tile.bit0[SPRITE_H - sy - 1] += 1 << sx;
					}
					if (colr > 1) {
						tile.bit1[SPRITE_H - sy - 1] += 1 << sx;
					}
					sy++;
				}
				sx++;
			}
			assert(sx == SPRITE_W && sy == SPRITE_H);
			s.tiles.emplace_back(tile);
		}
	}

	// Format the palette and tile together into one chunk
	std::cout << "Formatting the palette and tile together into one chunk" << std::endl;
	data.clear();
	for (uint8_t colr = 0; colr < s.palette.size(); colr++) {
		data.emplace_back(s.palette[colr]);
	}
	glm::u8vec4 vec;
	for (auto t_iter = s.tiles.begin(); t_iter != s.tiles.end(); t_iter++) {
		vec = glm::u8vec4(t_iter->bit0[0], t_iter->bit0[1], t_iter->bit0[2], t_iter->bit0[3]);
		data.emplace_back(vec);
		vec = glm::u8vec4(t_iter->bit0[4], t_iter->bit0[5], t_iter->bit0[6], t_iter->bit0[7]);
		data.emplace_back(vec);
		vec = glm::u8vec4(t_iter->bit1[0], t_iter->bit1[1], t_iter->bit1[2], t_iter->bit1[3]);
		data.emplace_back(vec);
		vec = glm::u8vec4(t_iter->bit1[4], t_iter->bit1[5], t_iter->bit1[6], t_iter->bit1[7]);
		data.emplace_back(vec);
	}

	// Save the tile and palette into one chunk
	std::cout << "Writing the chunk file" << std::endl;
	std::ofstream chunk_file(chunk_filename, std::ios::binary);
	write_chunk(std::string("sprt"), data, &chunk_file);
	chunk_file.close();
}
