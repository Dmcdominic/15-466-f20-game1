# memcorrupt

Author: Dominic Calkosz

Design: A new take on the classic memory game of matching pairs. Flipping a card has a chance to corrupt it and its neighbors, permanently.

Screen Shot:

![Screen Shot](screenshot.png)

How Your Asset Pipeline Works:

Spritesheets of tiles (8x8 pixels with 2 pixel spacing) can be added to the raw_assets folder as .png files and then imported using asset_pipline/import-sprites.exe.
A single spritesheet can only include 4 distinct colors, and thus makes up a palette.
The sheet is imported, processed into a vector of tiles and a single palette, and then written to a chunk file in dist/assets.
At runtime, the chunk files are loaded and the tiles and palettes are set into the ppu tile_table and palette_table respectively.

How To Play:

* Use the arrow keys or WASD to swap which card you have selected.
* Press 'Space' to flip the current card.
 * Flip 2 matching cards at a time until you've uncovered all pairs.
 * Continue playing to improve your high score... but beware of the ever-increasing corruption.
* Press 'Q' to quit.

Sources: All assets created by me in Aseprite.

This game was built with [NEST](NEST.md).

