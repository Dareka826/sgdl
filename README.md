# Simple Gelbooru Downloader

A program that I made to download all pictures from a specified tag or id/ids, because I couldn't find a way to get multiple pages by tag with the official API

You should probably use [mikf/gallery-dl](https://github.com/mikf/gallery-dl) instead

# Requirements
- libcurl
- regex

# Building

This project uses redo as its build system

Run `redo` to build

# TODO
* [x] get_image
* [x] command line options
* [x] get_by_tags
* [x] implement downloading to files
* [ ] add percent completion display
* [ ] circumvent page limit by limiting id range
* [ ] add verbose mode
* [ ] threads

