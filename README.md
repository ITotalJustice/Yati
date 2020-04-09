# YATI (Yet Another Title Installer).

This project has been abandoned.

It was great fun working on it and I got a lot further than I thought I would, however I no longer find it fun to work on.

----

# Current Progress

__**Currently can install:**__

* .nca
* .nsp
* .xci
* .nsz
* .xsz

__**Install Protocals:**__

* file browser for sd card installs
* direct gamecard installs
* very basic http / https support, loads the url from `/switch/yati/http_game.txt`
* very basic usb support. drag and drop the file you want to install onto the `usb_total.py`

__**Misc**__

* auto ignores fw version.
* split installs for all of the above (nca, nsp, xci, nsz, xsz).
* ticket browser + option to delete selected tickets (will be more feature complete soon).
* installs latest sig patches for either hekate / fusee primary users.

----

# What's next (in no particular order)

* remove hard coded header_key_source key (will do this first).
* re-write most of the code.
* "standard crypto" for ticketless installs.
* option to delete application.
* option to change current install location of installed application (i think theres an ipc func for it?).
* option to games to any format (ncas, nsp, xci, nsz, xsz).
* option to dump specific sections of nca (pfs0, header, romfs etc). would be useful for debugging and tools such as layeredFs.
* ticket dumping.
* installed app info when selected (play time, app size, save size, save owner(s), mkey, does require linked nnid etc).
* http / https browsing support.
* cloud install support.
* threaded installs (close to done).
* nca verification.
* option remove account dependency (useful for apps such as youtube which require a linked account). This can be easily done by parsing the control nca and changing the bit at offset `0x3213` to 0.
* ~~gamecard direct install (install a game directly from the cart).~~ added!

__Low Priority__
* option to launch installed application.
* option to list / load homebrew.
* usb hdd support for sx users.
* gui for the app (will do this last).
* write gui for usb.
* python script to split all file types, with a gui (very low priority).

----

# Credits

* Massive thank you for the fantastic documentation over on [switchbrew](https://switchbrew.org/wiki/Main_Page).
* Everyone that has contributed to [libnx](https://github.com/switchbrew/libnx).
* [Devkitpro](https://devkitpro.org/) for the fantastic toolchain.
* [Tinfoil](https://github.com/Adubbz/Tinfoil) for the functions `nca_get_string_from_id` and `nca_get_id_from_string` as well as being a great refrence for cnmt parsing.

----

# Patreon

Feel free to support me on patreon, I would really appreciate it!

<a href="https://www.patreon.com/totaljustice"><img src="https://c5.patreon.com/external/logo/become_a_patron_button@2x.png" alt="Patreon donate button" /> </a>
