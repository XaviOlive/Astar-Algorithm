# A* Assigment

This program is thought as application to specifically calculate the shortest path between the Basilica de Santa Maria del Mar in Barcelona (id: 240949599) to Giralda (Calle Mateos Gago) in Sevilla (id: 195977239) with the A* algorithm.

## Files

The folder sent contains several files used in the execution of the assigment: `write_main.c` program that was used to update the nodes of the graph used and to writte the binary file previously used; `Astar_main.c` program that was used to read the binary file and to compute the final path using A* algorithm; the headers `writelib.h` and `Astarlib.h`  which contain the standard C libraries, macros and the functions in needed in .c files and the `Report` explaining the results accuired.  
`data` folder contains data used in the report is stored.
`write` and `astar` executables are also included for if needed.

## Usage

`write_main.c` and `Astar_main.c`  must be first compiled. gcc compiler is recomended with -Ofast or -O3 optimizer for both files.

For execution, `write` executable must be executed first
NOTE: it is necessary to add the file "spain.csv" to the unzipped folder before executing. This file can be downloaded with the link: http://mat.uab.cat/~alseda/MasterOpt/spain.csv.zip
Then, `astar`  executable can be executed to obtain the results.

## Authors
@author: Adrià Colás Gallardo.        NIU: 1419956
@author: Xavier Olivé Fernández.    NIU: 1394407

