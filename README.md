# jackmeter

A tool to do simultaneous peak detection on multiple [Jack](https://jackaudio.org/) ports

# Challenge

`jackmeter` is part of the suite of tools we use to determine what might have
caused a bug in our software. It should give you a small window into our code
base.

As a small coding challenge we would like you to modify this application in some
way, for example:

* [ ] Simulate Jack by [reading from a `.wav` file](https://github.com/libsndfile/libsndfile).
* [ ] [Digital peak programme meter](https://en.wikipedia.org/wiki/Peak_programme_meter#/media/File:PPM_IEC_268-10_I_DIN.jpg) style output.
* [ ] Frequency display mode.
* [ ] Discontinous signal detection.
* [ ] Add some tests.
* [ ] Graphical UI.

But if you have some other idea, feel free to do that instead. You are also
welcome to take on more than one modification, but we don't expect you to spend
more than a couple of hours on this.

You are also encouraged to think about what you would have done differently had
you been the one to write this tool.

## Usage

```bash
$ jackmeter -h
Usage: jackmeter [-h] [-c] [-p] [-x] [-f <freq>] [-d <seconds>] [-n <measurements> ] [<port>, ...]

OPTIONS
-f      is how often to update the meter per second [8]
-d      duration seconds [infinite]
-n      n measurements [infinite]
-c      monitor only connected output ports [false]
-p      plain output [false]
-x      don't do pattern matching and don't sort<port>  the port(s) to monitor,
        partial matching and with regex support if no ports are given, displays
        all ports.
```

Example output:

```bash
:~/$ jackmeter -f 4 -p MYCLIENT1 MCLIENT2
# jackmeter 2022-03-07t11:13:47
MYCLIENT1:out_1 :   -8.8 db  (min: -24.0, max:  -4.4)
MYCLIENT1:out_2 :   -9.5 db  (min: -20.7, max:  -3.8)
MCLIENT2:out_1 :  -76.4 db  (min: -89.9, max: -71.6)
MCLIENT2:out_2 :  -76.9 db  (min: -88.4, max: -71.7)
MCLIENT2:out_3 :  -84.0 db  (min: -96.3, max: -78.4)
MCLIENT2:out_5 :  -95.5 db  (min:-102.3, max: -83.2)
MCLIENT2:out_6 :  -94.4 db  (min:-104.8, max: -83.9)
MCLIENT2:out_9 :  -90.7 db  (min: -99.5, max: -82.3)
MCLIENT2:out_10 :  -89.8 db  (min: -99.8, max: -82.4)
MCLIENT2:out_11 :  -99.7 db  (min:-108.4, max: -92.4)
MCLIENT2:out_12 : -100.6 db  (min:-108.4, max: -92.8)
MCLIENT2:out_16 :  -14.8 db  (min: -30.0, max: -10.4)
MCLIENT2:out_17 :  -15.5 db  (min: -26.7, max:  -9.8)
```

## Getting started

The build setup requires a 64-bit PC running [Ubuntu](http://www.ubuntu.com) or
similar; WSL on Windows should also work.

1. Install [Visual Studio Code](https://code.visualstudio.com/download)
2. Install Docker, e.g. `sudo apt install docker.io`; for WSL follow
   [this guide](https://docs.microsoft.com/en-us/windows/wsl/tutorials/wsl-containers).
3. Install the `Dev Containers` extension pack
4. Clone this repository.
5. Open the `jackmeter` folder in VS Code.
6. When asked to re-open in a container, do so.
7. Run `CTRL + SHIFT + B` from within VS Code or use `scripts/build.sh` to build
   `jackmeter`.
