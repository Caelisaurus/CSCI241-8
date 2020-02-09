#ifndef CODE_H
#define CODE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define NODE (calloc(1, sizeof(struct dtnode)))
#define TRUE 1
#define FALSE 0
#define eof 256 // Used in encode

// A structure that may function as a doubly-linked list OR binary tree node.
struct dtnode {
  struct dtnode *prev, *next, *left, *right;
  int c_val, freq; // c_val is a character value; freq is the node's frequency in input files for encode
};

// The following functions are from list_tree.c, which contains functions for manipulating my
// doubly-linked + tree nodes.
struct dtnode *newNode(int new_c, int new_freq); // Creates a new list node
struct dtnode *newTNode(int new_c, int new_freq, struct dtnode *l, struct dtnode *r); // Creates a new tree node
struct dtnode *makeList(); // Creates and returns the head of an empty list
struct dtnode *insert(struct dtnode *list, struct dtnode *toAdd); // Inserts a node (list kept sorted hi-lo, based on freq of nodes)
struct dtnode *rmFront(struct dtnode *list); // Removes the head element of list
int isEmpty(struct dtnode *list); // Tests is the list is empty
int length(struct dtnode *list); // Returns the number of non-sentinel elements
int isLeaf(struct dtnode *node); // Tests whether a node is a leaf
void freeTree(struct dtnode *tree); // Frees all memory being used by a tree-node structure
void freeTree_fr(struct dtnode *tree); // Does the hard stuff for freeTree() 

// These functions are used in encode.c
void encode(); // Calls all functions needed to encode input file
struct dtnode *makeHuff(struct dtnode *list); // Makes huffman tree
void getCodes(struct dtnode *tree); // Gets codes from tree
void getCodes_fr(struct dtnode *tree, int curChar); // Does the grunt work of getCodes()
void printHuff(struct dtnode *tree); // Prints the bit encoding of the huffman tree
void printHuff_fr(struct dtnode *tree); // Does the grunt work for printHuff()
void print_encode(); // Prints the encoded file to output
void freeCodes(); // Frees memory codes were stored in for encoding
void setPad(int n); // Alters the var used to keep track of padding at end of file.
void buffer(int bit); // Adds bit to the end of a bit buffer, prints corresponding char every CHAR_BIT bits
void buffer_code(int index); // Buffers an entire character code from the tree

// These are used in decode.c
void decode(); // Calls all functions necesarry for decoding input
struct dtnode *buildTree(struct dtnode *tree); // Reads in bits from input & builds the encoded tree
struct dtnode *buildTree_fr(struct dtnode *tree); // (Does the heavy lifting for buildTree)
void printDecode(struct dtnode *tree); // Prints the decoded file
int getBit(); // Returns the value of the next bit from the last read char
unsigned char getCh(); // Reads in a char worth of bits and returns the char's value.

#endif
