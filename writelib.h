// -- LIBRARIES ---
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>


// structure to represent a node
typedef struct {
    unsigned long id;           // Node identification
    char* name;
    double lat, lon;            // Node position
    unsigned short nsucc;       // Number of node successors; i. e. length of successors
    unsigned long* successors;  // List of successors
} node;



void ExitError(const char *miss, int errcode) { //Error functions used in the codes
    fprintf (stderr, "\nERROR: %s.\nStopping...\n\n", miss); exit(errcode);
}

signed long b_search(node* nodes, unsigned long id, unsigned long first_node, unsigned long last_node) { //Binary search function
    if ( nodes[first_node].id == id ) return first_node; // to know if is first_node
    if ( nodes[last_node].id == id ) return last_node; // to know if it is last_node
    unsigned long node_num;
    while ( last_node >= first_node ) {
        node_num = (last_node + first_node)/2; // to update the node id for every step
        if (nodes[node_num].id == id) return node_num;
        else if (nodes[node_num].id > id) last_node = node_num-1;
        else if (nodes[node_num].id < id) first_node = node_num+1;
    }
    return -1;  // return -1 if element is not in vector
}

void node_information(char* line, node* nodes, long int i) { // Take from node lines: id, lat and  lon, and stores it in the nodes vector in i position
    char* field;
    int namelength;
    unsigned short field_count; // counter of the number of fields
    for (field_count = 1; field_count < 12; field_count++) {
        field = strsep(&line, "|"); // for each count: get field selected
        if (field_count == 2) nodes[i].id = strtoul(field, (char **)NULL, 10); // get node id
        else if (field_count == 3) { //get the name of the node (most nodes don't have name)
            namelength = strlen(field);
            nodes[i].name = (char*) malloc(namelength + 1);
            strcpy( nodes[i].name, field );
        }
        else if (field_count == 10) nodes[i].lat = atof(field); // get latitude of each node
        else if (field_count == 11) nodes[i].lon = atof(field); // get longitude of each node
}
}

void num_succ_calc(unsigned short* nsuccdim, char* line, node* nodes, unsigned long nnodes) { //Calculus of nsuccdim for each node with the information of the way lines
    int way = 2;
    char* field;
    char* node_i;
    long int parent_id, son_id;
    long int parent_n, son_n;
    
    for (int j = 1; j < 9; j++) field = strsep(&line, "|");
    if (*field == 'o') way = 1; //check if the path between two nodes is in one way or in two ways.
    field = strsep(&line, "|");
    
    node_i = strsep(&line, "|"); // getting first member_node
    parent_id = strtoul(node_i, (char **)NULL, 10); //to read the node as unsigned long variable instead of char variable
    parent_n = b_search(nodes, parent_id, 0, nnodes-1); // to look if node is in the node's vector and assign it the id.
    
    while ((node_i = strsep(&line, "|")) != NULL) { // getting the rest of member_node
        son_id = strtoul(node_i, (char **)NULL, 10); //to read the node as unsigned long variable instead of char variable
        son_n = b_search(nodes, son_id, 0, nnodes-1); // to look if node is in the node's vector and assign it the id.
        if (way == 1) nsuccdim[parent_n] += 1;
        else { nsuccdim[parent_n] += 1;
            nsuccdim[son_n] += 1; } // count the number of successors for the node for 1 way or 2 ways
        parent_n = son_n; //Repeat the process for the two follogin nodes in the row
    }
}

void set_sons(unsigned short* counters, char* line, node* nodes, unsigned long nnodes) { //to write the adjency son's list of each node
    int way = 2;
    char* field;
    char* node_i;
    long int parent_id, son_id;
    long int parent_n = -1, son_n;
    int p_succ_num, s_succ_num;
    
    for (int j = 1; j < 9; j++) field = strsep(&line, "|");
    if (*field == 'o') way = 1; //check if the path between two nodes is in one way or in two ways.
    field = strsep(&line, "|");
    
    while ((parent_n == -1) && ((node_i = strsep(&line, "|")) != NULL)) { // getting first member_node
        parent_id = strtoul(node_i, (char **)NULL, 10); //to read the node as unsigned long variable instead of char variable
        parent_n = b_search(nodes, parent_id, 0, nnodes-1); // to look if node is in the node's vector and assign it the id.
    }

    while ((node_i = strsep(&line, "|")) != NULL) { // getting the rest of member_node
        son_id = strtoul(node_i, (char **)NULL, 10); //to read the node as unsigned long variable instead of char variable
        son_n = b_search(nodes, son_id, 0, nnodes-1); // to look if node is in the node's vector and assign it the id.
        if (son_n == -1) continue; //check point to avoiding errors
        p_succ_num = counters[parent_n]; // to get parent's index node in the node's successors counter
        s_succ_num = counters[son_n]; // to get son's index node in the node's successors counter
        if (way == 1) { (nodes[parent_n].successors)[p_succ_num] = son_n;  counters[parent_n] += 1; }
        else {
            (nodes[parent_n].successors)[p_succ_num] = son_n;
            (nodes[son_n].successors)[s_succ_num] = parent_n;
            counters[parent_n] += 1; counters[son_n] += 1; // to put the node's vector index into the node's successor list and increase the position of node's counter[i] for the next node
        }
        parent_n = son_n; //Repeat the process for the two follogin nodes in the row
    }
}
