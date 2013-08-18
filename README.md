kibibeat - programmatically generating percussive MIDI tracks
=============================================================

kibibeat is a text-based MIDI drum sequencer. You create a percussive track programmatically using a domain specific language.
kibibeat can't actually play music. It generates a Standard MIDI File that can be played by most music players.

Tutorial
--------

A percussive track is a combination of synchronized tracks, each one played by a single instrument (e.g. a kick drum or a snare drum). In kibibeat you create these single-instrument tracks independently and then mix them to the main track.

Each instrument track is represented by a Beat List, also known as blist. A [beat](http://en.wikipedia.org/wiki/Beat_(music\)) is a unit of time determined by the music tempo, which is constant inside a blist. We mark the beats in which the instrument is attacked by plus signs (`+`). The other beats are represented by minus signs (`-`). A blist is represented by a sequence of beats enclosed by square brackets, like `[+-+-]`. You can insert other non-whitespace ASCII characters inside a blist to make it more readable (e.g. bar delimiters), like `[-+-+|--++]`. These characters will be ignored by kibibeat.

kibibeat uses a stack-based language to let you perform operations on beats and other entities. Every operation is represented in [postfix notation](http://en.wikipedia.org/wiki/Reverse_Polish_notation), in which the operator follows all of its operands. For example, to repeat a pattern two we use the times operator (`*`):

```
[+--+] 2 *
```

There are 3 tokens on this example. Tokens are always separated by whitespace (regular spaces, linebreaks or tabs). A token can be a beat, a blist, a number or a name. kibibeat will read tokens from left to right and put then in a stack. When an operator token is found (such as `*`), it will be applied to objects that are on the stack, usually removing then. If an operation has an output, it will be pushed to the top of the stack. So, supposing the stack was empty before the operation above, after it the stack will contain only the blist `[+--++--+]`.

Once the blist for an instrument is defined, we can add it to the main track using the mix operator (`x`).

```
[+--+] 2 *
40 127 360 x
```

The mix operator has four operands:

* The blist of the track;
* The MIDI note number to be used in each `+` beat (e.g. 40 for a electric snare on the [General MIDI Standard Drum Map](http://en.wikipedia.org/wiki/File:GMStandardDrumMap.gif));
* The MIDI volume to use for `+` beats (0-127);
* The number of ticks per beat (currently kibibeat always assume 360*120 ticks per second).

We can also bind objects (such as a beat, a blist or a number) to names, making it easier to refer to then latter. Names should start with a dollar sign (`$`). The bind operator (`=`) binds an object to a name.

```
$bar [+--+] =
$bar 2 *
40 127 360 x
```

If we save the above code into a file named `mus.kb`, we can then run

```
kib mus.kb
```

and it will create a file `kibibeat.mid` (this name is not yet customizable) containing the track generated.

Interactive Mode
----------------

The easiest way to learn the kibibeat language is using the interactive mode.

Just call the kib executable without any argument:

```
kib
```

It will print a prompt (`> `), after which you can write tokens to be processed. Each time you hit return, it will print the contents of the stack and the current name table. The symbol `/` represents an empty stack and `{}` is an empty name table.

```
> $foo
$foo
{}
> [+-+] =
/
{$foo: [+-+]}
> $foo 2 *
[+-++-+]
{$foo: [+-+]}
```

Note that in the interactive mode there's no MIDI file output. It's mostly a didactic tool, also useful for testing.

kibibeat operators
------------------

* Invert (`~`)
```
BLIST ~
```
Invert every beat in the blist.

* Clear (`@`)
```
BLIST @
```
Clear every beat in the blist, setting it to `-`.

* Or (`|`)
```
BLIST BLIST |
```
Beatwise OR.
The length of the result is the length of the shortest input.

* And (`&`)
```
BLIST BLIST &
```
Beatwise AND.
The length of the result is the length of the shortest input.

* Repeat (`*`)
```
BLIST NUMBER *
```
Expand the blist to repeat its content number times.

* Pattern (`^`)
```
BLIST BLIST ^
```
Expand the first blist to multiply its length by the length of the second blist.
For each `+` beat on the second blist, there will be a copy of the first blist on the result.
For each `-` beat on the second blist, there will be a cleared version of the first blist on the result.

* Left Rotate (`<<`)
```
BLIST NUMBER <<
```
Rotate the blist number times to the left.

* Right Rotate (`>>`)
```
BLIST NUMBER >>
```
Rotate the blist number times to the right.

* Left Pad (`<`)
```
BLIST NUMBER BEAT <
```
Pad blist to the left by inserting number beats to the right.

* Right Pad (`>`)
```
BLIST NUMBER BEAT >
```
Pad blist to the right by inserting number beats to the left.

* Bind (`=`)
```
NAME OBJECT =
```
Bind object to name.

* Mix (`x`)
```
BLIST NUMBER NUMBER NUMBER x
```
Make a track with blist and add it to the current mix.
The first number is the MIDI note to use for `+` beats.
The second number is the MIDI volume to use for `+` beats.
The third number is the number of ticks per beat.
