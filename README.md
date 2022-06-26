# Manga-dl
A downloader and converter for manga.
###### Right now manga-dl is a very quick and dirty solution, and is in heavy development

## Works with
 - mangapark.net
 - More coming

## Why
I like reading manga, and I can't when I don't have WI-FI, this allows me to do so without it.

## How
1. Downloads the page given by the url to a temp folder.
2. Gets the links to all the chapters.
3. Proceeds to eat bandwidth and CPU to use a library called selenium in python (ew), to click a singular on the page, then finds all of the links to the images and adds them to a vector
4. Downloads each of the images to a temp folder
5. converts the images to a pdf

## Dependencies
 - python
 - python selenium
 - wget
 - image-magick

## TODO
 - Get rid of python: it's slow. If selenium is still needed, will probably go to java
 - Add caching: it sucks when the program stops, and it needs to rerun everything again.
 - Rework threading: it's a mess.