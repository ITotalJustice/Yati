# Atmosphere-Updater

YATI (Yet Another Title Installer).

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
* installs latest sig patches for either hekate / fusee primary users.

----

# What's next (in no particular order)

* remove hard coded header_key_source key (will do this first).
* re-write most of the code.
* http / https browsing support.
* cloud install support.
* threaded installs (close to done).
* write gui for usb.
* nca verification.
* option remove account dependency (useful for apps such as youtube which require a linked account). This can be easily done by parsing the control nca and changing the bit at offset `0x3213` to 0.
* python script to split all file types, with a gui (very low priority).
* ~~gamecard direct install (install a game directly from the cart).~~ added!
* dump games to any format (ncas, nsp, xci, nsz, xsz).
* usb hdd support for sx users.
* gui for the app (will do this last).

----

# Patreon

Feel free to support me on patreon, I would really appreciate it!

<a href="https://www.patreon.com/totaljustice"><img src="https://c5.patreon.com/external/logo/become_a_patron_button@2x.png" alt="Patreon donate button" /> </a>
