StarBrilliant's LADSPA audio plugins
====================================

These are some of my LADSPA audio plugins

Installation
------------

```bash
git clone git://github.com/m13253/sb-ladspa-plugins.git
cd src
make
sudo make install LADSPADIR=/usr/local/lib/ladspa
```
Alternatively you can install these plugins to `/usr/lib/ladspa`.

You can also install them to `$HOME/.ladspa`, or install them to another
directory and then set environment variable `$LADSPA` to that directory, if you
do not have root permission.

## Extrastereo

(Linearly) increases the difference between left and right channels which adds
some sort of "live" effect to playback.

Inspired by an MPlayer filter.

Technically, this filter sets `L=avg+factor*(L-avg); R=avg+factor*(R-avg)`,
where `avg=(L+R)/2`.

Note that extreme parameters may cause loud volume. It may be useful to insert
a gain filter to lower the volume before extrastereo.

**Factor:** Sets the difference coefficient (default: 2.5). 0 means mono sound
(average of both channels), with 1 sound will be unchanged, with -1 left and
right channels will be swapped.

You may also apply this filter along with a reverb filter.

## Earwax

Makes stereo audio recorded for loudspeakers easier to listen to on binaural
headphones. You will feel a stereo audio recorded for loudspeakers sounds like
coming from inside your head. This filter can move the stereo image from inside
your head to somewhere far away in front of you.

Inspired by a SoX filter, but this filter does better than that one.

Note that this filter may make audio worse if the original audio is not recorded
for loudspeakers, or you are not using a binaural headphone.

Technically, this filter emulates two virtual loudspeakers and calculates the
distances between your ears and the loudspeakers. A weaker sound with a slight
delay will be transferred from the loudspeaker at the other side.

**Loudspeaker distance in meters:** The linear distance from you to the virtual
loudspeaker.

**Loudspeaker angle:** The angle between the loudspeaker and the
straightforward direction. Positive means away from center, negative means
close to center.

**Dry/Wet:** The strength of this filter. Generally you want to set it to 1.

You may also apply this filter along with the BS2B filter.

License
-------

All these plugins are licensed under LGPL version 3. Absolutely no warranty is
provided, so that you use my programs at your own risk. For more information on
LGPL license, see the `COPYING` file packaged along with these programs.

Some other things
-----------------

I have not yet requested a LADSPA UniqueID. If you have another plugin with the
same LADSPA UniqueID bad things may happen. I will request one as soon as I
finish debugging and testing.
