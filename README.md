Simple DIY EEPROM programmer for my stupid experiments.

**Warning**: This repo is not intended to be used by anyone other than me.

Don't forget to tweak the resulting byte array:

```sh
xxd -i rom.bin | sed 's/unsigned char/const byte/; $ d' > rom.h
```

To run a script-program written in Haskell you can either compile it and
then run the produced executable file by doing

```sh
ghc --make makerom
```

and then

```sh
./makerom
```

or you can use the `runhaskell` command like so:

```sh
runhaskell makerom.hs
```

and your program will be executed on the fly

--

All credits to Ben Eater.
