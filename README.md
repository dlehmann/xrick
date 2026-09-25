xrick
=====

xrick is a clone of [Rick Dangerous](http://en.wikipedia.org/wiki/Rick_Dangerous),
known to run on macOS, Linux, Windows, BeOs, Amiga, QNX, all sorts of gaming consoles...

This version adds a few things to the original xrick:

- the high scores are kept in an editable text file,
- the in-game texts can be translated, English and German come with it,
- an arcade mode where every game needs a coin,
- entities walk in front of wooden beams, like in the Amiga version.

Contents:

- [License agreement & legal bable](#license-agreement--legal-bable)
- [Building](#building)
- [Playing](#playing): [controls](#controls),
  [command line options](#command-line-options),
  [changing the keys](#changing-the-keys)
- [Data directory](#data-directory)
- [High scores](#high-scores)
- [Languages and translations](#languages-and-translations)
- [Arcade mode](#arcade-mode)
- [Release history](#release-history), [contacts](#contacts)

License agreement & legal bable
-------------------------------

* Copyright (C) 1998-2002 BigOrno (bigorno@bigorno.net) (http://www.bigorno.net/xrick/)
* Copyright (C) 2008-2014 Pierluigi Vicinanza (pierluigi DOT vicinanza AT gmail.com)

I (BigOrno) have written the initial [xrick](http://www.bigorno.net/xrick/) code.
However, graphics and maps and sounds are by the authors of the original Rick Dangerous
game, and "Rick Dangerous" remains a trademark of its owner(s) -- maybe
Core Design (who wrote the game) or FireBird (who published it).
As of today, I have not been successful at contacting Core Design.

This makes it a bit difficult to formally release the whole code,
including data for graphics and maps and sounds, under the terms of
licences such as the GNU General Public Licence. So the code is
released "in the spirit" of the GNU GPL. Whatever that means.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.

Building
--------

### Requirements

* a C compiler (GCC, clang or MSVC)
* [CMake](http://www.cmake.org/) 3.10 or newer
* [SDL](https://www.libsdl.org/download-1.2.php) 1.2; on current systems
  [sdl12-compat](https://github.com/libsdl-org/sdl12-compat) provides it
  on top of SDL 2 or 3
* [zlib](http://www.zlib.net/)

On Debian or Ubuntu:

```
$ sudo apt-get install build-essential cmake libsdl1.2-dev zlib1g-dev
```

### Build

```
$ cd xrick-x.x.x
$ mkdir build
$ cd build
$ cmake ../source/xrick/projects/cmake
$ make
```

This builds `build/xrick`. To play without installing, run it from the
`game` directory, which holds the game data (`data.zip`) and the language
files:

```
$ cd ../game
$ ../build/xrick
```

### Build options

Options are switched with `-D<option>=ON` or `OFF` when calling `cmake`,
e.g. `cmake -DENABLE_JOYSTICK=ON ../source/xrick/projects/cmake`.

| Option                | Default | Meaning                                                        |
|-----------------------|---------|----------------------------------------------------------------|
| `GFXST`               | ON      | Atari ST graphics                                              |
| `GFXPC`               | OFF     | PC (CGA) graphics, instead of `GFXST`                          |
| `ENABLE_SOUND`        | ON      | sound and music                                                |
| `ENABLE_JOYSTICK`     | OFF     | joystick support                                               |
| `ENABLE_CHEATS`       | ON      | the cheat keys F7, F8 and F9                                   |
| `ENABLE_FOCUS`        | OFF     | pause the game when the window loses the focus                 |
| `ENABLE_ZIP`          | ON      | read the data from a zip archive (else only from a directory)  |
| `ENABLE_HISCORE_FILE` | ON      | keep the high scores in `highscores.txt`                       |
| `ENABLE_LANG_FILE`    | ON      | read the in-game texts from language files                     |
| `ENABLE_DEVTOOLS`     | OFF     | a viewer for the tiles, sprites and blocks, shown before the game |
| `DEBUG_xxx`           | OFF     | debug output on the console for one part of the game           |

The data archive has to match the graphics: the `game/data.zip` that comes
with xrick is made for the ST graphics. For the PC graphics, build the data
extractor in `source/data_extractor` with `GFXPC` and create the data
files with it.

Note that CMake writes the configuration to `source/xrick/config.h`, in the
source tree: with several build directories, the last one configured wins,
so run `cmake .` again in the directory you build.

### Install (optional)

```
$ make install
```

installs `xrick` to `/usr/local/bin`, and `data.zip`, the language files
and this README to `/usr/local/share/xrick` (change the prefix with
`cmake -DCMAKE_INSTALL_PREFIX=...`). Start the installed xrick with
`--data /usr/local/share/xrick/data.zip`, see
[Data directory](#data-directory).

Notes for the Rockbox version can be found in `assets/docs/README.rockbox`.

Playing
-------

Start xrick from the directory that contains `data.zip` (e.g. `game/`), or
tell it where the data is with `--data`. Options can be combined in any
order, for example:

```
$ xrick --fullscreen --lang de --keys a-d-w-s-SPACE
```

`xrick --help` explains all options.

Rick explores four worlds: South America, Egypt, a castle in Europe and a
missile base. Walk, jump, climb and crawl past the traps; poke enemies
with your stick to stun them for a moment, shoot them, or blow them up
with dynamite. You carry up to 6 bullets and 6 sticks of dynamite; boxes
refill them, and so do starting a new world and losing a life. You have
6 lives.

### Controls

| Key                     | Action                                                   |
|-------------------------|----------------------------------------------------------|
| arrow keys, or Z X O K  | left, right, up (jump, climb), down (crawl, climb down)  |
| SPACE                   | fire, and start the game on the title screen             |
| fire + left or right    | poke with the stick                                      |
| fire + up               | shoot a bullet                                           |
| fire + down             | lay a stick of dynamite                                  |
| P                       | pause, press again to continue                           |
| E                       | end the game (game over)                                 |
| ESC                     | quit xrick, at once and without asking                   |
| C                       | insert a coin (with `--coins` only)                      |
| F1                      | window / fullscreen                                      |
| F2, F3                  | zoom the window out / in                                 |
| F4                      | sound off / on                                           |
| F5, F6                  | volume down / up                                         |
| F7                      | cheat "trainer": unlimited lives, bullets and dynamite   |
| F8                      | cheat "never die"                                        |
| F9                      | cheat "expose": show hidden entities, also behind walls  |

The cheats only work while playing and are shown as T, N and V at the
top left of the screen. Z X O K and SPACE can be changed, see
[Changing the keys](#changing-the-keys).

With a joystick (build option `ENABLE_JOYSTICK`), the stick moves and any
button fires.

### Command line options

**`-h`, `--help`**

Print an explanation of the options and exit.

**`--version`**

Print the version and the copyright notice and exit.

**`--fullscreen`**

Start in fullscreen mode instead of a window. xrick picks the smallest
screen mode that fits the game and zooms to it; `--zoom` only applies to
the window. F1 switches between window and fullscreen while playing.

**`--zoom <zoom>`**

Size of the window: the game is 320x200 pixels, `<zoom>` multiplies that,
from 1 (320x200) to 4 (1280x800). Default: 2 (640x400). F2 and F3 zoom
out and in while playing.

**`--speed <speed>`**

Game speed, as the time between two frames in milliseconds: from 1
(fastest) to 100 (slowest). Default: 40, the normal speed.
`--speed 30` is a bit faster, `--speed 50` a bit slower.

**`--map <map>`**

Start the game at the beginning of world `<map>`, from 1 to 4:
1 South America, 2 Egypt, 3 Europe (castle), 4 Europe (missile base).
Default: 1. After a game over, the next game starts there again.

**`--submap <submap>`**

Start the game at submap (screen area) `<submap>`, from 1 to 47. The
world is derived from it (submaps 1-9 are world 1, 10-20 world 2, 21-38
world 3, 39-47 world 4), so `--submap` overrides `--map`; the first
submap of a world (1, 10, 21, 39) is the same as `--map`. Mostly useful to
test a particular place of a map.

**`--keys <left>-<right>-<up>-<down>-<fire>`**

Change the keys for the five game controls, see
[Changing the keys](#changing-the-keys) below.

**`--data <archive>`**

Where the game data is: a zip file, or a directory with the unpacked data.
Default: `data.zip` in the directory where xrick is run from. This also
decides where the high scores and language files are, see
[Data directory](#data-directory).

**`--nosound`**

Start without sound. Without this option sound is on; F4 mutes and
unmutes it while playing.

**`--vol <vol>`**

Sound volume at start, from 0 (silent) to 10 (loudest). Default: 10.
F5 and F6 turn it down and up while playing.

**`--lang <code>`**

Language of the in-game texts, read from `lang/<code>.txt` in the data
directory, e.g. `--lang de` for German. Default: `en`. xrick exits with
an error if the requested file can not be read. See
[Languages and translations](#languages-and-translations).

**`--coins`**

Arcade mode: every game needs a coin, see [Arcade mode](#arcade-mode).

Options that take a value exit with an error message if the value is
missing or out of range, as do unknown options.

### Changing the keys

`--keys` sets the keys for left, right, up (jump, climb), down (crawl,
climb down) and fire, in this order, as key names separated by `-`:

```
$ xrick --keys <left>-<right>-<up>-<down>-<fire>
```

All five keys must be given. The default is `z-x-o-k-SPACE`. The arrow
keys always work for the four directions as well, whatever `--keys` says.

Key names are not case sensitive (`a` and `A` are the same key).
`assets/docs/KeyCodes` lists them all; the most useful ones are:

| Keys                          | Names                                                                     |
|-------------------------------|---------------------------------------------------------------------------|
| letters, digits               | `a` ... `z`, `0` ... `9`                                                   |
| space, enter, tab, backspace  | `SPACE`, `RETURN`, `TAB`, `BACKSPACE`                                      |
| arrow keys                    | `LEFT`, `RIGHT`, `UP`, `DOWN`                                             |
| modifiers                     | `LSHIFT`, `RSHIFT`, `LCTRL`, `RCTRL`, `LALT`, `RALT`, `LSUPER`, `RSUPER`  |
| keypad                        | `KP0` ... `KP9`, `KP_PERIOD`, `KP_ENTER`, `KP_PLUS`, `KP_MINUS`, ...     |
| navigation                    | `INSERT`, `DELETE`, `HOME`, `END`, `PAGEUP`, `PAGEDOWN`                   |
| punctuation                   | `COMMA`, `PERIOD`, `MINUS`, `PLUS`, `SLASH`, `SEMICOLON`, `QUOTE`, ...   |

Examples:

```
$ xrick --keys a-d-w-s-SPACE              # WASD, fire with space
$ xrick --keys LEFT-RIGHT-UP-DOWN-LCTRL   # arrow keys, fire with left ctrl
$ xrick --keys KP4-KP6-KP8-KP2-KP0        # numeric keypad
```

Arcade controllers usually send keys: many USB encoders are set up like
MAME, with the stick on the arrow keys and the first buttons on `LCTRL`,
`LALT` and `SPACE`. For such a stick and fire on the first button, use
`--keys LEFT-RIGHT-UP-DOWN-LCTRL`. If you do not know which keys your
encoder sends, try a key tester (e.g. `xev` on Linux) first.

The other keys can not be changed yet: P pause, E end the game, ESC quit,
C insert a coin, the function keys F1 to F9, and A and B. Do not give
`--keys` one of these keys: xrick then only does one of the two actions
(e.g. with `E` as fire, E still ends the game and never fires).

Data directory
--------------

xrick looks for its data in `data.zip` in the directory where it is run
from; `--data <archive>` points it to another zip file or to a directory
with the unpacked data. The high scores and the language files live next to
the data, in the *data directory*:

- the directory that contains the zip file, e.g. `/usr/local/share/xrick`
  for `--data /usr/local/share/xrick/data.zip`,
- or the data directory itself, if `--data` names a directory.

Without `--data`, this is the directory where xrick is run from. So in the
source tree, `game/` holds `data.zip`, `lang/` and `highscores.txt`
together. For an installed xrick, start it with `--data` pointing to the
installed `data.zip`, so that it finds the language files installed next
to it.

High scores
-----------

The hall of fame keeps the 8 best scores. They are saved to
`highscores.txt` in the [data directory](#data-directory):

- At start, xrick reads the file. If it does not exist, xrick creates it,
  empty, so the hall of fame starts empty.
- After a game whose score makes it into the hall of fame, xrick asks for
  a name and saves the file right away. A score of 0 never gets in.
- If the file can not be written (e.g. the data directory is not writable
  in a system wide installation), xrick says so on the console; the high
  scores then only last until xrick exits.

The file is plain text and can be edited by hand, e.g. to correct a name
or to reset the high scores:

```
# xrick high scores, one entry per line: SCORE NAME
# NAME: up to 10 characters, A-Z, 0-9, '.' and spaces
012500 RICK
008700 INDY
```

- One entry per line: the score, then blanks, then the name. The score is
  written with six digits, but leading zeroes are optional; scores above
  999999 become 999999.
- Names have up to 10 characters, longer ones are cut. Lower case is upper
  cased; anything but A-Z, 0-9 and `.` becomes a blank. A line with only
  a score gives an entry without a name.
- Empty lines and lines starting with `#` are ignored, as are lines that
  do not start with a number (xrick says so on the console).
- The order of the lines does not matter: xrick sorts the entries and
  keeps the 8 best.
- Delete the file, or all its entries, to reset the high scores.

Edit the file while xrick is not running: xrick only reads it at start,
and rewrites it (with its own two comment lines, dropping yours) after
each new entry, which would undo changes made in the meantime.

Built without `ENABLE_HISCORE_FILE`, xrick starts with the hall of fame of
the original game and forgets new entries when it exits.

Languages and translations
--------------------------

The in-game texts are read from `lang/en.txt` in the
[data directory](#data-directory); `--lang <code>` reads
`lang/<code>.txt` instead. xrick comes with:

- `en.txt`: English, the texts of the original game, and the template for
  new languages,
- `de.txt`: German.

If `lang/en.txt` is missing, xrick uses the texts of the data archive. If
a language asked for with `--lang` is missing, xrick exits with an error.

### What is translated

| Section                   | Text                                                    | Lines | Width |
|---------------------------|---------------------------------------------------------|-------|-------|
| `[intro1]` ... `[intro4]` | map intro of the four worlds: a title, then the story   | 1 + 11 | 30   |
| `[intro5]`                | end of the game, after the last world                   | 1 + 11 | 30   |
| `[gameover]`              | game over banner                                        | 1     | 23    |
| `[paused]`                | pause banner                                            | 1     | 23    |
| `[entername]`             | prompt on the high score name screen                    | 1     | 30    |
| `[insertcoin]`            | blinking prompt on the start screen with `--coins`      | 1     | 16    |
| `[credits]`               | label of the coin counter with `--coins`                | 1     | 13    |
| `[world1]` ... `[world4]` | world names on a hidden level select screen             | 1     | 26    |
| `[worldnumber]`           | "world 1 of 4" line of that screen                      | 1     | 30    |
| `[pressfire]`             | prompt of that screen                                   | 1     | 30    |

Width is the maximum number of characters per line. The titles and
pictures of the original game (e.g. "Rick Dangerous", the Core Design
logo, the hall of fame and congratulations titles) are graphics and can
not be translated.

### Writing a translation

1. Copy `lang/en.txt` to `lang/<code>.txt`, e.g. `lang/fr.txt`.
2. Translate the lines below each `[section]` header; keep the headers as
   they are, in lower case.
3. Start xrick with `--lang <code>` and look at each text: the map intros
   show before each world, game over after losing the last life (or with
   E), pause with P, the name screen after a high score, the coin texts
   with `--coins`.
4. Watch the console: xrick reports lines that are too long, too many
   lines, and unknown sections.

Things to know:

- **The font only has A-Z, 0-9, blanks and `,` `.` `?`**. Lower case is
  upper cased, and German umlauts are written as AE, OE, UE and SS. Any
  other character, including accented letters like é or ñ and marks like
  `!`, `:`, `-` or `'`, becomes a blank: write `E` instead of `É`, and
  rephrase instead of using other marks.
- **Count umlauts as two characters**: "GRÜN" is 5 characters wide on
  screen (GRUEN).
- **Lines longer than the width are cut**, and xrick prints them on the
  console. Lines are centered, so shorter lines are fine.
- **The file must be UTF-8**, which most editors use by default. In
  another encoding (e.g. Latin-1), umlauts become blanks.
- **Lines starting with `#` are comments**, anywhere in the file. A text
  line can not start with `#`.
- **Empty lines** at the start and end of a section are ignored; empty
  lines in between are kept, e.g. to separate paragraphs of a map intro.
- **Missing sections keep the English texts** of the data archive, so a
  translation can be done step by step. Extra lines in one line sections
  are ignored.
- In `[worldnumber]`, the first `#` is replaced by the number of the world
  and the second one by the number of worlds, e.g. `WELT # VON #` shows
  "WELT 2 VON 4".
- In `[credits]`, xrick adds a blank and the number of coins after the
  text, which is why it is 3 characters shorter than `[insertcoin]`.

Built without `ENABLE_LANG_FILE`, xrick always shows the English texts of
the data archive.

Arcade mode
-----------

`xrick --coins` turns xrick into an arcade machine: every game needs a
coin.

- The start screen shows a blinking INSERT COIN while there are no coins,
  else the number of coins left (CREDITS 2).
- C inserts a coin, up to 99. A coin inserted on the start screen starts a
  game right away; with coins inserted before, fire starts a game.
- Each game uses one coin and gives the usual 6 lives. Coins inserted while
  playing are kept for the next games.
- Without coins, fire does nothing on the start screen.
- Coins are not saved: they are lost when xrick exits.

For an arcade cabinet, combine it with `--fullscreen`, and with `--keys`
for the stick and fire button, e.g.:

```
$ xrick --coins --fullscreen --keys LEFT-RIGHT-UP-DOWN-LCTRL
```

Keep in mind that ESC quits xrick at once, and E ends the current game.

Release History
---------------

Please see the file called CHANGELOG.md.

More details on the original xrick at http://www.bigorno.net/xrick/

Contacts
--------

Report problems or ask questions to:

* _BigOrno_ (bigorno@bigorno.net)
* _Pierluigi Vicinanza_ (pierluigi DOT vicinanza AT gmail.com)
