#ifndef ONLINE_RECORD_DATASTRUCT_H
#define ONLINE_RECORD_DATASTRUCT_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct Node {
    char* key;
    int value;
    struct Node *left, *right;
    int height;
} Node;


///////////////////////
//FUNCTION DECLARATIONS
///////////////////////


Node* newNode(char* key, int value);
int height(Node *N);
int max(int a, int b);
Node *rightRotate(Node *y);
Node *leftRotate(Node *x);
int getBalance(Node *N);
Node * minValueNode(Node* node);

Node* insert(Node* node, char* key, int value);
Node* deleteNode(Node* root, char* key);
Node* search(Node* root, char* key);
void update(Node* root, char* key, int newValue);

void preOrder(Node *root);

int is_online(char *username);
void set_online(char *username, int socket);
void set_offline(char *username);


#endif /* ONLINE_RECORD_DATASTRUCT_H */