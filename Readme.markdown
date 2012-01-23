# PAMQuickLookPlugin

Apple QuickLook (http://wikipedia.org/wiki/Quick_Look/) plugin for viewing Netpbm (http://netpbm.sourceforge.net/) image files.

PGM, PPM and PAM files with 8-bit and 16-bit samples are supported.

To install plugin build PAMQuickLookPlugIn.qlgenerator and copy it into /Library/QuickLook to make it available for or users (administrator rights are required), or into ~/Library/QuickLook to make it available for current user.

Plugin currently supports next formats:

* PBM files: Only ASCII mode (P1) is currently supported. Binary mode (P3) is nt supported yet.

* PGM files: Both ASCII (P2) and Binary (P4) modes are supported.

* PPM files: Both ASCII (P3) and Binary (P6) modes are supported.

* PAM files: BLACKANDWHITE and BLACKANDWHITE_ALPHA modes are not implemented.

## License

BSD-style license. See License.txt

## TODO

* Implement support for binary black-and-white images (1-bit).

* Scale down very large images to reduce preview generation time.

* Test on more images.
