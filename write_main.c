#include "writelib.h"

int main (void) {
    
    //DECLARATION OF VARIABLES:
    FILE *fin;
    FILE *fin2;
    unsigned long nnodes = 23895681UL;
    char name[257];
    
    unsigned short* nsuccdim;
    long int i = 0;
    node *nodes; //Nodes vector
    
    char* buffer = NULL;
    size_t bufsize = 0;
    size_t line = 0;
    
    //Opening spain.csv file
    if ((fin = fopen("spain.csv", "r")) == NULL) ExitError("when opening spain.csv file", 1);
    
    //Allocate space for nodes vector and from the auxiliar nsuccdim vector
    if((nodes = (node *) malloc(nnodes*sizeof(node))) == NULL) ExitError(" when allocating memory for the nodes vector", 2);
    if ((nsuccdim = (unsigned short*) calloc(nnodes, sizeof(unsigned short))) == NULL) ExitError(" when allocating memory for the nsuccdim vector", 3);
        
    printf("\n Updating nodes ...\n\n");
    
    while (line >= 0) { //Updating basic information to nodes vector and counting the number of successors for each node
        line = getline(&buffer, &bufsize, fin);             // read next line skipping the already read
        if (*(buffer) == '#') continue;                     // if it is a comment: discart
        else if (*(buffer) == 'n') {                        // if it is a node: Process the node and move on to another line
            node_information(buffer, nodes, i);
            i += 1; continue; }
        else if (*(buffer) == 'w') {
            num_succ_calc(nsuccdim, buffer, nodes, nnodes); // if it is a way: update nsuccdim from info of the way line and move on to another line
            continue; }
        else break;                  // when all nodes and ways jave been read, finish the loop.
    }
    
    
    for (i = 0; i < nnodes; i++) { //Allocate space for succesors list in each node form nodes vector
        nodes[i].nsucc = nsuccdim[i];
        if((nodes[i].successors = (unsigned long*) malloc(nsuccdim[i]*sizeof(unsigned long))) == NULL) ExitError("when allocating memory for the successors nodes", 3);
        nsuccdim[i]=0;
    }
    
    
    fseek(fin, 0, SEEK_SET); //Reset getline function to start again at the begining
    while (line >= 0) {
        line = getline(&buffer, &bufsize, fin);                 // read next line skipping the already read
        if (*(buffer) == '#') continue;                         // if it is a comment: discart
        else if (*(buffer) == 'n') continue;                    // if it is a node: discard
        else if (*(buffer) == 'w') {
            set_sons(nsuccdim, buffer, nodes, nnodes);          // if it is way: update successors and move on to another line
            continue;
        }
        else break;                        // when all nodes and ways jave been read, finish the loop
    }
    

    printf("\n Writing binary file ...\n\n");
    
// Computing the total number of successors
    unsigned long ntotnsucc = 0UL;
    for(i = 0; i < nnodes; i++) ntotnsucc += nodes[i].nsucc;
       
// Setting the name of the binary file
    strcpy(name, "spain.bin");
    if ((fin2 = fopen (name, "wb")) == NULL)
        ExitError("the output binary data file cannot be opened", 4);
    
// Global data --- header
    if( fwrite(&nnodes, sizeof(unsigned long), 1, fin2) +
        fwrite(&ntotnsucc, sizeof(unsigned long), 1, fin2) != 2 )
            ExitError("when initializing the output binary data file", 5);

// Writing all nodes
    if( fwrite(nodes, sizeof(node), nnodes, fin2) != nnodes )
        ExitError("when writing nodes to the output binary data file", 6); //fin2 escrit
       
// Writing sucessors in blocks
    for(i = 0; i < nnodes; i++) if(nodes[i].nsucc) {
            if ( fwrite(nodes[i].successors, sizeof(unsigned long), nodes[i].nsucc, fin2) != nodes[i].nsucc )
                ExitError("when writing edges to the output binary data file", 7);
    }
    
    //Closde files
    fclose(fin);
    fclose(fin2);
    
    //Free memory allocated
    for (i = 0; i < nnodes; i++) { free(nodes[i].successors); }
    free(buffer);
    free(nodes);
    free(nsuccdim);
    
    printf("\n TASK COMPLETED\n\n");
    
    return 0;
}


