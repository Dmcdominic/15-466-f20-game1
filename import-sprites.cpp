#include <glm/glm.hpp>
#include "load_save_png.hpp"
#include "data_path.hpp"
#include "import-sprites.hpp"
#include "read_write_chunk.hpp"

#include <string>
#include <vector>
#include <stdint.h>
#include <fstream> 


//...and for c++ standard library functions:
#include <iostream>
#include <stdexcept>



int main(int argc, char** argv) {
#ifdef _WIN32
	//when compiled on windows, unhandled exceptions don't have their message printed, which can make debugging simple issues difficult.
	try {
#endif
		
		import_spritesheet("test spritesheet", LowerLeftOrigin);
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


// TODO - description
// Loads a .png spritesheet at filename into a chunk
void import_spritesheet(std::string filename, OriginLocation origin) {
	std::string png_filename = raw_assets_path(filename) + ".png";
	std::string chunk_filename = dist_assets_path(filename) + ".chunk";
	// Load the png
	glm::uvec2 size;
	std::vector< glm::u8vec4 > data;
	load_png(png_filename, &size, &data, origin);
	std::cout << "SIZE:\n" << (size.x) << ", " << (size.y) << "\n";
	// TODO - Iterate over the png data, converting it to tiles and a palette
	//std::array<PPU466::Palette, 8U> palette_table;
	auto data_iter = data.begin();
	for (uint16_t r = 0; r < size.x; r++) {
		for (uint16_t c = 0; c < size.y; c++) {
			assert(data_iter != data.end());
			//TODO

			data_iter++;
		}
	}
	assert(data_iter == data.end());
	// Save the tile and palette into one chunk
	std::ofstream chunk_file(chunk_filename, std::ios::binary);
	write_chunk(std::string("sprt"), data, &chunk_file);
}


// Gets the path to the raw_assets folder
std::string raw_assets_path(std::string const& suffix) {
	static std::string _raw_assets_path;
	if (_raw_assets_path.empty()) {
		_raw_assets_path = data_path("");
		_raw_assets_path = _raw_assets_path.substr(0, _raw_assets_path.rfind('\\'));
		_raw_assets_path = _raw_assets_path + "/" + RAW_ASSETS_DIR_NAME;
	}
	return _raw_assets_path + "/" + suffix;
}

// Gets the path to the dist/assets folder
std::string dist_assets_path(std::string const& suffix) {
	static std::string _dist_assets_path;
	if (_dist_assets_path.empty()) {
		_dist_assets_path = data_path("");
		_dist_assets_path = _dist_assets_path.substr(0, _dist_assets_path.rfind('\\'));
		_dist_assets_path = _dist_assets_path + "/" + DIST_ASSETS_DIR_NAME;
	}
	return _dist_assets_path + "/" + suffix;
}