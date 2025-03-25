# Windows NT 3.1 Kernel32 Shim

This is the repository for Discord Messenger's Kernel32 shim for Windows NT 3.1.  Its aim is to
implement a shim for certain APIs that were implemented in versions earlier than Discord Messenger
targets.  These functions aren't necessarily referenced by Discord Messenger or OpenSSL - both
of which can be modified from source to avoid linking them before runtime, but MinGW references
quite a few APIs that Windows NT 3.1 simply does not implement.

It performs its duty by:

- Using the actual procedures, when available, or

- Providing an (admittedly maybe crappy, but passable) implementation of the function, if Windows
  did not provide us with its own version.

## Usage

To use, simply build, and replace all `kernel32.dll` strings (they may be in caps) with `kernec32.dll`.
Then, drop the `kernec32.dll` resulting binary next to the executables that you need to use it with.

## License

This project is licensed under the MIT license.  See the license file for details.
