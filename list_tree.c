/*
 * list_tree.c
 * Caeli Massey
 * Last edited December 5 2018
 *
 * This file contains all functions used to manipulate dtnode structures in encode.c and decode.c.
 * These nodes may be used to create doubly-linked lists or binary trees. 
 */

#include "code.h"

// Creates and returns a new dtnode
struct dtnode *newNode(int new_c, int new_freq){ 
  struct dtnode *node = NODE;

  if(node == NULL){
    fprintf(stderr, "%s", "Unable to allocate memory properly.\n");
    exit(3);
  } else {
    node->prev = node->next = node->left = node->right = NULL;
    node->c_val = new_c; // ???
    node->freq = new_freq; // ???
  }
  return node;
}

// Creates a new "tree node" (left and right nodes are specified at time of allocation)
struct dtnode *newTNode(int new_c, int new_freq, struct dtnode *l, struct dtnode *r){ 
  struct dtnode *node = NODE;

  if(node == NULL){
    fprintf(stderr, "%s", "Unable to allocate memory properly.\n");
    exit(3);
  } else {
    node->prev = node->next = NULL;
    node->left = l;
    node->right = r;
    
    node->c_val = new_c; 
    node->freq = new_freq; 
  }
  return node;
}

// Will free the memory holding a tree structure
void freeTree(struct dtnode *tree){
  static int called = FALSE;

  if(!called && isEmpty(tree)){
    return;
  } else {
    struct dtnode *tmp = tree;
    if(!called){
      tmp = tmp->next;
      free(tmp->prev);
      free(tmp->next);
      called = TRUE;
    }
    
    if(isLeaf(tmp)){
      free(tmp);
    } else {
      freeTree(tmp->left);
      freeTree(tmp->right);
      free(tmp);
    }
  }
}


// Returns a new list 
struct dtnode *makeList(){
  // Sentinel nodes
  struct dtnode *head = newNode(-2, 0);
  struct dtnode *tail = newNode(-2, 0);

  head->next = tail; // Point them at each other
  tail->prev = head;

  return head;
}

// Returns true if the list is empty, false otherwise.
int isEmpty(struct dtnode *list){
  if(list->next->next == NULL){
    return TRUE;
  } else {
    return FALSE;
  }
}

// Returns the number of nodes in a linked list (ignoring sentinel nodes)
int length(struct dtnode *list){
  int len = 0;
  if(isEmpty(list)){ // No nodes! :(
    return len;
  } else { // Traverse and count each non-sentinel node of the list
    struct dtnode *tmp = list;
    while(tmp->next->next != NULL){
      tmp = tmp->next;
      len++;
    }

    return len;
  }
}

// Determines whether a node is a leaf (has no children) or not
int isLeaf(struct dtnode *node){
  if((node->left == NULL)&&(node->right == NULL)){
    return TRUE;
  } else {
    return FALSE;
  }
}


/* Will insert a node (toAdd) into a list (list) based on frequency, from lowest to highest.
* A node of equal value to another is placed before any equal nodes in the list. */
struct dtnode *insert(struct dtnode *list, struct dtnode *toAdd){
  struct dtnode *tmp = list; // For traversing list
  int skip = isEmpty(list); // So that empty lists are added to properly

  while(tmp->next->next != NULL){ // Locate where toAdd should be inserted
    if(skip){
      break;
    } else if(tmp->next->freq < toAdd->freq){
      tmp = tmp->next;
      continue;
    } else {
      break;
    }
  }
  
  // Insert the toAdd in the proper spot
  toAdd->next = tmp->next;
  tmp->next->prev = toAdd;
  tmp->next = toAdd;
  toAdd->prev = tmp;
  return list; // Return the head of the list
}

// Removes the head node of the list (for converting between list and huffman tree) :)
struct dtnode *rmFront(struct dtnode *list){
  if(isEmpty(list)){
    fprintf(stderr, "Cannot remove from an empty list.\n");
    exit(1);
  } else {
    struct dtnode *tmp = list->next;
    tmp->next->prev = tmp->prev;
    tmp->prev->next = tmp->next;
    tmp->next = tmp->prev = NULL;
    return list;
  }
}
