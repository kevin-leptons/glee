GLEE
====

        Stupid media file player.

        Kevin Leptons <kevin.leptons@gmail.com>
        CC by 4.0 License
        Oct, 2017

FEATURES
========

        * Command line tools
        * Cross platform
        * Scan audio and video file in specific directories
        * Search indexed file by keyword and page number
        * Play file by identity or by keyword
        * Core library for anybody who wants to implement GUI tools

NOTES
=====

        <glee> base on C++11 and Boost library, and as theory, <glee> should
        work on multi-platform which supports C++11 and C++ compiler. However,
        <glee> was tested on Debian 9 only. Building and packing in other
        platforms such as Windows or Mac, or other GNU/Linux is possible, just
        do follow DEVELOPMENT section.

INSTALL
=======

        Here is pre-build for Debian 9, pick up an version and download it

                https://drive.google.com/open?id=0B6Eqm2oY7b1vdjMycUQxVUZpQk0

        Also download Boost library because Boost version in Debian 9
        repository is not match with <glee>

                https://drive.google.com/open?id=0B6Eqm2oY7b1vTmpHNzNZTS1kcFU

        Installing

                $ sudo dpkg -i libboost_1.65.1_all.deb
                $ sudo dpkg -i glee_0.1.0_all.deb

        If you are not in Debian 9, build <glee> from source in DEVELOPMENT
        section.

TUTORIAL
========

        Assume that you have directory contains media files as below:

                |- /mnt/resources/sound
                |       |
                |       |_ u_cant_touch_this.mc_hammer.mp3
                |       |- ice_ice_baby.vanila_ice.flac
                |       |- despacito.luis_fonsi_ft_daddy_yankee.mp4
                |       |- ...
                |- ...

        Add directory and scan it 

                $ glee_dir add /mnt/resources/sound
                $ glee_dir list
                #1 /mnt/resources/sound
                $ glee_dir scan 1
                Scan /mnt/resources/sound ...

        Show files which indexed

                $ glee_file list
                #1 u_cant_touch_this.mc_hammer.mp3
                #2 ice_ice_baby.vanila_ice.flac
                #3 despacito.luis_fonsi_ft_daddy_yankee.mp4
                ...

        Or search files by keyword an page number

                $ glee_file list -k hammer -p 1
                #1 u_cant_touch_this.mc_hammer.mp3

        Play media file

                $ glee_file play 1
                Play: #1 u_cant_touch_this.mp3

        If your directory contains many files, try play by keyword

                $ glee_master play -k mc_hammer
                #1 u_cant_touch_this.mp3

        For full Usage Document, see ./doc/usage.txt

DEVELOPMENT
==========

        See ./doc/dev.txt

REFERENCES
==========

        C++11 standard
        https://en.wikipedia.org/wiki/C%2B%2B11

        Boost library v1.65.1
        http://www.boost.org/doc/libs/1_65_1/
