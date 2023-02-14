# FSA
[File of Stream Array,C/C++ libraly](http://www.tinyforest.jp/oss/fsa_en.html)

The C/C++ standard functions open()/read()/write()/close() can read and write any file opened in a single stream image.
FSA offers simillar functions with much flexible I/O capabilities(stream array).
Specifying the stream index,the user of FSA can handle multiple streams in a single file.
Each stream is completely independent and has its own stream size.

For more informations and usage of FSA,refer to fsa_en.html(English) or fsa_jp.html(Japanese) on your browser.

This software can be redistributed under GNU Lesser General Public License.

To build this software on Linux => see makefile    
To build this software on Windows => create Visual studio solution,and add source files.  
To use this library => include fsa.h in your source codes(see tans.c) and link appropriate library files.
This software has been tested on Windows-10(32-bit & 64-bit) and Linux(32-bit CentOS-5 & 64-bit CentOS-7). 
