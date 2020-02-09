/*
 * decode.c
 * Caeli Massey
 * Last edited December 5 2018
 * 
 * This program will read a text file encoded by encode and print the decompressed, decoded version
 * of it (in put and output files can both be specified via redirection or parameters at the time 
 * of execution).
 */
#include "code.h"

/* struct dtnode *buildTree(struct dtnode *tree); */
/* struct dtnode *buildTree_fr(struct dtnode *tree); */
/* void printDecode(struct dtnode *tree); */
/* void decode(); */
/* void printTree(struct dtnode *tree); */
/* int getBit(); */
/* unsigned char getCh(); */

FILE *in;
FILE *out;

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
      exit(1);// Check number??
    }
    out = freopen(argv[2], "w", stdout);

  } else if(argc == 2){ // Set only input file

    in = freopen(argv[1], "r", stdin);

    if(in == NULL){
      fprintf(stderr, "Input file \"%s\" does not exist.\n", argv[1]);
      exit(1);// Check number??
    }
  } else { // Use stdin and stdout (should be redirected for dependable results)
    ;
  }

  
  decode(); // Do the thing ~
  fclose(stdin); // Close streams (in case they're files)
  fclose(stdout);

  return 0;
}

void decode(){
  struct dtnode *huff = makeList(); // The soon-to-be huffman tree
  huff = buildTree(huff); // Builds ze tree (just in time for Christmas!)
  printDecode(huff); // Prints the decoded version of the file
  freeTree(huff); // Frees memory allocated for storing the tree
}

// Returns the next bit read in from input 
int getBit(){
  static int ch = 0; // The current char bits are being read from
  static int called = 0;
  int toReturn = 0;
  
  if(!called){  // (gccx kept yelling at me for setting a static var to getchar() in one line)
    ch = getchar();
    called = 1;
  }
  
  if(ch != EOF){ // I don't think this will ever happen, but better safe than sorry.
    static int count = 0;

    if((ch & 0x80)){ // The first bit of ch
      toReturn = 1;
    } else {
      toReturn =  0;
    }

    ch = (ch << 1); // move the next bit to the "front of the line"
  
    if(++count == CHAR_BIT){ // Out of bits? Grab 8 more!
      ch = getchar();
      count = 0;
    }
  } else { // Something's... not right
    fprintf(stderr, "Error: EOF reached.\n");
    exit(1);
  }

  return toReturn;
}

// This function will collect bits (using getBit()) until a full char has been read --
// the char is then returned for printing.
unsigned char getCh(){
  unsigned char toReturn = 0;

  int i = 0;
  int bit = 0;
  while(i < CHAR_BIT){
    bit = getBit();
    if(bit){
      toReturn = (toReturn << 1) | 1;
    } else {
      toReturn = (toReturn << 1);
    }
    i++;
  }

  return toReturn;
}

// Prints the decoded version of the input file 
void printDecode(struct dtnode *tree){
  struct dtnode *tmp = tree;
  if(isEmpty(tree)){
    printf("Tree empty... :/ \n");
    return;
  } else {
    tmp = tmp->next;
  }

  int nextBit;
  while(TRUE){
    nextBit = getBit();
    if(nextBit/*c == '1'*/){ // Traverse the tree according to the encoded file 
      tmp = tmp->right;
    } else /*if(c == '0')*/{
      tmp = tmp->left;
    }

    if(isLeaf(tmp)){ // When a leaf is reached, print its char value
      if(tmp->c_val != -1){
	putchar((char)tmp->c_val);
	tmp = tree->next; // Then return to root for next traversal
      } else {
	return;
      }
    }
  }
}

// This funtion calls buildTree_fr() to build the encoded huffman tree in the file, then correct's EOF's value
// in the tree afterwards.
struct dtnode *buildTree(struct dtnode *tree){
  tree = buildTree_fr(tree);
  struct dtnode *tmp = tree->next;

  int nextBit;
  while(!isLeaf(tmp)){ // This will find the leaf storing EOF's code :)
    nextBit = getBit();   
    if(nextBit){
      tmp = tmp->right;
    } else{
      tmp = tmp->left;
    }
  }

  tmp->c_val = -1; // Correct EOF's character value
  return tree;
}

// Recursively builds a huffman tree based on text at the beginning of the file. 
struct dtnode * buildTree_fr(struct dtnode *tree){
  static int called = FALSE;
  struct dtnode *tmp = tree;

  int nextBit = getBit();
  
  if(!nextBit){ // Must be an inner node
    if(!called){ // So the tree is set up correctly for the first 0 :)
      tree = insert(tree, newTNode(0, 0, NULL, NULL)); // The root
      tmp = tmp->next;
      called = TRUE;
      buildTree_fr(tmp);
    } else if(tmp->left == NULL){ // Set left if it hasn't been set
      tmp->left = newTNode(0, 0, NULL, NULL);
      buildTree_fr(tmp->left);
      buildTree_fr(tmp);
    } else { // Otherwise set right
      tmp->right = newTNode(0, 0, NULL, NULL);
      buildTree_fr(tmp->right);
    }
  } else if(nextBit){ // Add the proceeding character value to the tree
    if(tmp->left == NULL){ // Set left if it hasn't been set
      tmp->left = newTNode(getCh(), 1, NULL, NULL);
      buildTree_fr(tmp);
    } else { // Otherwise set right -- no need to recurse!
      tmp->right = newTNode(getCh(), 1, NULL, NULL);
    }
  }

  return tree;
}
