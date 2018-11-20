# mosaicgenerator

In this application, I created a way for a mosaic to be created using several other pictures. Although this seems simple, 
at the core, the problem is much tougher since I had to use structs for the individual pixels and PNGs that I was working 
with. Currently, the program takes a directory of images, and uses a nearest neighbor algorithm to determine the average 
color of the entire PNG, and determine a useful space within the source image (if any) where the 'tile' picture can be 
implemented. 
