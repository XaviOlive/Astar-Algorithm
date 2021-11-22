
// ---MACROS---
#define R 6371          // Earth radius (km)
#define pi 4*atan(1)

typedef char Queue; //New variable type: Queue indicating in each list (None, Open or Closed) is each node
enum whichQueue{N, O, C};


typedef struct { //Struct where information for looking for the path is stored = the status of the node
    double g; //Stores minimum distance computed to arribe to this node
    double h; //Stores the heuristic function value from the node to the node_goal
    double f; //f=g+h, and it used to indicate the preference (when it is minimum) for expanding this node respecto to the others
    unsigned long parent; //Stores the parent from which we reached this node with the minimum path
    Queue in_queue; // Indicates in each list is the node
} node_status;


typedef struct open_list_node { //Structure of a list
    unsigned long val; //The value indicates the current node
    struct open_list_node* next; //And then a pointer indicates which will be the next node of the list
} open_list_node;

double haversine_distance (node* nodes, unsigned long node1, unsigned long node2) { //it obtains the distance between two nodes in the surface of the Earth through haversian formula
    double lat1 = nodes[node1].lat * pi / 180.f;
    double lat2 = nodes[node2].lat * pi / 180.f;
    double diff_lat = (nodes[node2].lat - nodes[node1].lat) * pi / 180.f;
    double diff_lon = (nodes[node2].lon - nodes[node1].lon) * pi / 180.f;
    double a = sin(diff_lat/2) * sin(diff_lat/2) + cos(lat1) * cos(lat2) * sin(diff_lon/2) * sin(diff_lon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    double d = R * c;
    return d;
}

void enqueue(open_list_node *queue, unsigned long val, node_status* status) {//it allows to add a new element in the queue in the best position according to the minimum f of nodes.
    open_list_node* new_node;
    if ((new_node = (open_list_node*) malloc(sizeof(open_list_node))) == NULL) ExitError("when allocating memory for a new node in the OPEN list", 21);
    new_node->val = val;
    new_node->next = NULL;//Allocate memory and generate new node position in the list where val indicates the id of the node

    while (queue->next != NULL){
        if (status[new_node->val].f <= status[(queue->next)->val].f) break;
        queue = queue ->next; //Looking for the best position according to the minimum f.
    }
    if (queue->next != NULL) {new_node->next = queue->next;
        queue->next = new_node; } //In case position is intermediate we add the new node and we adequate the poiters of next node
    else {queue->next = new_node;} //In case of the last position we add the new node and we indicate the pointer to the last final node
}

void AStar (node* nodes, unsigned long node_start, unsigned long node_goal, node_status* status) { //Astar loop function
    
    unsigned long node_current, node_successor;
    unsigned long open_elements;
    open_list_node* OPEN = NULL;
    open_list_node* PROV = NULL;
    int successor_num;
    double successor_current_cost;
    double w;
    
    //Store information of the OPEN list
    if ((OPEN = (open_list_node*) malloc(sizeof(open_list_node))) == NULL) ExitError("when allocating memory for the OPEN list", 22);
    
    //Actuallizaton of all the information for the first node
    status[node_start].in_queue = 1;
    status[node_start].g = 0;
    status[node_start].h = haversine_distance(nodes, node_start, node_goal);
    status[node_start].f = status[node_start].h;
    open_elements = 1; //Indicate in which list is node_start
    OPEN->val = node_start; //Put node_start in the OPEN list
    
    printf("Preloop\n\n\n");
    //While loop starting:
    while (open_elements != 0) {
        node_current = OPEN->val; //Taking from open list the node with the lowest f (classification is done when adding a node to the list)

        if (node_current == node_goal) {
            printf("We have found the solution (node: %lu).\n\n", nodes[node_current].id);
            break; //Finishing the loop for case we reach the solution
        }
        
        for (successor_num = 0; successor_num < nodes[node_current].nsucc; successor_num++) {//for each successor for the node:
            node_successor = (nodes[node_current].successors)[successor_num];
            w = haversine_distance( nodes, node_current, node_successor );
            successor_current_cost = status[node_current].g + w; //Setting the provisional successor current cost
            
            if (status[node_successor].in_queue == 1) {
                if (status[node_successor].g <= successor_current_cost) continue; //When the provision current cost is hihger that the previous one, no changes are done and next successor is called.
                status[node_successor].g = successor_current_cost;
                status[node_successor].f = status[node_successor].g + status[node_successor].h; //Otherwise new g and f are computed
            }
            else if (status[node_successor].in_queue == 2) {
                if (status[node_successor].g <= successor_current_cost) continue; //When the provision current cost is hihger that the previous one, no changes are done and next successor is called.
                open_elements++; //Actualization of the dimension of the open list
                status[node_successor].in_queue = 1; //Actualization of the node list indicator (successor)
                status[node_successor].g = successor_current_cost;
                status[node_successor].f = status[node_successor].g + status[node_successor].h; //New g and f are computed
                enqueue(OPEN, node_successor, status); //Addition of the node to the OPEN list in the indicated position (with f value actualized)
            }
            else {
                status[node_successor].in_queue = 1; //Actualization of the node list indicator (successor)
                open_elements++; //Actualization of the dimension of the open list
                status[node_successor].h = haversine_distance( nodes, node_successor, node_goal ); //Calculation of h
                status[node_successor].g = successor_current_cost;
                status[node_successor].f = status[node_successor].g + status[node_successor].h; //New g and f are computed
                enqueue(OPEN, node_successor, status); //Addition of the node to the OPEN list in the indicated position (with f value actualized)
            }

            status[node_successor].parent = node_current; //Setting the new parent
        }
        
        status[node_current].in_queue = 2; //Actualization of the node list indicator (current) once expanded
        open_elements--; //Actualization of the dimension of the open list
        PROV = OPEN->next;   free(OPEN);     OPEN = PROV; //Removing current node from OPEN list and freeing its space.
    }
    
    printf("While done\n\n\n");
    
    if (open_elements == 0) ExitError("OPEN list is empty before reaching destination", 23);
    
    while (OPEN != NULL) {                                                             
        PROV = OPEN->next;   free(OPEN);     OPEN = PROV; //Removing all nodes of the OPEN list and freeing their space.
    }

}

unsigned long output_path(node* nodes, unsigned long node_start, unsigned long node_goal, node_status* status) {
    FILE *fout;
    char name[257];
    unsigned long path_len = 0;
    unsigned long son_node = node_goal;
    
    while (son_node != node_start) { //Count the number of nodes reached for getting the minimum distance
        path_len++;
        son_node = status[son_node].parent;
    }
    path_len++;
    
    unsigned long path[path_len]; //Generation of the path vector of the dimensions calculated
    
    son_node = node_goal;
    for (signed long i = path_len-1; i >= 0; i--) { //Storing the nodes reached in order in each position of the vector
        path[i] = son_node;
        son_node = status[son_node].parent;
    }

    strcpy(name, "path.csv"); //Generation of the path FILE output
    if ((fout = fopen (name, "w+")) == NULL) ExitError("when creating the path.csv file", 24);
    fprintf(fout, "step|node_id|latitude|longitude|distance_acc\n");
    for (unsigned long i = 0; i < path_len; i++) {
        fprintf(fout, "%lu|%lu|%.7f|%.7f|%.7f\n", i, nodes[path[i]].id, nodes[path[i]].lat, nodes[path[i]].lon, status[path[i]].g);
    }
    fclose(fout);
    return path_len;
}

