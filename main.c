#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define KEYS 2
#define DEBUG 0

typedef struct data_node {       /* bundle data items together in a struct */
  char key[30];
  int freq;
} data_t;

/* has one more key and pointer to facilitate splits and finding the mid key */
typedef struct node {              /* 2-3 tree node */
  int num_keys;                    /* number of keys in the data array */
  data_t data[KEYS+1];             /* data array */
  struct node *tree_p[KEYS+1+1];   /* pointer array */
} tree_t;

tree_t * create_node () ;
void insert (tree_t **root, data_t val);
void concordance_file_write(tree_t *root);
void recur_insert (tree_t **root, data_t val, int *split, data_t *mid, tree_t **newp);
tree_t * search_23tree (tree_t *root, char word[30]);
void print_tree (tree_t *tree);
int recur_print (tree_t *tree, int level);
int total_nodes(tree_t *tree);
int keys(int num);
void inorder(tree_t *n, FILE *fp);
void tree_check(tree_t *root, char str[30]);

/*
 AUTHOR: Suchith Pinreddy, Yash Nikumb
 FILENAME: main.c
 SPECIFICATION: Counting the number of unique words, and the frequency of the words and putting it in a text file. 
 FOR: CS 2413 Data Structures Section 002
*/

int main()
{
    tree_t *root = NULL;
    data_t word;
    int count;
    char data[30];
    FILE *infile;
    infile = fopen("text.txt", "r");
    while(fscanf(infile, "%s", data)==1){
        for(int i = 0; i<=(strlen(data));i++){
            if (data[i]>=65 && data[i] <=90){
                data[i] = data[i] + 32;
            }
            if (!((data[strlen(data)-1]) >= 'a' && (data[strlen(data)-1]) <= 'z')){
                data[strlen(data)-1] = '\0';
            }
        }
        strncpy(word.key, data, 30);
        word.key[strlen(data)] = '\0';
        word.freq = 1;
        insert(&root, word);
    }
    //printf("%p", &root);
    print_tree (root);
    concordance_file_write(root);
    char str[30] = "in";
    char str1[30] = "shut";
    for(int j = 0; j<=(strlen(str)+1);j++){
        if (str[j]>=65 && str[j]<=90){
            str[j] = str[j] + 32;
        }
        if (!((str[strlen(str)-1]) >= 'a' && (str[strlen(str)-1]) <= 'z')){
                str[strlen(str)-1] = '\0';
            }
    }
    for(int k = 0; k<=strlen(str1);k++){
        if (str1[k]>=65 && str1[k]<=90){
            str1[k] = str1[k] + 32;
        }
        if (!((str1[strlen(str1)-1]) >= 'a' && (str1[strlen(str1)-1]) <= 'z')){
                str1[strlen(str1)-1] = '\0';
            }
    }    
    tree_check(root, str);
    tree_check(root, str1);
    count = total_nodes(root);
    return 0;
}

/* NAME: create_node 
 PARAMETERS: No parameters
 PURPOSE: create_node creates and initializes an empty tree node
 PRECONDITION: No parameteters 
 POSTCONDITION: It returns a pointer variable when we run a function 
*/ 

/* create_node creates and initializes an empty tree node */
tree_t * create_node () {
    tree_t *newp = (tree_t *) malloc (sizeof(tree_t));
    int i;
    for (i = 0; i < KEYS; i++) {
        newp->data[i].key[0] = '\0'; //initializing as null to the leaf structure |NULL|NULL|NULL| with ptr is null
        newp->data[i].freq = 1;
        newp->tree_p[i] = NULL; //ptr corressponding to left mid  null
    }
    newp->tree_p[i] = NULL; //making right ptr NULL?
    newp->num_keys = 0; //initializing whole node with null?
    return(newp); //return add
}

/* NAME: insert
 PARAMETERS: tree_t **root, data_t val
 PURPOSE: The 
 PRECONDITION: What should be true regarding the parameters and when the function can be called
 POSTCONDITION: What should be true after the function returns, such as variables changed or values returned
*/ /* NAME: print_tree
 PARAMETERS: tree_t *tree
 PURPOSE: The function trees 
 PRECONDITION: Actual variable should be a struct type
 POSTCONDITION: It should print the tree correctly 
*/ 

void insert (tree_t **root, data_t val) {
  data_t mid;
  int split;
  tree_t *newp;
  tree_t *ptr;
  int i;
  tree_t *rep = NULL;
  //data_t rep;
  // empty tree - create new root node and insert key
  if (*root == NULL) {
    if (DEBUG) printf("\ninsert - empty tree - val = %s freq = %d\n",val.key, val.freq);
    newp = create_node(); //address got
    newp->data[0] = val;
    newp->data[0].freq = 1;//putting data
    //printf("Freq is %d\n",newp->data[0].freq);
    newp->num_keys = 1;  //indication of key insertion
    *root = newp; //root is assigned an add
  }
  // non-empty tree - create a new root if a split occurred
  else {
    rep = search_23tree(*root,val.key);
    if (rep == NULL) {  //searches?
      split = 0; //where is split changing
      if (DEBUG) printf("\ninsert - non-empty tree before recur call - val = %s freq=%d\n",val.key,val.freq);
      recur_insert (root,val,&split,&mid,&ptr); //split val might change
      if (DEBUG) printf("insert - non-empty tree after recur call - val = %s freq = %d split = %d\n",val.key, val.freq,split);
      if (split) {
        newp = create_node(); //for new root
        newp->data[0] = mid;  //with mid
        newp->tree_p[0] = *root; //saving prev
        newp->tree_p[1] = ptr; //saving last or creating new node in first link?
        //newp->tree_p[2] = NULL;
        newp->num_keys = 1; //assignment of keys to 1 for root
        *root = newp; //root add changed
      }
    }
    else{
      for (int i = 0; i <3; i++){
        if (strncmp(val.key, rep->data[i].key,30)==0){
          rep->data[i].freq++;
        }
        else{
          continue;
        }
      }
    }
  }
} 

/* NAME: recur_insert
 PARAMETERS: tree_t **root, data_t val, int *split, data_t *mid, tree_t **newp
 PURPOSE: Inserts the links
 PRECONDITION: Should be a struct tupes and int value as given in formal parameter
 POSTCONDITION: It should correctly insert the links in the 2-3 search trees 
*/ 

void recur_insert (tree_t **root, data_t val, int *split, data_t *mid, tree_t **newp) {
  int i,j;
  tree_t *np;
  if ((*root)->tree_p[0] == NULL) {    // insert val in leaf node
    if (DEBUG) printf("recur_insert - insert in leaf - val = %s  (*root)->data[0].key = %s val.freq = %d\n",val.key,(*root)->data[0].key, val.freq);
    for (i = (*root)->num_keys; i > 0 && strncmp(val.key, (*root)->data[i-1].key, 30) < 0; i--) {
      (*root)->data[i] = (*root)->data[i-1];
      (*root)->tree_p[i+1] = (*root)->tree_p[i];
    }
    (*root)->data[i] = val;
    (*root)->tree_p[i+1] = NULL;
    (*root)->num_keys++;
    if (DEBUG) {
      printf("recur_insert - after leaf insert\n");
      for (j=0; j < (*root)->num_keys; j++) {
        printf("  %s\n",(*root)->data[j].key);
        printf("  %p\n",(*root)->tree_p[j]);
      }
      printf ("  %p\n",(*root)->tree_p[j]);
    }
    if ((*root)->num_keys == KEYS+1) {     // split leaf node
      if (DEBUG) printf("recur_insert - split leaf node\n");
      *split = 1;
      *newp = create_node ();
      (*newp)->data[0] = (*root)->data[KEYS];
      (*newp)->num_keys = 1;
      *mid = (*root)->data[1];
      (*root)->num_keys = 1;
    }
  }
  else {               // internal node
    if (DEBUG) printf("recur_insert - internal - before recur - val = %s val.freq = %d (*root)->data[0].key = %s\n",val.key, val.freq, (*root)->data[0].key);
    // determine which tree_p pointer to traverse
    for (i = 0; i < (*root)->num_keys && strncmp(val.key,(*root)->data[i].key,30)>0; i++);
    recur_insert (&((*root)->tree_p[i]), val, split, mid, newp);
    if (DEBUG) printf("recur_insert - internal - after recur - val = %s  val.freq = %d (*root)->data[0].key = %s  split = %d\n",val.key,val.freq, (*root)->data[0].key,*split);
    if (*split) {   // a split occurred on the call to recur_insert
      if (DEBUG) printf("recur_insert - splitting - mid = %s mid.freq  =  (*root)->num_keys = %d\n",(*mid).key,(*mid).freq,(*root)->num_keys);
      *split = 0;
      // insert *mid and *newp into **root
      for (i = (*root)->num_keys; i > 0 && strncmp(mid->key, (*root)->data[i-1].key,30) < 0; i--) {
      (*root)->data[i] = (*root)->data[i-1];
      (*root)->tree_p[i+1] = (*root)->tree_p[i];
      }
      (*root)->data[i] = *mid;
      (*root)->tree_p[i+1] = *newp;
      (*root)->num_keys++;
      if (DEBUG) {
        printf("recur_insert - after internal insert\n");
        for (j=0; j < (*root)->num_keys; j++) {
          printf("  %s \t %d\n",(*root)->data[j].key, (*root)->data[j].freq);
          printf("  %p\n",(*root)->tree_p[j]);
        }
        printf ("  %p\n",(*root)->tree_p[j]);
      }
      if ((*root)->num_keys == KEYS+1) {     // split internal node
        *split = 1;
        *newp = create_node ();
        (*newp)->data[0] = (*root)->data[KEYS];
        (*newp)->tree_p[0] = (*root)->tree_p[2];
        (*newp)->tree_p[1] = (*root)->tree_p[3];
        (*newp)->num_keys = 1;
        *mid = (*root)->data[1];
        (*root)->num_keys = 1;
      }
    }
    }
}

/* NAME: search_23tree 
 PARAMETERS: tree_t *root, char word[30]
 PURPOSE: It searches if there are any matches in words
 PRECONDITION: Should be a struct type and a array of char 
 POSTCONDITION: Should fin the no. of unique words and search it
*/ 

tree_t * search_23tree (tree_t *root, char word[30]) {
  tree_t *p = NULL;
  if (root != NULL) {
    int i;
    for (i = 0; i < root->num_keys && strncmp(word, root->data[i].key, 30) > 0; i++);  /* position i with respect to num */
    if (i < root->num_keys && strncmp(word, root->data[i].key, 30)==0)       /* check for num in root->data[i] */
      p = root;
    else
      p = search_23tree (root->tree_p[i],word);                /* keep searching down root->tree_p[i] */
  }
  return (p);
}
/* NAME: print_tree
 PARAMETERS: tree_t *tree
 PURPOSE: The function trees 
 PRECONDITION: Actual variable should be a struct type
 POSTCONDITION: It should print the tree correctly 
*/ 

void print_tree (tree_t *tree) {
  //printf("\nThe 2-3 tree is:\n");
  int x = recur_print(tree,0);
}

/* NAME: recur_print
 PARAMETERS: tree_t *tree, int level
 PURPOSE: The purpose of the following function is to print the level in the file 
 PRECONDITION: Should be a struct type and an integer 
 POSTCONDITION: Recursive function and should stop at some point of time.
*/ 

int recur_print (tree_t *tree, int level) {
  int i;
  if (tree != NULL) {
    for (i = 0; i < level; i++){
    }
    for (i = 0; i <= tree->num_keys; i++){
      recur_print (tree->tree_p[i],level+1);
      }
  }
  else{
    return level;
  }


}

/* NAME: inorder
 PARAMETERS: tree_t * n, FILE *fp
 PURPOSE: it traverses the file in ascending order. 
 PRECONDITION: Actual parameter pased should be a struct type and a FIlE variable. 
 POSTCONDITION: It prints the foolowing file in ascending order of words correctly 
*/ 
void inorder(tree_t * n, FILE *fp){
  if (n!= NULL){
    inorder(n->tree_p[0], fp);
    //printf("%s \t %d\n", n->data[0].key, n->data[0].freq);
    fprintf(fp, "%s \t %d \n", n->data[0].key, n->data[0].freq);
    inorder(n->tree_p[1], fp);
    if(n->num_keys == KEYS){
      fprintf(fp, "%s \t %d \n", n->data[0].key, n->data[0].freq);
      //printf("%s \t %d\n", n->data[1].key, n->data[1].freq);
      inorder(n->tree_p[2], fp);
      }
  }
} 

/* NAME: total_nodes
 PARAMETERS:tree_t *tree
 PURPOSE: It counts the total no. of nodes 
 PRECONDITION: Actual parameters should be a strcut type.
 POSTCONDITION: The function should return the total no. of nodes 
*/ 

int total_nodes(tree_t *tree){ 
  int n=0;

  if (tree==NULL){
    return 0;
  }
  else{
    n=n+1;
  }
		n+=total_nodes(tree->tree_p[0]);
		n+=total_nodes(tree->tree_p[1]);
    n += total_nodes(tree->tree_p[2]);
	return n;
}
/* NAME: concordance_file_write
 PARAMETERS:tree_t *root
 PURPOSE: It counts writes thee no. of unique words and the freqency 
 PRECONDITION: Actual parameters should be a strcut type.
 POSTCONDITION: writes the file in the concordonce and prints it. 
*/ 

void concordance_file_write(tree_t *root){
    FILE *outfile;
    int nodes;
    outfile = fopen("concordance.txt", "w");
    nodes = total_nodes(root);
    fprintf(outfile, "There are %d distinct words in the text file:\n", nodes);
    inorder(root,outfile);
    fclose(outfile);
}
/* NAME: concordance_file_write
 PARAMETERS:tree_t *root
 PURPOSE: print height and size of the tree
 PRECONDITION: Actual parameters should be a strcut type and a char.
 POSTCONDITION: prints the height with the help of the other functions defined  
*/ 
void tree_check(tree_t *root, char word[30]){
    tree_t *val;
    printf("Tree Check:\n");
    int height = recur_print(root,0);
    //height=height+1;
    printf("\tTree Height: %d\n", height);
    int nodes = total_nodes(root);
    printf("\tTree Size: %d nodes\n", nodes);
    val = search_23tree(root, word);
  if(val){
        printf("\t%s is found and as a frequency %d\n", word, val->data->freq);
  }
  else{
        printf("\t%s is not found\n",word);
    }
} 
/*  
Output: 


1. example text.txt – passed but failed to print the no. of keys correctly.
Tree Check:
        Tree Height: 2
        Tree Size: 12 nodes
        in is found and as a frequency 2
Tree Check:
        Tree Height: 2
        Tree Size: 12 nodes
        shut is not found
2. text.txt with one word – passed 
Tree Check:
        Tree Height: 1
        Tree Size: 1 nodes
        in is found and as a frequency 1
Tree Check:
        Tree Height: 1
        Tree Size: 1 nodes
        shut is not found
3. empty text.txt – passed 
Tree Check:
        Tree Height: 1
        Tree Size: 0 nodes
        in is not found
Tree Check:
        Tree Height: 1
        Tree Size: 0 nodes
        shut is not found
4. text.txt with > 500 total words – Partially passed  but failed no. of keys correctly 
Tree Check:
        Tree Height: 3
        Tree Size: 241 nodes
        in is found and as a frequency 11
Tree Check:
        Tree Height: 3
        Tree Size: 241 nodes
        shut is not found



Team Members: 1) Suchith Pinreddy: Implemeted the insert and search functions for the following problem. Debugged and modified the rest of the functions . 
              2) Yashowardhan Nikumb: Implemeted the struct nodes, printing the total number oof nodes and the print recur function in the following problem. Debugged and modified the insert and search function according to the main problem.
1. What would be the worst case for inserting n words into a 2-3 tree? 
Ans. The worst case scenario of insertig n words into a 2-3 tree is O(log n)

2. Big O of the best case for inserting n words into a 2-3 search tree
Ans. Big O of the best case scenario is when we are inserting the least no. of words in a 2-3 search tree i.e null. 
3. Big O of the storage requirements for 2-3 search tree with n keys 
Ans. Big O storage requirements for 2-3 search tree is linear i.e is O(n).
4. For the example text.txt file, the tree height is 3. What is the minimum possible height? 
Ans. The minimum possible height of the tree with tree height should be 1. 
*/ 

