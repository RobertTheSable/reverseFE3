# FE3 Reverse Recruitment Maker

Program that generates a reverse recruitment ROM for Fire Emblem: Monshou no Nazo.

This should be compatible with any ROM, including JP and TL-patched ROMs, but I have not tested it extensivley.

## Files in this repository

### src

The source code.

### ini

Configuration files neccesary for the sorter to run. 

#### SHVC FIREEMBLEM
These files contain important address presets. Some are different between 1.0 and 1.1, hence the need for different config files.

#### characters.ini
Contains mapping info about character/unit data. This includes ordering, item data, class data, and more.
* The Book1/2 sections derermine character order.
    * The first element is the in-game character name byte.
    * The second is the name in text, which is used for the HTML output.
    * The last element is the group, which is important for when multiple of characters are recruited at the same time.
        * For example, Rikard and Wendel would normally get swapped in Book 1 reordering, but they aren't actually swapped because they are recruited at the ame point, more-or-less.
* Classes and Items match class and item bytes to text.
* Chapters is used for determining if a map is indoor or not. The first element is the chapter index, and the second if a character ID.
    * Some maps are only partly indoors, so a specific character ID needs to be given.
    * Fully indoor maps have 0xFF for the character ID.

#### text.ini
An incomplete mapping of text data. Really it only the lenght to skip after reading a certain byte. This is used for replacing portaits in text, which is necessary for cases like Samto's recruitment.

#### events.ini
Another incomplete length mapping for event data. This is used for the limited event parsing/fixing the sorter does, like the Book 1 Tiki fix for example.

## Compiling

Simply run "make" to generate an executeable in the build folder. All required ini files will be copied as well.
