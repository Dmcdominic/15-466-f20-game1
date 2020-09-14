#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include "Load.hpp"
#include "load-sprites.cpp"
#include <random>
#include <time.h>



// Load tiles and palettes from chunk files
Load< LoadedSprites > loadedSprites(LoadTagDefault); //will 'new LoadedSprites()' by default


PlayMode::PlayMode() {
	std::cout << "--------- PlayMode() called --------" << std::endl;
	// Set palette_table and tile_table using loadedSprites
	ppu.palette_table = loadedSprites->palette_table;
	ppu.tile_table = loadedSprites->tile_table;

	// Background laid out using the first 6 tiles in "background sheet"
	for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
		for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
			ppu.background[x + PPU466::BackgroundWidth * y] = 4;
			if (y >= (PPU466::ScreenHeight / 8) - 6 || y < 3 ||
				x >= (PPU466::ScreenWidth / 8) - 4 || x < 3) {
				ppu.background[x + PPU466::BackgroundWidth * y] = 5;
				continue;
			}
			if (x % 4 == 3 || y % 4 == 3) {
				ppu.background[x + PPU466::BackgroundWidth * y] = 6;
			}
		}
	}

	// First, seed the random number generator
	std::srand((unsigned int)time(NULL));

	InitBoard();
}

void PlayMode::InitBoard() {
	// ---- BOARD SETUP ----
	std::cout << "Starting Board Setup" << std::endl;
	// an array to label which cards should use which tiles
	std::array< uint8_t, MEM_TILES_TOTAL > tile_order;
	for (uint8_t i = 0; i < tile_order.size(); i++) {
		tile_order[i] = MEM_TILES_START + i;
	}
	// Now shuffle it
	for (uint8_t i = 0; i < tile_order.size() - 1; i++) {
		uint8_t swap_index = i + (rand() % (MEM_TILES_TOTAL - i));
		uint8_t tmp = tile_order[i];
		tile_order[i] = tile_order[swap_index];
		tile_order[swap_index] = tmp;
	}
	std::cout << "Done creating tile_order. Now creating partner_order" << std::endl;

	// an array to label which cards will partner up
	std::array< uint8_t, BOARD_W* BOARD_H > partner_order;
	for (uint8_t i = 0; i < partner_order.size(); i++) {
		partner_order[i] = i;
	}
	// Now shuffle it
	for (uint8_t i = 0; i < partner_order.size() - 1; i++) {
		uint8_t swap_index = i + (rand() % (BOARD_W * BOARD_H - i));
		uint8_t tmp = partner_order[i];
		partner_order[i] = partner_order[swap_index];
		partner_order[swap_index] = tmp;
	}
	std::cout << "Done creating partner_order. Now initializing the board" << std::endl;

	// Initialize the board
	for (uint32_t cx = 0; cx < BOARD_W; cx++) {
		for (uint32_t cy = 0; cy < BOARD_H; cy++) {
			uint8_t cIndex = cx + cy * BOARD_W;
			Card* card = &Cards[cIndex];
			if (card->faceup) continue;

			// if it's not faceup, we have to initialize it and its partner
			uint8_t p_ctr = 0;
			uint8_t partner_index = partner_order[p_ctr];
			while (partner_index == UINT8_MAX || partner_index == cIndex || Cards[partner_index].faceup) {
				p_ctr++;
				partner_index = partner_order[p_ctr];
			}
			Card* partner = &Cards[partner_index];
			partner_order[p_ctr] = UINT8_MAX;

			card->faceup = true;
			partner->faceup = true;
			card->partner = partner_index;
			partner->partner = cIndex;

			card->attributes = 1;
			partner->attributes = 1;

			// pick an index and use it for these 2 cards. But no other cards can use it after this!
			uint8_t tileIndex = tile_order[cIndex];
			card->index = tileIndex;
			partner->index = tileIndex + MEM_TILES_TOTAL;
		}
	}
	// Flip them all facedown
	for (auto cardIter = Cards.begin(); cardIter != Cards.end(); cardIter++) {
		cardIter->faceup = false;
	}
	all_faceup = false;
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size, bool &QUIT) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT || evt.key.keysym.sym == SDLK_a) {
			selected_card.x = (selected_card.x == 0) ? (BOARD_W - 1) : (selected_card.x - 1);
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT || evt.key.keysym.sym == SDLK_d) {
			selected_card.x = (selected_card.x == BOARD_W - 1) ? (0) : (selected_card.x + 1);
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP || evt.key.keysym.sym == SDLK_w) {
			selected_card.y = (selected_card.y == BOARD_H - 1) ? (0) : (selected_card.y + 1);
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN || evt.key.keysym.sym == SDLK_s) {
			selected_card.y = (selected_card.y == 0) ? (BOARD_H - 1) : (selected_card.y - 1);
			down.downs += 1;
			down.pressed = true;
			return true;
		}

		// Flip the current card!
		if (evt.key.keysym.sym == SDLK_SPACE) {
			if (all_faceup) {
				InitBoard();
				return true;
			}
			uint8_t cardIndex = selected_card.x + selected_card.y * BOARD_W;
			Card *card = &Cards[cardIndex];
			if (faceup_cards[0] != UINT8_MAX && faceup_cards[1] != UINT8_MAX) {
				// TWO cards are already faceup. Flip them both facedown
				Cards[faceup_cards[0]].faceup = false;
				Cards[faceup_cards[1]].faceup = false;
				faceup_cards[0] = UINT8_MAX;
				faceup_cards[1] = UINT8_MAX;
			} else if (card->faceup) {
				// This is already faceup
				card->shake_time = 0.2f;
			} else if (faceup_cards[0] == UINT8_MAX) {
				// No other card is faceup. Flip this one
				corruptArea(cardIndex);
				card->faceup = true;
				faceup_cards[0] = cardIndex;
			} else {
				// Only one other card is faceup. Let's see if they're a match!
				card->faceup = true;
				if (card->partner == faceup_cards[0]) {
					// If they ARE a match, reset the faceup_cards
					score++;
					faceup_cards[0] = UINT8_MAX;
					faceup_cards[1] = UINT8_MAX;
					// Check here if they're ALL faceup. If so, set all_faceup
					for (auto cardIter = Cards.begin(); cardIter != Cards.end();) {
						if (!cardIter->faceup) {
							break;
						}
						cardIter++;
						if (cardIter == Cards.end()) {
							all_faceup = true;
						}
					}
				} else {
					// Otherwise, set the second faceup_card to this (and chance to corrupt)
					corruptArea(cardIndex);
					faceup_cards[1] = cardIndex;
				}
			}
		}

		// Quit!
		if (evt.key.keysym.sym == SDLK_q) {
			QUIT = true;
		}

	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	// (will be used to set background color)
	background_fade += elapsed / 10.0f;
	background_fade -= std::floor(background_fade);

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;

	// iterate over card shake_time
	for (auto cIter = Cards.begin(); cIter != Cards.end(); cIter++) {
		cIter->shake_time -= elapsed;
	}
}

// Corrupt a card and its neighbors
void PlayMode::corruptArea(uint8_t cIndex, bool bypass_rand) {
	if (!bypass_rand && corrupt_odds < (static_cast<float>(rand()) / static_cast<float>(RAND_MAX))) {
		corrupt_odds += corrupt_odds_delta;
		return;
	}
	Card *card = &Cards[cIndex];
	Card *up = &Cards[uint8_t((int(cIndex) - BOARD_W) % BOARD_TOTAL)];
	Card *down = &Cards[uint8_t((int(cIndex) + BOARD_W) % BOARD_TOTAL)];
	Card *left = &Cards[uint8_t((int(cIndex) - 1) % BOARD_TOTAL)];
	Card *right = &Cards[uint8_t((int(cIndex) + 1) % BOARD_TOTAL)];

	uint8_t randRow = rand() % 8;
	uint8_t tmp = ppu.tile_table[card->index].bit0[randRow];
	ppu.tile_table[card->index].bit0[randRow] = ppu.tile_table[up->index].bit0[randRow];
	ppu.tile_table[up->index].bit0[randRow] = ppu.tile_table[down->index].bit0[randRow];
	ppu.tile_table[down->index].bit0[randRow] = ppu.tile_table[left->index].bit0[randRow];
	ppu.tile_table[left->index].bit0[randRow] = ppu.tile_table[right->index].bit0[randRow];
	ppu.tile_table[right->index].bit0[randRow] = tmp;

	randRow = rand() % 8;
	tmp = ppu.tile_table[card->index].bit1[randRow];
	ppu.tile_table[card->index].bit1[randRow] = ppu.tile_table[up->index].bit1[randRow];
	ppu.tile_table[up->index].bit1[randRow] = ppu.tile_table[down->index].bit1[randRow];
	ppu.tile_table[down->index].bit1[randRow] = ppu.tile_table[left->index].bit1[randRow];
	ppu.tile_table[left->index].bit1[randRow] = ppu.tile_table[right->index].bit1[randRow];
	ppu.tile_table[right->index].bit1[randRow] = tmp;

	//make them shake
	card->shake_time = 0.4f;
	up->shake_time = 0.4f;
	down->shake_time = 0.4f;
	left->shake_time = 0.4f;
	right->shake_time = 0.4f;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//background color will be some hsv-like fade:
	ppu.background_color = glm::u8vec4(
		std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 0.0f / 3.0f) ) ) ))),
		std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 1.0f / 3.0f) ) ) ))),
		std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 2.0f / 3.0f) ) ) ))),
		0xff
	);

	// cards
	uint32_t sc = 0;
	for (uint32_t cx = 0; cx < BOARD_W; cx++) {
		for (uint32_t cy = 0; cy < BOARD_H; cy++) {
			Card *card = &Cards[cx + cy * BOARD_W];
			ppu.sprites[sc].x = BOARD_X + 32 * cx - 8;
			ppu.sprites[sc].y = BOARD_Y + 32 * cy - 8;
			if (card->shake_time > 0) {
				ppu.sprites[sc].x += 2 * (rand() % (2 * SHAKE_MULT)) - SHAKE_MULT;
				ppu.sprites[sc].y += 2 * (rand() % (2 * SHAKE_MULT)) - SHAKE_MULT;
			}
			if (card->faceup) {
				ppu.sprites[sc].index = card->index;
				ppu.sprites[sc].attributes = card->attributes;
			} else {
				ppu.sprites[sc].index = 7;
				ppu.sprites[sc].attributes = 0;
			}
			sc++;
		}
	}

	// select indicator
	for (uint32_t corner_y = 0; corner_y < 2; corner_y++) {
		for (uint32_t corner_x = 0; corner_x < 2; corner_x++) {
			ppu.sprites[sc].x = BOARD_X + 32 * selected_card.x - 8 + ((corner_x == 0) ? -8 : 8);
			ppu.sprites[sc].y = BOARD_Y + 32 * selected_card.y - 8 + ((corner_y == 0) ? -8 : 8);
			ppu.sprites[sc].index = 8 + corner_x + 2 * (1 - corner_y);
			ppu.sprites[sc].attributes = 0;
			sc++;
		}
	}

	// Score counter
	uint16_t tens = score / 10;
	uint16_t ones = score % 10;

	if (tens > 0) {
		ppu.sprites[sc].x = BOARD_X - 8;
		ppu.sprites[sc].y = BOARD_Y + 32 * 5 - 8;
		ppu.sprites[sc].index = MEM_TILES_START + tens;
		ppu.sprites[sc].attributes = 1;
		sc++;
	}

	ppu.sprites[sc].x = BOARD_X - 8 + 8;
	ppu.sprites[sc].y = BOARD_Y + 32 * 5 - 8;
	ppu.sprites[sc].index = MEM_TILES_START + ones;
	ppu.sprites[sc].attributes = 1;
	sc++;

	//--- actually draw ---
	ppu.draw(drawable_size);
}
