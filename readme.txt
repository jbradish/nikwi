Nikwi Deluxe
============


0. Contents
-----------
  1. About
     1.1. About Slashstone
  2. Compiling
  3. Running


1. About
--------
   Nikwi is a 2D platform game i wrote and released to the unsuspecting world 
 in December 2005.  After that the world remained unsuspected as if nothing
 had happened, until a year later when i released Nikwi Deluxe.  Again,
 nothing has changed for the world much, until a while later the game was
 picked up by an unofficial Ubuntu repository and after that the official
 Debian repository.  Still, the world remained unsuspected at large.  So now,
 about seven years after the original game's release, i re-release the game
 yet again - this time on GitHub with a new licence: zlib.  Based on previous
 experience, i can foresee that the world will still not notice.

   Nikwi (before the Deluxe version) was my first commercial (shareware) game.
 At the time the word "indie" wasn't yet widely known and nobody paid really
 any attention to platform games (yes, there was such a time!).  In fact when
 i released and announced it in a game development forum (the only place i knew
 that had anything to do about indie games in general) there were comments
 about how it isn't a puzzle or casual game.  At that time the casual scene
 was still in the hands of small developers, it was exploding fast and if you
 weren't making a casual game, you were doing it wrong - aparently i did it
 wrong since Nikwi never had a single sale :-).

   So a year later i decided to release Nikwi as open source (and practically
 forget about it).  I fixed a couple of bugs, made a couple of levels slightly
 easier and made the "deluxe" version.  At the time i was still under the
 impression that open source should be GPL, so i sticked GPL to the code and
 released it on the Internet.  I submitted it to Happy Penguin (the Linux game
 tome or somethinglike that), got some nice commens, saw it being ported to
 PSP at some point later, etc.

   These days while i was looking on my previous PC's hard disk, i found the
 source code of Nikwi, so i decided to clean it up and relicense it under the
 license i prefer these days - zlib.  This release isn't any different from
 the original Nikwi Deluxe release.  The only thing i changed is the build
 process that now relies on a single Makefile instead of a custom preprocessor
 that i had written at the time and a single bug fix i found while running the
 game under Valgrind - just in case (although i never saw that bug to actually
 occur).  Also this time the source release contains all the needed sources
 and "source" data files since none of the libraries i used for Nikwi are
 developed any more.

   The game took about three weeks to write.  The earliest post i can find on
 the Internet about the game was for a second alpha version i released in
 November 28, 2005 and the game was released in December 15, 2005 (so that
 would be 15 days).  Before the second Alpha, i was working on it for about
 3-4 days and i made a patch the next day of the release (there was a sound
 glitch), so rounded up it was three weeks of development.  The development
 was *very* intense - for personal reasons, i had to finish the game as fast
 as i could.  Most of the days i slept for only like 6 hours and worked on
 the game for the rest of the time almost non-stop (pausing only to eat). At
 the last "day", i was away for about 30 hours after 5 hours of sleep.  While
 this isn't something i really want to repeat, i can say that i liked the
 experience - i just wish i could take it easier at the time and fix a couple
 of issues the game had.  But that was my lesson that rushing something is
 rarely a good idea :-).  Having said that, i think the game was good
 considering the circumstances.

1.1. About Slashstone
     ----------------
   Slashstone was the name i used at the time for my game and program releases
 in a similar way to Runtime Legend of today.  Unfortunately, like the rest of
 my domain names, slashstone.com was snatched away when due to a health issue
 i couldn't pay for my domain names and cybersquatters registered them almost
 instantly.  Please ignore the references to slashstone.com and Slashstone in
 the game and use runtimelegend.com instead.


2. Compiling
------------
   To compile the game you need GNU Make, SDL 1.2 development libraries and
 a UNIX environment (MSYS under Windows is fine).  Simply go to the root
 directory of the source code release and type:

      make

 This will build the program and the data files.  To clean the source tree
 type:

      make clean

 There isn't a make install/uninstall at the moment.  You need to run the
 game from its directory.  The Debian maintainers made some modifications to
 the code to make the game be able to "spread out" to the whole system in a
 UNIX-y fashion, but since i wanted to change the license, i decided to avoid
 their patches.  I may reimplement the patches at some point.

 Note that if you want to work on the data files, you need to make sure that
 the justdata.up file is not built.  Type:

      make nikwi

 to only make the program and skip the justdata.up file.


3. Running
----------
   Nikwi Deluxe needs the data files at the current directory.  It looks in
 the justdata.up file for the data files (justdata.up is a package file that
 was originally used by a 3D game engine i was working at the time - the up
 extension means "Undead Package" because the engine was named "Undead
 Engine") and if the justdata.up file does not exist, it looks in the data
 directory.  The justdata.up file is generated using the upack tool which can
 be found in the src/tools directory.

   By default Nikwi Deluxe runs in windowed mode, but it can be made to run
 in fullscreen mode by adding the --fullscreen argument, like:

      nikwi --fullscreen

 Under Windows it is recommended to use a shortcut that adds the argument.

 For information on playing the game, see the manual.

 Kostas "Bad Sector" Michalopoulos
 badsectoracula@gmail.com
 badsector@runtimelegend.com




