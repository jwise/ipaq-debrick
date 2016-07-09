# `debrick`: a collection of old things from the handhelds.org days

In this directory is a collection of old JTAG-related tools from the
handhelds.org days.  Probably most of this dates back to 2002-2004.  Code
quality and authorship varies; some of it is code that's identifiably mine
that's good code, and some of it is code that's identifiably mine that's
crap code, and some of it is code that I can't figure out who wrote, and
some is code from varying other authors (I think Matt Reimer and Ian Molton
feature prominently).  I won't spend too much time
documenting much of it, but below is a quick summary for the curious.

I hope you enjoy.

## `libjtag` / `jtag_core`

Most of these directories have a Xeroxed chunk of code in them, `libjtag`
and `jtag_core`.  Apparently I believed in the `cp` method of code reuse.
I think this worked by opening the `/dev/parport` device, and bit-banging an
attached JTAG level shifter.

## `bsrflash`: boundary scan flash tools for NOR Flash

This directory contains two tools, `bsrdevmem` and `bsrflash`, which operate
on what looks like chip select #0 on Intel PXA250/PXA255.  `bsrdevmem` is
about what you expect; `bsrflash` can control whatever NOR flash part iPAQ
h1910 had, and also some Intel StrataFlash, which presumably is what iPAQ
h5450 had.

Reflashing 128 kB using `bsrflash`, I believe, took around 45 minutes.  (Or
maybe less?  Maybe it was only Jflash that was that bad?)

## `fastjtag`: using DBGTX/DBGRX/LDIC for better JTAG throughput

One thing to note is that boundary scan is *really* slow.  `fastjtag` was a
set of tools that were somewhat better.  The general flow was:

1. Use `bsrflash` to load a very small stub (`jtagrx`) onto PXA250 that
turned on the instruction cache and spun `DBGRX` to wait for an indication
that the next stage was ready.  This wasn't too painful, because the stub
was very small.

2. Use `miniload` to load a somewhat larger stub.  `minicache` downloaded a
stub called `memclient`, which used the `DBGRX` register to present a
word-at-a-time interface to allow host side programs to interact with
hardware attached to PXA250.  Also, `memclient` presented a higher-level
flash interface for various flash parts that could have been attached.

3. Use `rdevmem`, `rdump`, or `rflash` to load the contents of your choice
into flash.

Other bits are hazy.  For instance, `icacheboot` was an attempt to skip the
first step and have the PXA250 reset directly into icache.  I'm not sure if
it ever worked.

## `gtag`: a GTK+ JTAG interface

Hey, look, I discovered GLADE!

## `hamcop`: tools for iPAQ h2210 companion chip

iPAQ h2210 grew a companion chip, HAMCOP, that the system flash was attached
to.  In this directory is a first stage bootloader, `first-stage.S`; the
predecessor to that, which simply loads a chunk of code from the serial port
into DRAM and jumps to it, `debrick.S`, and a tool, `hamcop`, which provided
a handful of toys.  `hamcop` served two major roles:

1. Reverse engineering of the HAMCOP boundary scan chain.  You could set
individual bits of the chain high or low, or mechanically go searching
through the chain looking for a pin that you had your logic probe attached
to.

2. (later) Reading from and writing to attached flash on HAMCOP.  It could
scribble a small image to the boot sector of HAMCOP's flash -- perhaps,
`debrick.S`! -- which would be enough to get the system far enough to flash
something more major back on board.
