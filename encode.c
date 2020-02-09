/* 
* encode.c
* Caeli Massey
* Last edited 5 December 2018
*
* This program will read in a text file (either redirected in the command line or entered as
* a parameter at execution), make a huffman tree of its characters, and print a compressed 
* encoding of it in the specified output (again, output can be specified through a second
* parameter or redirection). */
#include "code.h"

char * *codes; // An array of strings, used to store the Huffman tree codes.
FILE *in; // Set in main if user enters a file name
FILE *out; // Set in main if user enters a second file name
int pad = 0; // Used to figure out how many padding 0's will be needed at the end of the file.

int main(int argc, char * argv[]){
  /* The following mess of if statements sets input/output (I used freopen() instead of fopen()
   * to make the rest of my program a little simpler to write) */
  if(argc > 3){// Ya done messed up!
    printf("Usage: ./encode <input_file> [output_file]\n");
    exit(1); // Check number
  } else if(argc == 3){ // Set input and output files
    in = freopen(argv[1], "r", stdin);
    if(in == NULL){
      fprintf(stderr, "Input file \"%s\" does not exist.\n", argv[1]);
      exit(1);
    }
    
    out = freopen(argv[2], "w", stdout);

  } else if(argc == 2){ // Set only input file
    
    in = freopen(argv[1], "r", stdin);
    
    if(in == NULL){
      fprintf(stderr, "Input file \"%s\" does not exist.\n", argv[1]);
      exit(1);
    }
  } else { // Use stdin and stdout (should be redirected for dependable results)
    ;
  }

  encode(); // Does the thing :)
  fclose(stdin); // Close streams (in case they're files)
  fclose(stdout);

  return 0;  
}

// Does all of the things!!
void encode(){
  // Initializes the list (that will later be the huffman tree, hence the name)
  struct dtnode *huff = makeList(); 
  int c = getchar(); // First char from input
  long long freqs[256]; // An int array for storing char frequencies
  int i = 0; // An iterator

  // Set up the array that will hold the codes for each char.
  codes = calloc(256, 9*sizeof(char)); 
  while(i <= eof){
    codes[i] = calloc(9, sizeof(char));
    codes[i][0]='\0';
    i++;
  }
  
  i = 0;
  while(i < 256){ // zero out frequency array for ease of adding
    freqs[i++] = 0;
  }

  while(c != EOF){ // Gets frequencies of chars in input
    freqs[c] += 1;
    c = getchar();
  }

  i = 0; // Reset to retraverse the array
  while(i < 256){ // Builds the list that will be used for creating a huffman tree
    if(freqs[i] > 0){ // Makes a node for each char with >0 frequency; list is kept sorted
      huff = insert(huff, newNode(i, freqs[i]));
    }
    i++;
  }

  insert(huff, newNode(-1, 1)); // EOF is hardcoded into the list (its frequency is, obviously, always 1)

  huff = makeHuff(huff); // Builds the tree
  getCodes(huff); // Makes an array of the codes in input
  printHuff(huff); // Prints the encoded tree to output
  print_encode(); // Encodes input file in output
  freeTree(huff); // Frees memory tree & codes took up.
  freeCodes(); // Frees the memory the character codes were stored in.
}

// This function is a bit buffer for output; every time CHAR_BIT bits are accumulated, the corresonding character will be printed! 
void buffer(int bit){ // (Bit will be either 1 or 0, hence if statements)
  static char buff = 0;
  static int count = 0;

  if(bit){
    buff = (buff << 1)|1;
  } else {
    buff = buff << 1;
  }

  if((++count) == CHAR_BIT){ // Time to print~
    count = 0;
    putchar(buff);
  }
}

// This function will call buffer until an entire code from the huffman tree has been buffered. 
void buffer_code(int index){ // Index is the char value whose code should be buffered
  char * code = codes[index];
  int i = 0;
  while(code[i] != '\0'){ 
    if(code[i]-'0'){
      buffer(1);
    } else {
      buffer(0);
    }
    i++;
  }
}

// Frees the array of codes used to encode output
void freeCodes(){
  int i;
  for(i = 0; i <= eof; i++){
    free(codes[i]);
  }
  free(codes);
}

// Returns to the beginning of input and prints an encoded version to output
void print_encode(){
  rewind(stdin); // Moves to the beggining of input
  int c = getchar(); // Starts reading again

  while(c != EOF){ // Print the encoded file :)
    setPad(strlen(codes[c]));
    buffer_code(c);
    c = getchar();
  }

  setPad(strlen(codes[eof])); // Account for EOF in pad 
  buffer_code(eof); // Add EOF to end (so decode knows where to stop)

  if(pad != 0){ // Pads out the final byte with 0's
    pad = (CHAR_BIT - pad);
    int i;
    for(i = 0; i < pad; i++){
      buffer(0); 
    }
  }
}

// Takes a doubly linked list (of dtnodes) as input and returns a huffman tree of its contents
struct dtnode *makeHuff(struct dtnode *list){
  while(length(list) > 1){
    struct dtnode *first = list->next; // Get two nodes to be removed/added do new node
    struct dtnode *second = first->next;

    rmFront(list); // Remove the first two nodes from the list
    rmFront(list); // (they will now be accessed vie the node inserted below)
    
    list = insert(list, newTNode(0, ((first->freq) + (second->freq)), first, second)); // insert new branch
  }
  
  return list; // "List" is now a huffman tree!!
}

// Simply adjusts pad for each char/string printed to output (I'm lazy)
void setPad(int n){
  pad += n;
  pad %= CHAR_BIT;
}

// Prints the encoded tree (using printHuff_fr) and adds the code for EOF to the end
void printHuff(struct dtnode *tree){
  printHuff_fr(tree);
  setPad((int)strlen(codes[eof]));
  buffer_code(eof);
}

// Prints all of the encoded tree except the trailing EOF code (printed by printHuff())
void printHuff_fr(struct dtnode *tree){
  static int called = FALSE;
  int curC;
  if(!called && isEmpty(tree)){ // Can't traverse an empty tree, silly!
    return;
  } else {
    struct dtnode *tmp = tree;
    if(!called){ // Move to the first "real" node of the tree
      tmp = tmp->next;
      called = TRUE;
    }

    if(isLeaf(tmp)){
      if(tmp->c_val == -1){ // So that EOF is handled correctly.
	curC = 255;
      } else {
	curC = tmp->c_val;
      }
      setPad(1);
      buffer(1);
      int i; // Prints the first CHAR_BIT bits of the character!! :)
      for(i = 0; i < CHAR_BIT; i++){
	buffer(!!((curC << i) & 0x80));
      }
      
    } else {
      setPad(1);
      buffer(0);
      printHuff_fr(tmp->left);
      printHuff_fr(tmp->right);
    }
  }
}

// Calls the ~actual~ getCodes with an added parameter
void getCodes(struct dtnode *tree){
  getCodes_fr(tree, 0);
}

// Gets the codes for each char in the huffman tree and stores them in the codes array
void getCodes_fr(struct dtnode *tree, int curChar){
  static char curCode[17]; // Holds the code for the current leaf
  static int called = FALSE;
  
  if(!called && isEmpty(tree)){ // An empty tree won't have any codes!!
    return;
  } else {
    struct dtnode *tmp = tree;
    if(!called){ // Skips head sentinel node
      tmp = tmp->next;
      called = TRUE;
    }

    if(isLeaf(tmp)){
      if(tmp->c_val == -1){
	strcpy(codes[eof], curCode);
      } else {
	strcpy(codes[tmp->c_val], curCode);
      }
    } else {
      curCode[curChar] = '0'; // Traverse left subtree
      curCode[curChar+1] = '\0';
      getCodes_fr(tmp->left, ++curChar);

      curCode[--curChar] = '1'; // Traverse right subtree
      curCode[curChar+1] = '\0';
      getCodes_fr(tmp->right, ++curChar);
    }
  }
}
