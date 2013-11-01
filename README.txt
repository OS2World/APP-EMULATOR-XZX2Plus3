This little utility will convert XZX-tape files and raw binaries to files
accepted by the Spectrum +3DOS. This is done by making a +3DOS Header either
from the XZX-tape header or from informations given by the user. If the file
contains a header, the informations found here will be used, if the files is
a raw binary the user must supply some additional informations.
The program is written in ANSI C, and compiled with GCC under Linux, porting
to other Unix platforms should therefor be No Problem, hopefully :)

Currently there is only support for type 0 and 3 =(BASIC & CODE) files, but
type 1 & 2, the array type, will soon be supported too.

usage: xzx2plus3 <file>

xzx2plus3 will by itself determine wheater the <file> is a XZX-tape file or 
it's a raw binary, and if it's a binary the user must tell the program what
type the +3DOS version should be, together with some other informations.
There will then be generated a 128 bytes +3DOS header which will be put
in front of <file>, an eventually XZX-tape header will of course be stripped
off, and <file> should now be as produced by the Spectrum +3/2A.
It can now be transfered to a +3 Disc, e.g using 22DISK if using a MS-DOS
machine.

NOTE! The <file> will no longer be a valid XZX-file.

Totally Freeware so use at your own risk.

Thomas A. Kjaer 04/94
email: takjaer@daimi.aau.dk
