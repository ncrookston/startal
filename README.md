##STARTAL

Welcome to the source of SyntheTic Aperture RADAR Tool and Libraries.  This software was created to fulfill the requirements for [my Master's project](http://digitalcommons.usu.edu/gradreports/112/) at Utah State University.  I hope it's generally useful and I'm happy to answer questions.

While only tested on Windows, I have made efforts to ensure that the code is able to compile on linux.  I anticipate that few things will need significant changes to compile on a variety of platforms.
Required packages (not included) are:
 * [boost](boost.org)
 * [CMake](cmake.org)
 * [Django](https://www.djangoproject.com/)
 * [FFTW](http://fftw.org)
 * [GeographicLib](http://geographiclib.sourceforge.net/) 
 * [libjpeg](http://libjpeg.sourceforge.net/)
 * [zlib](http://www.zlib.net/)

As always, take care on MS Windows to ensure that the CRT matches for all compiled libraries.  Check `functions.cmake` in the Sarry folder to see where paths to assorted libraries are currently expected, but feel free to edit this to suit your needs.  Then compile `Sarry` by running cmake (as an out-of-source build).  Copy `Sarry.exe` into the `Startal/Sarry` directory, and the shared libraries into `Startal/Sarry/dlls`.

Install Django (last tested with 1.3.0) as instructed on their website.  Start the server on the port ofyour choice, and navigate to it.  You should be able to start selecting areas of interest.  Unfortunately, I am unable to provide my implementation of convolutive back-projection, but it is reasonably straightforward to implement using the primitives.  Likewise, I can provide neither data nor
format information for the particular SAR sensor this system was validated with.  The primitives to simplify creating Antenna sublcasses are present and require little additional code to parse raw IQ formats.
