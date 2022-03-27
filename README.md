# SFML Automata 2D

<img src="https://github.com/ndrake127/sfml-automata-2d/blob/master/resources/demo.gif" width="50%">

<hr style="border:2px solid gray"> </hr>

A 2D cellular automaton simulator displayed using SFML with parallelism. Currently using Conway's Game of Life rules.

Depends on SFML and tclap, which can be automatically installed using the included installdeps.sh file. This likely needs to be run as root.

<hr style="border:2px solid gray"> </hr>

To generate an gif called out.gif in the root directory, run:

rm png/\*  
bin/prog --png 1  
./joinpng  
