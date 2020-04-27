WinSk
=====

English | [French](README_FR.md)

A native client/server application, allowing you to control computer(s) remotely through commands.

The project contains:

- A client & server written in C language & exploiting [Windows API](https://en.wikipedia.org/wiki/Windows_API) & [Winsock](https://en.wikipedia.org/wiki/Winsock) libraries (EDI: [DevC++ 4.9.9.2](https://sourceforge.net/projects/dev-cpp/files/Binaries/Dev-C%2B%2B%204.9.9.2/)).
- An android server app written in Java, to control the computer(s) from your phone (EDI: Android Studio 2.3).

## Features

- Turn off client computer
- Cut the sound
- Send/receive files
- Stream client screen
- Update client app
- Explore client files/directories
- Keylogger
- ...

## Run on Linux

This project is mainly targeting the Windows OS, but it can also be runned & compiled on Linux using [Wine](https://www.winehq.org/).

:warning: Some features like the keylogger doesn't work on Linux.

## Changelog

Check file(s) `main.c` for changelog(s).

## License

This program is licensed under the [MIT](LICENSE) license.
