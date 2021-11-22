#include "writelib.h"
#include "Astarlib.h"

int main (int argc, char *argv[]) {
    
    //DECLARATION OF VARIABLES:
    unsigned long nnodes, ntotnsucc;
    unsigned long* allsuccessors;
    node* nodes;
    node_status* status = NULL;
    unsigned long origin, dest;
    clock_t start, end;
    unsigned long path_len;
    
    //Opening spain.bin file for reading
    FILE *fin;
    if ((fin = fopen ("spain.bin", "rb")) == NULL)
        ExitError("the data file does not exist or cannot be opened", 11);
    
    printf("\n\nReading binary file ...\n\n");
    
    // Global data --- header
    if( fread(&nnodes, sizeof(unsigned long), 1, fin) +
        fread(&ntotnsucc, sizeof(unsigned long), 1, fin) != 2 )
            ExitError("when reading the header of the binary data file", 12);
    
    // Getting memory for all data
    if ((nodes = (node*) malloc(nnodes*sizeof(node))) == NULL)
        ExitError("when allocating memory for the nodes vector", 13);
    if ((allsuccessors = (unsigned long*) malloc(ntotnsucc*sizeof(unsigned long))) == NULL)
        ExitError("when allocating memory for the edges vector", 14);
    
    
    // Reading all data from file
    if( fread(nodes, sizeof(node), nnodes, fin) != nnodes )
        ExitError("when reading nodes from the binary data file", 15);
    if( fread(allsuccessors, sizeof(unsigned long), ntotnsucc, fin) != ntotnsucc)
        ExitError("when reading sucessors from the binary data file", 16);
    fclose(fin);
    
    // Setting pointers to successors and copying the names of all the nodes
    for (long i = 0; i < nnodes; i++) {
        if(nodes[i].nsucc) {
            nodes[i].successors = allsuccessors;
            allsuccessors += nodes[i].nsucc;
        }
    }
    
    //Allocating memory for the satatus node vector
    if ((status = (node_status*) malloc(nnodes*sizeof(node_status))) == NULL) ExitError("when allocating memory for the status vector", 17);
    
    //
    for (long i = 0; i < nnodes; i++) {
        status[i].in_queue = 0;                                                             // all nodes start neither in OPEN nor CLOSED list
        status[i].g = INFINITY;
        status[i].f = INFINITY; // all nodes start with INF g and f function
    }
    
    //SETING of the specific places (initial node and final node):
    origin = (unsigned long)b_search(nodes, 240949599, 0, nnodes-1);
    dest = (unsigned long)b_search(nodes, 195977239, 0, nnodes-1);
    printf("Route to calculate:\n");
    printf("  - From: Santa Maria del Mar, Barcelona (node: 240949599) -- latitude %f -- longitude %f\n dest_node: %lu\n", nodes[origin].lat, nodes[origin].lon, origin);
    printf("  - To: Giralda, Sevilla (node: 195977239) -- latitude %f -- longitude %f\n origin_node: %lu\n\n", nodes[dest].lat, nodes[dest].lon, dest);
    
    
    printf("\nRunning A* ...\n\n");
    start = clock();
    AStar(nodes, origin, dest, status); //Astar implemetation
    end = clock(); //Time counter
    printf("\nGenerating path file ...\n\n");
    path_len = output_path(nodes, origin, dest, status); //Generating path and path file
    printf("\nTASK COMPLETED\n\n");
    
    printf("\nRESUME:\n");
    printf("  - The found minimum distance is: %.5f km. \n", status[dest].g);
    printf("  - The needed time was %.6fs. \n", ((double) end-start)/CLOCKS_PER_SEC);
    printf("  - The path was done by passing for %lu nodes.\n\n", path_len);

    
    //Free all allocated memory
    free(status);
    free(nodes);
    allsuccessors -= ntotnsucc;
    free(allsuccessors);
            
    return 0;
}
