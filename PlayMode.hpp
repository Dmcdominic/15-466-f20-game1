#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <array>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size, bool &QUIT) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	virtual void corruptArea(uint8_t cIndex, bool bypass_rand = false);
	virtual void InitBoard();

	//----- settings -----
	static uint8_t const MEM_TILES_TOTAL = 32;
	static uint8_t const MEM_TILES_START = 16;

	static uint8_t const BOARD_W = 6;
	static uint8_t const BOARD_H = 5;
	static uint8_t const BOARD_TOTAL = BOARD_W * BOARD_H;

	static uint8_t const BOARD_X = 8 * 6;
	static uint8_t const BOARD_Y = 8 * 6;

	static uint8_t const SHAKE_MULT = 1;


	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	glm::uvec2 selected_card = glm::uvec2(0, 0); // Which card the selector is currently on
	std::array< glm::uint8_t, 2 > faceup_cards = { UINT8_MAX, UINT8_MAX}; // the cards' index in Cards, if faceup. Otws, it is UINT8_MAX

	struct Card {
		uint8_t index;
		uint8_t attributes;
		bool faceup = false;
		uint8_t partner; // Partner card's index in Cards
		float shake_time = 0.0f; // set this above 0 to have it "shake" for that many seconds
	};
	std::array< Card, BOARD_W * BOARD_H > Cards;

	bool all_faceup = true;

	float corrupt_odds = 0.1f;
	float corrupt_odds_delta = 0.001f;
	uint16_t score = 0;

	//some weird background animation:
	float background_fade = 0.0f;


	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
