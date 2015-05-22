# bch_codec
User BCH (Bose-Chaudhuri-Hocquenghem) encode/decode library based on bch module from linux kernel

License is GPL.

This is a fork of the bch.c file in the linux kernel written by Ivan Djelic at Parrot.
It closely follows the original with the following enhancements:
  * all kernel-specific functions have been removed
  * added bit level functions (rather than packed bytes) support for BCH messages, codewords, parity words
  * added error correction interface functions

The code has only been tested on linux, but seems portable.
