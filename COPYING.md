Any file in this project that doesn't state otherwise, and isn't listed as an exception below, is Copyright 2019 **The openhoi authors**, and licensed under the terms of the GNU General Public License Version 3, or (at your option) any later version ("GPL3+"). A copy of the license can be found in [legal/gpl-v3](/legal/gpl-v3) inside this repository.

_The openhoi authors are:_

| Full name                   | Aliases                     | E-Mail                                            |
|-----------------------------|-----------------------------|---------------------------------------------------|
| Thomas Reiser               | thomasreiser                | reiser.thomas à gmail dawt com                    |

If you're a first-time committer, add yourself to the above list. This is not just for legal reasons, but also to keep an overview of all those nicknames.

A full list of all openhoi authors ("contributors") can also be determined from the VCS, e.g. via `git shortlog -sne`, or conveniently looked up on [the GitHub web interface](https://github.com/openhoi/openhoi/graphs/contributors).

Details on individual authorships of files can be obtained via the VCS, e.g. via `git blame`, or the GitHub web interface.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License Version 3 for more details.

If you wish to include a file from **openhoi** in your project, make sure to include all required legal info. The easiest way to do this would probably be to include a copy of this file (`COPYING.md`), and to leave the file's copyright header untouched.

Per-file license header guidelines:

In addition to this file, to prevent legal caveats, every source file *must* include a header.

**openhoi-native** source files, that is, files that were created by _the openhoi authors_, require the following one-line header, preferably in the first line, as a comment:

    Copyright 20XX-20YY the openhoi authors. See COPYING.md for legal info.

`20XX` is the year when the file was created, and `20YY` is the year when the file was last edited. When editing a file, make sure the last-modification year is still correct.

Authors of 3rd-party files should generally not be entered in the "openhoi authors" list.

All 3rd-party files **must** be included in the following list:

List of all 3rd-party files in openhoi:

Source files
 - `thirdparty/imgui/imconfig.h`, `thirdparty/imgui/imgui*` → [MIT](/legal/mit-imgui)
 *<sub><sup>(taken from [ocornut/imgui](https://github.com/ocornut/imgui))</sup></sub>*
 
openhoi source files heavily influenced by 3rd-party code:
 - `game/include/gui/gui_manager.hpp`, `game/src/gui/gui_manager.cpp`, `game/include/gui/imgui_renderable.hpp`, `game/src/gui/imgui_renderable.cpp` → [MIT](/legal/mit-ogre-imgui)
 *<sub><sup>(original code extracted from [OGRECave/ogre-imgui](https://github.com/OGRECave/ogre-imgui)))</sup></sub>*

Graphic files
 - `dist/graphics/coat_of_arms/de.dds` / `dev-assets/coat_of_arms/de.svg` → free for any use but with copyright holder attribution
*<sub><sup>(taken from [Wikipedia](https://commons.wikimedia.org/wiki/File:Wappen_Deutsches_Reich_-_Reichsadler_1889.svg); Copyright [David Liuzzo](https://commons.wikimedia.org/wiki/User:David_Liuzzo))</sup></sub>*

Audio files
 - `dist/audio/background/tannhaeuser.ogg` → [public domain](/legal/public-domain-audio)
 *<sub><sup>(taken from [Wikipedia](https://commons.wikimedia.org/wiki/File:Overture_to_Tannhauser_-_U.S._Marine_Band.ogg); Composition: Richard Wagner in the 1840s; Performance: United States Marine Corps. from 22 May 1995 until 25 May 1995)</sup></sub>*
 - `dist/audio/background/ride_of_the_valkyries.ogg` → [public domain](/legal/public-domain-audio) 
 *<sub><sup>(taken from [Wikipedia](https://commons.wikimedia.org/wiki/File:Richard_Wagner_-_Ride_of_the_Valkyries.ogg); Composition: Richard Wagner in the 1870s; Performance: American Symphony Orchestra for Edison Records, transferred ot the United States National Park Service)</sup></sub>*
 - `dist/audio/background/coriolan.ogg` → [CC Public Domain Mark 1.0](/legal/cc-pdm-1.0) 
 *<sub><sup>(taken from [archive.org / Musopen](https://archive.org/details/MusopenCollectionAsFlac); Composition: Ludwig van Beethoven in 1807; Performance: Czech National Symphony Orchestra for the Musopen Project)</sup></sub>*

Material and shader files
 - `dist/materials/**/DualQuaternionSkinning_*`, `dist/materials/**/FFPLib_*`, `dist/materials/**/HardwareSkinningShadow*`, `dist/materials/**/SGXLib_*` → [MIT](/legal/mit-ogre)
 *<sub><sup>(taken from [OGRECave/ogre](https://github.com/OGRECave/ogre))</sup></sub>*
 - `dist/materials/**/imgui*` → [MIT](/legal/mit-ogre-imgui)
 *<sub><sup>(originally taken from [OGRECave/ogre-imgui](https://github.com/OGRECave/ogre-imgui))</sup></sub>*

Font files
 - `dist/fonts/gui.ttf` → [SIL 1.1](/legal/sil-1.1)
 *<sub><sup>(based on from [Google Noto Fonts](https://www.google.com/get/noto) but extended by SS runes)</sup></sub>*
 - `dev-assets/squealer.ttf` → [Free for our usage (logo creation)](/legal/typodermic.pdf) *<sub><sup>(taken from [Typodermic Fonts](http://typodermicfonts.com/squealer/))</sup></sub>*

CMake Files
 - `cmake/cgal/FindCGAL.cmake` → [BSD](/legal/bsd-findcgal)
 *<sub><sup>(taken from [sidch/CMake](https://github.com/sidch/CMake))</sup></sub>*
 - `cmake/global/FindRapidJSON.cmake` → [BSD](/legal/bsd-findrapidjson)
 *<sub><sup>(taken from [lbaehren/CMakeModules](https://github.com/lbaehren/CMakeModules))</sup></sub>*