xrick
=====

xrick is a clone of [Rick Dangerous](http://en.wikipedia.org/wiki/Rick_Dangerous),
known to run on macOS, Linux, Windows, BeOs, Amiga, QNX, all sorts of gaming consoles...

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

**Requirements:**

* [CMake](http://www.cmake.org/)
* [SDL](https://www.libsdl.org/download-1.2.php) version 1.2.x
* [zlib](http://www.zlib.net/)

1. *Install requirements*

  ```
  $ sudo apt-get install build-essential
  $ sudo apt-get install libsdl1.2-dev
  $ sudo apt-get install zlib1g-dev
  $ sudo apt-get install cmake
  ```

2. *Create a build directory*

  ```
  $ cd xrick-x.x.x
  $ mkdir build
  $ cd build
  ```

3. *Generate your Makefile*

  `$ cmake ../source/xrick/projects/cmake`

4. *Build*

  `$ make`

5. *Install (optional)*

  `$ make install`

Platform specific notes can be found in README.platforms.

Usage
-----

`xrick --help` will tell you all about command-line options.

### Data directory

xrick looks for its data in `data.zip` in the directory where it is run
from; `--data <archive>` points it to another zip file or to a directory
with the unpacked data. The high scores and the language files live next to
the data, in the *data directory*:

- the directory that contains the zip file, e.g. `/usr/local/share/xrick`
  for `--data /usr/local/share/xrick/data.zip`,
- or the data directory itself, if `--data` names a directory.

Without `--data`, this is the directory where xrick is run from. For an
installed xrick (`make install`), start it with `--data` pointing to the
installed `data.zip`, so that it finds the language files installed next
to it.

### High scores

High scores are saved to `highscores.txt` in the data directory. The file
is created empty on first start and can be edited by hand: one entry per
line as `SCORE NAME`, names up to 10 characters (A-Z, 0-9, `.` and
spaces). Delete the file to reset the high scores. If the data directory
is not writable (e.g. a system wide installation), xrick says so on the
console and keeps the high scores only until it exits.

### Languages

The in-game texts (map intros, game over, pause, name entry, coin mode and
level select) are read from `lang/en.txt` in the data directory.
`xrick --lang de` uses `lang/de.txt` instead. To add a language, copy
`en.txt` to `lang/<code>.txt`, translate it and start xrick with
`--lang <code>`; the file itself describes the format.

### Arcade mode

`xrick --coins` turns on the arcade mode: the start screen asks for a coin,
inserted with the C key, or shows the number of coins left. A coin
inserted on the start screen starts a game right away; with coins inserted
before, fire starts a game. Each coin is good for one game, coins inserted
while playing are kept for the next ones.

Controls
--------

- left, right, up (jump) or down (crawl): arrow keys or Z, X, O and K.
- fire: SPACE, end: E, pause: P, exit: ESC.
- insert coin (with `--coins` only): C.
- use left, right, up, down + fire to poke something with your stick,
  lay a stick of dynamite, or fire a bullet.
- toggle fullscreen: F1 ; zoom in/out: F2, F3.
- mute: F4 ; volume up/down: F5, F6.
- cheat modes, "trainer": F7 ; "never die": F8 ; "expose": F9.

More details at http://www.bigorno.net/xrick/

Release History
---------------

Please see the file called CHANGELOG.md.

Contacts
--------

Report problems or ask questions to:

* _BigOrno_ (bigorno@bigorno.net)
* _Pierluigi Vicinanza_ (pierluigi DOT vicinanza AT gmail.com)
