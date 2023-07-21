#include <online_record_datastruct.h>

Node *root = NULL;

/**
 * Creates and initializes a new node for a binary search tree.
 *
 * @param key - The key value for the node.
 * @param value - The value associated with the key.
 * @return Node* - A pointer to the newly created node.
 */
Node* newNode(char* key, int value) {
    // Allocate memory for the new node
    Node* node = (Node*)malloc(sizeof(Node));

    // Allocate memory for the key string and copy the value
    node->key = malloc(strlen(key) + 1);
    strcpy(node->key, key);
    // Assign the value and set the left and right pointers to NULL
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    // Set the height of the node to 1 (assuming it is a leaf node)
    node->height = 1;

    // Return the newly created node
    return node;
}

/**
 * Returns the height of a given node in a binary search tree.
 *
 * @param N - A pointer to the node.
 * @return int - The height of the node. Returns 0 if the node is NULL.
 */
int height(Node *N) {
    // Check if the node is NULL
    if (N == NULL)
        return 0;

    // Return the height of the node
    return N->height;
}

/**
 * Returns the maximum of two integers.
 *
 * @param a - The first integer.
 * @param b - The second integer.
 * @return int - The maximum of the two integers.
 */
int max(int a, int b) {
    return (a > b)? a : b;
}

/**
 * Performs a right rotation on the given node.
 *
 * @param y - The node to perform the right rotation on.
 * @return Node* - The new root node after the rotation.
 */
Node *rightRotate(Node *y) {
    // Store the left child of y in a variable x
    Node *x = y->left;

    // Store the right child of x in a variable T2
    Node *T2 = x->right;

    // Perform the right rotation
    x->right = y;
    y->left = T2;
    // Update the height of y and x
    y->height = max(height(y->left), height(y->right))+1;
    x->height = max(height(x->left), height(x->right))+1;

    // Return the new root node after the rotation
    return x;
}

/**
 * Performs a left rotation on the given node.
 *
 * @param x - The node to perform the left rotation on.
 * @return Node* - The new root node after the rotation.
 */
Node *leftRotate(Node *x) {
    // Store the right child of x in a variable y
    Node *y = x->right;

    // Store the left child of y in a variable T2
    Node *T2 = y->left;

    // Perform the left rotation
    y->left = x;
    x->right = T2;
    // update height
    x->height = max(height(x->left), height(x->right))+1;
    y->height = max(height(y->left), height(y->right))+1;

    // Return the new root node after the rotation
    return y;
}

/**
 * Calculates the balance factor of the given node.
 *
 * @param N - The node to calculate the balance factor for.
 * @return int - The balance factor of the node.
 */
int getBalance(Node *N) {
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

/**
 * Inserts a new node with the given key and value into the binary search tree.
 *
 * @param node - The root node of the binary search tree.
 * @param key - The key of the new node to be inserted.
 * @param value - The value of the new node to be inserted.
 * @return Node* - The updated root node of the binary search tree.
 */
Node* insert(Node* node, char* key, int value) {
    // If the tree is empty, create a new node and return it as the root node
    if (node == NULL)
        return(newNode(key, value));

    // If the key is less than the current node's key, insert it into the left subtree
    if (strcmp(key, node->key) < 0)
        node->left  = insert(node->left, key, value);
    // If the key is greater than the current node's key, insert it into the right subtree
    else if(strcmp(key, node->key) > 0)
        node->right = insert(node->right, key, value);
    // If the key is already present, return the node
    else
        return node;

    // Update the height of the current node
    node->height = 1 + max(height(node->left), height(node->right));

    // Check the balance factor of the current node
    int balance = getBalance(node);

    // Perform necessary rotations to maintain balance
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

    // Return the updated root node
    return node;
}

/**
 * Returns the node with the minimum value in the given binary search tree.
 *
 * @param node - The root node of the binary search tree.
 * @return Node* - The node with the minimum value.
 */
Node * minValueNode(Node* node) {
    Node* current = node;

    // Traverse to the leftmost node to find the minimum value
    while (current->left != NULL)
        current = current->left;

    return current;
}


/**
 * Deletes a node with the given key from the binary search tree.
 *
 * @param root - The root node of the binary search tree.
 * @param key - The key of the node to be deleted.
 * @return Node* - The root node after the deletion operation.
 */
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

/**
 * Searches for a node with the given key in the binary search tree.
 *
 * @param root - The root node of the binary search tree.
 * @param key - The key to search for.
 * @return Node* - The node with the given key if found, NULL otherwise.
 */
Node* search(Node* root, char* key) {
    // If the root is NULL or the root's key matches the search key, return the root
    if (root == NULL || strcmp(root->key, key) == 0)
        return root;

    // If the root's key is less than the search key, move to the right subtree
    if (strcmp(root->key, key) < 0)
        return search(root->right, key);

    // If the root's key is greater than the search key, move to the left subtree
    return search(root->left, key);
}

/**
 * Updates the value of a node with the given key in the binary search tree.
 * If the node with the given key is found, its value is updated with the new value.
 *
 * @param root - The root node of the binary search tree.
 * @param key - The key of the node to update.
 * @param newValue - The new value to assign to the node.
 */
void update(Node* root, char* key, int newValue) {
    // Search for the node with the given key in the binary search tree
    Node* node = search(root, key);

    // If the node is found, update its value with the new value
    if (node != NULL) {
        node->value = newValue;
    }
}

/**
 * Checks if a user with the given username is online.
 *
 * @param username - The username to check.
 * @return -1 if the user is not online, otherwise returns the socket associated with the user.
 */
int is_online(char *username){
    // Search for the node with the given username in the binary search tree
    Node *found = search(root, username);

    if (found == NULL){
        return -1;
    } else {
        return found->value;
    }
}

/**
 * Sets a user with the given username as online with the specified socket.
 *
 * @param username - The username of the user to set as online.
 * @param socket - The socket associated with the user.
 */
void set_online(char *username, int socket) {
    // Insert a new node with the username and socket in the binary search tree
    root = insert(root, username, socket);
}

/**
 * Sets a user with the given username as offline.
 *
 * @param username - The username of the user to set as offline.
 */
void set_offline(char *username) {
    root = deleteNode(root, username);
}
