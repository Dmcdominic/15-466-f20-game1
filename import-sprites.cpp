#include <glm/glm.hpp>
#include "load_save_png.hpp"

#include <string>
#include <vector>
#include <stdint.h>


//...and for c++ standard library functions:
#include <iostream>
#include <stdexcept>

// TODO - main...?



void load_spritesheet(std::string filename, glm::uvec2* size, std::vector< glm::u8vec4 >* data, OriginLocation origin) {
  // TODO
}


int main(int argc, char** argv) {
#ifdef _WIN32
	//when compiled on windows, unhandled exceptions don't have their message printed, which can make debugging simple issues difficult.
	try {
#endif
		
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
