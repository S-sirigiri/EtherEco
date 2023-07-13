#include <online_record_datastruct.h>

Node *root = NULL;

Node* newNode(char* key, int value) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->key = malloc(strlen(key) + 1);
    strcpy(node->key, key);
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

int height(Node *N) {
    if (N == NULL)
        return 0;
    return N->height;
}

int max(int a, int b) {
    return (a > b)? a : b;
}

Node *rightRotate(Node *y) {
    Node *x = y->left;
    Node *T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max(height(y->left), height(y->right))+1;
    x->height = max(height(x->left), height(x->right))+1;
    return x;
}

Node *leftRotate(Node *x) {
    Node *y = x->right;
    Node *T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max(height(x->left), height(x->right))+1;
    y->height = max(height(y->left), height(y->right))+1;
    return y;
}

int getBalance(Node *N) {
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

Node* insert(Node* node, char* key, int value) {
    if (node == NULL)
        return(newNode(key, value));

    if (strcmp(key, node->key) < 0)
        node->left  = insert(node->left, key, value);
    else if(strcmp(key, node->key) > 0)
        node->right = insert(node->right, key, value);
    else
        return node;

    node->height = 1 + max(height(node->left), height(node->right));

    int balance = getBalance(node);

    if (balance > 1 && strcmp(key, node->left->key) < 0)
        return rightRotate(node);

    if (balance < -1 && strcmp(key, node->right->key) > 0)
        return leftRotate(node);

    if (balance > 1 && strcmp(key, node->left->key) > 0) {
        node->left =  leftRotate(node->left);
        return rightRotate(node);
    }

    if (balance < -1 && strcmp(key, node->right->key) < 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

Node * minValueNode(Node* node) {
    Node* current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

Node* deleteNode(Node* root, char* key) {
    if (root == NULL)
        return root;

    if (strcmp(key, root->key) < 0)
        root->left = deleteNode(root->left, key);
    else if(strcmp(key, root->key) > 0)
        root->right = deleteNode(root->right, key);
    else {
        if((root->left == NULL) || (root->right == NULL)) {
            Node *temp = root->left ? root->left : root->right;

            if(temp == NULL) {
                temp = root;
                root = NULL;
            } else {
                free(root->key); // free the old key
                root->key = strdup(temp->key); // duplicate the key
                root->value = temp->value;
                root->left = temp->left;
                root->right = temp->right;
                root->height = temp->height;
            }

            if(temp != NULL) {
                free(temp->key);
                free(temp);
            }
        } else {
            Node* temp = minValueNode(root->right);
            free(root->key); // free the old key
            root->key = strdup(temp->key); // duplicate the key
            root->value = temp->value;
            root->right = deleteNode(root->right, temp->key);
        }
    }

    if (root == NULL)
        return root;

    root->height = 1 + max(height(root->left), height(root->right));

    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

Node* search(Node* root, char* key) {
    if (root == NULL || strcmp(root->key, key) == 0)
        return root;

    if (strcmp(root->key, key) < 0)
        return search(root->right, key);

    return search(root->left, key);
}

void update(Node* root, char* key, int newValue) {
    Node* node = search(root, key);
    if (node != NULL) {
        node->value = newValue;
    }
}

// A utility function to print preorder traversal of the tree.
void preOrder(Node *root) {
    if(root != NULL) {
        printf("%s %d", root->key, root->value);
        preOrder(root->left);
        preOrder(root->right);
    }
}

int is_online(char *username){

    Node *found = search(root, username);

    if (found == NULL){
        return -1;
    } else {
        return found->value;
    }
}

void set_online(char *username, int socket) {
    root = insert(root, username, socket);
}

void set_offline(char *username) {
    root = deleteNode(root, username);
}
