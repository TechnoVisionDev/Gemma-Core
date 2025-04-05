
Debian
====================
This directory contains files used to package gemmad/gemma-qt
for Debian-based Linux systems. If you compile gemmad/gemma-qt yourself, there are some useful files here.

## gemma: URI support ##


gemma-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install gemma-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your gemma-qt binary to `/usr/bin`
and the `../../share/pixmaps/gemma128.png` to `/usr/share/pixmaps`

gemma-qt.protocol (KDE)

