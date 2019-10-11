# Diceware Passphrase Generator

An implementation of the [Diceware](world.std.com/~reinhold/diceware.html) method for creating passphrases.

## Background

Diceware is a method for creating passphrases using a special word list. It was created by Arnold G. Reinhold in 1995. Words are selected at random from the word list to form a passphrase. Each word is selected using five dice rolls. Each word in a Diceware word list corresponds to a five digit index (see example below).

```
16655 clause
16656 claw
16661 clay
16662 clean
16663 clear
16664 cleat
16665 cleft
16666 clerk
21111 cliche
```

Diceware word lists contain 7776 words, (6^5). Each randomly selected word adds log2(7776), 12.92 bits of entropy to a passphrase.

### Generating Dice Rolls

There are likely a number of different ways to generate a series five random digits in order to select Diceware words. This program attempts to follow the style of the Diceware method. ```dieRoll()``` generates an unpredictable random number, between 1 and 6, using the ```randombytes_uniform()``` function from **libsodium**.

### Checking Word List Integrity

The function ```verifyWordlist()``` checks the local directory to see if one of the following Diceware word lists exists: 

* diceware.wordlist.txt (original wordlist)
* beale.wordlist.txt (simplified by Alan Beale)
* eff.large.wordlist.txt (simplified by EFF)

The integrity of the word list is checked by validating a SHA-256 file hash of a chosen word list. **libsodium** is also used for calculating SHA-256 hashes.  

### Guarded Heap Allocations

**libsodium's** ```sodium_malloc()``` (and ```sodium_free()```) are used to allocate guarded memory regions and are used when generating dice rolls, constructing passphrases and reading word list files. ```sodium_malloc()``` also places implicit calls to other libsodium functions such as ```sodium_mlock()``` to help prevent memory regions being swapped to disk as well as ```sodium_memzero()``` to zero memory regions after use.  

### Note on Randomness

Strength of Diceware is its use of randomness but also its ability to generate passphrases more memorable than random alphanumeric or ASCII password strings. 

Diceware words cannot be selected based on their attractiveness, ease of memorising or other human-centric criteria. If words within a passphrase are generated from a uniformly random process, the only way to attack a Diceware passphrase is by brute force. Passphrases of six words in length are currently considered to be **strong** passphrases (77.52 bits of entropy).

If the word lists are public knowledge, can't Diceware passphrase be dictionary attacked? No. Dictionary attacks exploit weaknesses in human generated passwords, where certain password constructions are more likely than others. In Diceware, all passphrases are equally likely.

### True Random Number Generators

Diceware's creator strongly advocates use of actual dice as a source of randomness. If interested in generating authentic Diceware passphrases the EFF have you well covered with this attractive set of ["True RNGs"](https://supporters.eff.org/shop/eff-passphrase-dice).

### Diceware Word Lists

This repo also contains ```/wordlists``` with the three previously mentioned Diceware word lists to experimenting with. 

Others pre-built wordlists, in various languages, are available from the [Diceware site](http://world.std.com/~reinhold/diceware.html#Diceware%20in%20Other%20Languages|outline). 

## Compiling

There's no ```makefile```. The code can be compiled as follows with the ```libsodium``` linker option.

```>$ gcc -Wall diceware.c -lsodium -o diceware```

Follow installation guide for ```libsodium``` [here](https://libsodium.gitbook.io/doc/installation).

## To Do

* Experiment with [Chinese word lists](http://world.std.com/~reinhold/diceware.html#Diceware%20in%20Other%20Languages|outline). 
* Implement option to add randomly selected [special characters](http://world.std.com/~reinhold/dicewarefaq.html#specialchars) to a passphrase.

## Built With

* [GNU C](https://www.gnu.org/software/gnu-c-manual/)
* [Libsodium](https://github.com/jedisct1/libsodium)
* [Diceware Word List](http://world.std.com/~reinhold/diceware.wordlist.asc)
* [Alan Beale's Diceware Word List](http://world.std.com/~reinhold/beale.wordlist.asc)
* [EFF Diceware Word List](https://www.eff.org/files/2016/07/18/eff_large_wordlist.txt)

## Authors

* **Andrew Houlbrook** - **Initial work** - [AndrewHoulbrook](https://github.com/andrewhoulbrook)