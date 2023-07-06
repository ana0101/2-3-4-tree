#include <iostream>
#include <fstream>
#include <limits>
using namespace std;

ifstream in("abce.in");
ofstream out("abce.out");

const int MIN = numeric_limits<int>::min();
const int MAX = numeric_limits<int>::max();

class Node {
public:
    int order;
    int data[3];
    Node* children[4];
    Node* parent;

    Node();
};

Node::Node() {
    order = 1;
    for (int i = 0; i < 3; i ++)
        data[i] = MAX;
    for (int i = 0; i < 4; i ++)
        children[i] = nullptr;
    parent = nullptr;
}


class Tree {
public:
    Node* root;

    Tree();
    void insertNumber(int number, Node* node);
    void splitRoot();
    Node* splitNode(Node* node);
    void deleteNumber(int number, Node* node);
    void deleteNumberInNode(int index, Node* node);
    int predecessor2(Node* node);
    int successor2(Node* node);
    void merge(int index, Node* node);
    void merge2(Node* node);
    void leftSiblingRotation(int index, Node* node);
    void rightSiblingRotation(int index, Node* node);
    bool searchNumber(int number, Node* node);
    int predecessor(int number, Node* node);
    int successor(int number, Node* node);
};

Tree::Tree() {
    root = new Node();
}


// insereaza un numar in arbore
void Tree::insertNumber(int number, Node* node) {
    // daca radacina e goala
    if (this->root->order == 1) {
        this->root->data[0] = number;
        this->root->order = 2;
        return;
    }

    // daca nodul curent are 3 valori, ii facem split
    if (node->order == 4) {
        // daca nodul curent este radacina, trebuie creata o noua radacina
        if (node->parent == nullptr) {
            this->splitRoot();
            this->insertNumber(number, this->root);
        }
        else {
            // daca nu este radacina, luam valoarea din mijloc si o inseram in parinte
            int promoted = node->data[1];
            Node* newNode = this->splitNode(node);

            if (number < promoted)
                this->insertNumber(number, node);
            else
                this->insertNumber(number, newNode);
        }
    }
    else {
        int index = 0;
        while (index < node->order-1 && number > node->data[index])
            index ++;

        // daca este un nod intern, coboram cu inserarea
        if (node->children[0] != nullptr) {
            this->insertNumber(number, node->children[index]);
        }
        else {
            // daca este frunza, inseram direct
            for (int i = node->order-1; i > index; -- i) {
                node->data[i] = node->data[i-1];
            }
            node->data[index] = number;
            node->order ++;
        }
    }
}


// departe radacina
void Tree::splitRoot() {
    int promoted = this->root->data[1];
    // cream doua noduri noi
    Node* leftChild = new Node();
    Node* rightChild = new Node();

    // punem valorile in cei doi copii
    leftChild->data[0] = this->root->data[0];
    rightChild->data[0] = this->root->data[2];

    // stergem acele valori din radacina
    this->root->data[1] = this->root->data[2] = MAX;
    this->root->data[0] = promoted;

    // punem fostii copii ai radacinii noilor copii
    if (this->root->children[0] != nullptr) {
        leftChild->children[0] = this->root->children[0];
        this->root->children[0]->parent = leftChild;

        leftChild->children[1] = this->root->children[1];
        this->root->children[1]->parent = leftChild;

        rightChild->children[0] = this->root->children[2];
        this->root->children[2]->parent = rightChild;

        rightChild->children[1] = this->root->children[3];
        this->root->children[3]->parent = rightChild;
    }

    // punem nodurile noi ca copii radacinii
    this->root->children[0] = leftChild;
    leftChild->parent = this->root;

    this->root->children[1] = rightChild;
    rightChild->parent = this->root;

    this->root->children[2] = this->root->children[3] = nullptr;

    // punem ordinele
    this->root->order = leftChild->order = rightChild->order = 2;
}


// desparte un nod
Node* Tree::splitNode(Node* node) {
    int promoted = node->data[1];
    int index = 0;
    while (index < 2 && promoted > node->parent->data[index])
        index ++;
    
    // inseram promoted in parinte
    for (int i = node->parent->order-1; i > index; -- i) {
        node->parent->data[i] = node->parent->data[i-1];
    }
    node->parent->data[index] = promoted;
    node->parent->order ++;

    // cream un nod nou cu ultima valoare din nodul curent
    Node* newNode = new Node();
    newNode->data[0] = node->data[2];
    newNode->order = 2;

    // stergem ultimele doua valori din nodul curent
    node->data[1] = node->data[2] = MAX;
    node->order = 2;

    // legam nodul nou de parinte
    for (int i = node->parent->order-1; i > index+1; i --)
        node->parent->children[i] = node->parent->children[i-1];

    node->parent->children[index+1] = newNode;
    newNode->parent = node->parent;

    // mutam copiii
    if (node->children[0] != nullptr) {
        newNode->children[0] = node->children[2];
        node->children[2]->parent = newNode;
        node->children[2] = nullptr;

        newNode->children[1] = node->children[3];
        node->children[3]->parent = newNode;
        node->children[3] = nullptr;
    }
    return newNode;
}


// sterge un numar din arbore
void Tree::deleteNumber(int number, Node* node) {
    // daca arborele e gol
    if (this->root->order == 1)
        return;

    int index = 0;
    while (index < node->order-1 && number > node->data[index])
        index ++;

    // daca am ajuns in frunza, stergem direct
    if (node->children[0] == nullptr) {
        if (index < node->order-1)
            if (number == node->data[index])
                this->deleteNumberInNode(index, node);
    }
    else {
        // daca suntem intr-un nod intern si am gasit numarul
        if (index < node->order-1 && number == node->data[index]) {

            // daca copilul stang are ordinul > 2, punem predecesorul
            if (node->children[index]->order > 2) {
                node->data[index] = this->predecessor2(node->children[index]);
                this->deleteNumber(node->data[index], node->children[index]);
            }
            else {
                // daca copilul drept are ordinul > 2, punem succesorul
                if (node->children[index+1]->order > 2) {
                    node->data[index] = this->successor2(node->children[index+1]);
                    this->deleteNumber(node->data[index], node->children[index+1]);
                }
                else {
                    // daca amandoi copiii au ordinul 2, facem merge
                    // daca nodul curent are ordinul > 2, facem merge
                    if (node->order > 2) {
                        this->merge(index, node);
                        this->deleteNumber(number, node->children[index]);
                    }
                    else {
                        // daca nodul curent are ordinul 2, atunci inseamna ca e radacina si facem merge cu toti trei
                        this->merge2(node);
                        this->deleteNumber(number, node);
                    }
                }
            }
        }
        else {
            // daca urmeaza sa coboram intr-un nod cu ordinul > 2, coboram
            if (node->children[index]->order > 2) {
                this->deleteNumber(number, node->children[index]);
            }
            else {
                // daca urmeaza sa coboram intr-un nod cu ordinul 2, trebuie modificat
                // fratele stang
                if (index > 0 && node->children[index-1]->order > 2) {
                    this->leftSiblingRotation(index, node);
                    this->deleteNumber(number, node->children[index]);
                }
                else {
                    // fratele drept
                    if (index < node->order-1 && node->children[index+1]->order > 2) {
                        this->rightSiblingRotation(index, node);
                        this->deleteNumber(number, node->children[index]);
                    }
                    else {
                        // daca niciunul din frati nu are ordinul > 2, atunci ne uitam la parinte
                        // daca parintele are ordinul > 2, facem merge
                        if (node->order > 2) {

                            // merge cu fratele stang
                            if (index > 0) {
                                this->merge(index-1, node);
                                this->deleteNumber(number, node->children[index-1]);
                            }
                            else {
                                // merge cu fratele drept
                                if (index < node->order-1) {
                                    this->merge(index, node);
                                    this->deleteNumber(number, node->children[index]);
                                }
                            }
                        }
                        else {
                            // daca nici parintele nu are ordin > 2, atunci inseamna ca e radacina si facem merge cu toti trei
                            this->merge2(node);
                            this->deleteNumber(number, node);
                        }
                    }
                }
            }
        }
    }
}


// sterge un numar dintr-un nod
void Tree::deleteNumberInNode(int index, Node* node) {
    for (int i = index; i < node->order-2; i ++)
        node->data[i] = node->data[i+1];
    node->data[node->order-2] = MAX;
    node->order --;
}

// predecesorul din subarbore
int Tree::predecessor2(Node* node) {
    if (node->children[0] == nullptr)
        return node->data[node->order-2];
    return this->predecessor2(node->children[node->order-1]);
}

// succesorul din subarbore
int Tree::successor2(Node* node) {
    if (node->children[0] == nullptr)
        return node->data[0];
    return this->successor2(node->children[0]);
}

// uneste doi copii
void Tree::merge(int index, Node* node) {
    Node* leftChild = node->children[index];
    Node* rightChild = node->children[index+1];

    // punem valorile noi in copilul stang
    leftChild->data[1] = node->data[index];
    leftChild->data[2] = rightChild->data[0];
    leftChild->order = 4;

    // mutam copiii copilului drept la cel stang
    if (rightChild->children[0] != nullptr) {
        leftChild->children[2] = rightChild->children[0];
        rightChild->children[0]->parent = leftChild;

        leftChild->children[3] = rightChild->children[1];
        rightChild->children[1]->parent = leftChild;
    }

    // mutam restul copiilor nodului
    for (int i = index+1; i < node->order-1; i ++)
        node->children[i] = node->children[i+1];
    node->children[node->order-1] = nullptr;

    // stergem valoarea din nod
    this->deleteNumberInNode(index, node);
}


// uneste radacina si cei doi copii
void Tree::merge2(Node* node) {
    Node* leftChild = node->children[0];
    Node* rightChild = node->children[1];

    // mutam valorile in radacina
    node->data[1] = node->data[0];
    node->data[0] = leftChild->data[0];
    node->data[2] = rightChild->data[0];

    // mutam copiii la radacina
    if (leftChild->children[0] != nullptr) {
        node->children[0] = leftChild->children[0];
        leftChild->children[0]->parent = node;

        node->children[1] = leftChild->children[1];
        leftChild->children[1]->parent = node;

        node->children[2] = rightChild->children[0];
        rightChild->children[0]->parent = node;

        node->children[3] = rightChild->children[1];
        rightChild->children[1]->parent = node;
    }
    else {
        node->children[0] = node->children[1] = nullptr;
    }

    // ordinul
    node->order = 4;
}


void Tree::leftSiblingRotation(int index, Node* node) {
    // mutam valorile
    node->children[index]->data[1] = node->children[index]->data[0];
    node->children[index]->data[0] = node->data[index-1];
    node->data[index-1] = node->children[index-1]->data[node->children[index-1]->order-2];
    this->deleteNumberInNode(node->children[index-1]->order-2, node->children[index-1]);

    // ordinele
    node->children[index]->order ++;

    // mutam copiii
    if (node->children[index-1]->children[0] != nullptr) {
        node->children[index]->children[2] = node->children[index]->children[1];
        node->children[index]->children[1] = node->children[index]->children[0];
        node->children[index]->children[0] = node->children[index-1]->children[node->children[index-1]->order];
        node->children[index-1]->children[node->children[index-1]->order]->parent = node->children[index];
        node->children[index-1]->children[node->children[index-1]->order] = nullptr;
    }
}


void Tree::rightSiblingRotation(int index, Node* node) {
    // mutam valorile
    node->children[index]->data[1] = node->data[index];
    node->data[index] = node->children[index+1]->data[0];
    this->deleteNumberInNode(0, node->children[index+1]);

    // ordinele
    node->children[index]->order ++;

    // mutam copiii
    if (node->children[index+1]->children[0] != nullptr) {
        node->children[index]->children[2] = node->children[index+1]->children[0];
        node->children[index+1]->children[0]->parent = node->children[index];
        for (int i = 0; i < node->children[index+1]->order; i ++)
            node->children[index+1]->children[i] = node->children[index+1]->children[i+1];
        node->children[index+1]->children[node->children[index+1]->order] = nullptr;
    }
}


// cauta un numar in arbore
bool Tree::searchNumber(int number, Node* node) {
    // daca arborele e gol
    if (this->root->order == 1)
        return 0;
    else {
        int index = 0;
        while (index < node->order-1 && number > node->data[index])
            index ++;

        // daca am gasit numarul
        if (index < node->order-1 && number == node->data[index])
            return 1;
        else {
            // daca suntem intr-un nod intern si inca nu l-am gasit
            if (node->children[0] != nullptr)
                return this->searchNumber(number, node->children[index]);
            else
                // daca suntem intr-o frunza si nu l-am gasit, inseamna ca nu exista
                return 0;
        }
    }
}


// predecesorul unui numar
int Tree::predecessor(int number, Node* node) {
    int index = 0;
    while (index < node->order-1 && number >= node->data[index])
        index ++;

    // daca am gasit numarul
    if (index > 0 && number == node->data[index-1]) 
        return number;
    else {
        // daca am ajuns in frunza
        if (node->children[0] == nullptr) {
            if (index == 0)
                return MIN;
            return node->data[index-1];
        }
        else {
            // daca suntem intr-un nod intern
            if (index == 0)
                return this->predecessor(number, node->children[0]);
            return max(node->data[index-1], this->predecessor(number, node->children[index]));
        }
    }
}


// succesorul unui numar
int Tree::successor(int number, Node* node) {
    int index = 0;
    while (index < node->order-1 && number >= node->data[index])
        index ++;

    // daca am gasit numarul
    if (index > 0 && number == node->data[index-1])
        return number;
    else {
        // daca am ajuns in frunza
        if (node->children[0] == nullptr) {
            if (index == node->order-1)
                return MAX;
            return node->data[index];
        }
        else {
            // daca suntem intr-un nod intern
            if (index == node->order-1)
                return this->successor(number, node->children[node->order-1]);
            return min(node->data[index], this->successor(number, node->children[index]));
        }
    }
}


int main() {
    int n, op, number;
    Tree* tree = new Tree();
    in >> n;

    for (int i = 0; i < n; i ++) {
        in >> op >> number;

        switch(op) {
            case 1: {
                // inserare
                tree->insertNumber(number, tree->root);
                break;
            }

            case 2: {
                // stergere
                tree->deleteNumber(number, tree->root);
                break;
            }

            case 3: {
                // cautare
                out << tree->searchNumber(number, tree->root) << "\n";
                break;
            }

            case 4: {
                // predecesor
                out << tree->predecessor(number, tree->root) << "\n";
                break;
            }

            case 5: {
                // succesor
                out << tree->successor(number, tree->root) << "\n";
                break;
            }

            case 6: {
                // sort
                int nr2;
                in >> nr2;
                int nr3 = tree->successor(number, tree->root);
                while (nr3 <= nr2) {
                    out << nr3 << " ";
                    nr3 = tree->successor(nr3+1, tree->root);
                }
                out << "\n";
                break;
            }

            default: {
                break;
            }
        }
    }
    delete tree;

    return 0;
}