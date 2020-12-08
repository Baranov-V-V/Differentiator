#include "Differentiator_header.h"

#define P node->parent
#define L node->left
#define R node->right
#define RD node->right->data
#define LD node->left->data
#define LT node->left->type
#define RT node->right->type
#define NT node->type
#define ND node->data

#define d(node) Differentiate(node)
#define c(node) CopyTree(node)
#define CONST(value) NodeConstruct(TYPE_CONST, value, node->parent, nullptr, nullptr)
#define ADD(left, right) NodeConstruct(TYPE_OP, OP_ADD, node->parent, left, right)
#define SUB(left, right) NodeConstruct(TYPE_OP, OP_SUB, node->parent, left, right)
#define MUL(left, right) NodeConstruct(TYPE_OP, OP_MUL, node->parent, left, right)
#define DIV(left, right) NodeConstruct(TYPE_OP, OP_DIV, node->parent, left, right)
#define COS(right) NodeConstruct(TYPE_OP, OP_COS, node->parent, nullptr, right)
#define SIN(right) NodeConstruct(TYPE_OP, OP_SIN, node->parent, nullptr, right)
#define LN(right) NodeConstruct(TYPE_OP, OP_LN, node->parent, nullptr, right)
#define EXP(right) NodeConstruct(TYPE_OP, OP_EXP, node->parent, nullptr, right)
#define POW(left, right) NodeConstruct(TYPE_OP, OP_POW, node->parent, left, right)
#define TAN(right) NodeConstruct(TYPE_OP, OP_TAN, node->parent, nullptr, right)

Node* NodeConstructEmpty() {
    Node* new_node = (Node*) calloc(1, sizeof(Node));

    if (new_node == nullptr) {
        fprintf(stderr, "Can not allocate new Node\n");
        return nullptr;
    }

    new_node->type = TYPE_NO;
    new_node->left = nullptr;
    new_node->right = nullptr;
    new_node->parent = nullptr;

    return new_node;
}

Node* NodeConstruct(TYPE type, type_t data, Node* parent, Node* left, Node* right) {

    Node* new_node = (Node*) calloc(1, sizeof(Node));
    if (new_node == nullptr) {
        fprintf(stderr, "Can not allocate new Node\n");
        return nullptr;
    }

    new_node->left = left;
    new_node->right = right;
    new_node->parent = parent;

    new_node->type = type;
    new_node->data = data;

    return new_node;
}

ExitCodes NodeDestruct(Node* node) {
    assert(node != nullptr);

    free(node);

    EXIT_OK;
}

ExitCodes TreeConstruct(Tree* tree) {
    assert(tree != nullptr);

    tree->variables = (int*) calloc(MAX_VARIABLES_COUNT, sizeof(int));
    tree->root = nullptr;

    return EXIT_OK;
}

ExitCodes DestructNodes(Node* node) {
    assert(node != nullptr);

    if (node->left != nullptr) {
        return DestructNodes(node->left);
    }
    if (node->right != nullptr) {
        return DestructNodes(node->right);
    }
    free(node);

    return EXIT_OK;
}

ExitCodes TreeDestruct(Tree* tree) {
    assert(tree != nullptr);

    TreeClear(tree);
    free(tree->variables);

    return EXIT_OK;
}

ExitCodes TreeClear(Tree* tree) {
    assert(tree != nullptr);

    if (tree->root != nullptr) {
        DestructNodes(tree->root);
    }

    for (int i = 0; i < MAX_VARIABLES_COUNT; ++i) {
        tree->variables[i] = 0;
    }

    tree->root = nullptr;

    return EXIT_OK;
}

bool FindVar(Node* node) {
    if (node == nullptr) {
        return false;
    }

    if (node->type == TYPE_VAR) {
        return true;
    }

    if (FindVar(node->left) == true) {
        return true;
    }

    if (FindVar(node->right) == true) {
        return true;
    }

    return false;

}

void SetParents(Node* node) {
    if (node != nullptr) {
        if (node->left != nullptr) {
            node->left->parent = node;
        }
        if (node->right != nullptr) {
            node->right->parent = node;
        }
        SetParents(node->left);
        SetParents(node->right);
    }
}

Node* CopyTree(Node* node) {
    if (node == nullptr) {
        return nullptr;
    }
    return NodeConstruct(node->type, node->data, node->parent,
                         CopyTree(node->left), CopyTree(node->right));
}

Node* Differentiate(Node* node) {
    if (node == nullptr) {
        return nullptr;
    }

    Node* der = nullptr;

    switch (node->type) {

        case TYPE_CONST:
            der = CONST(0);
            break;

        case TYPE_VAR:
            der = CONST(1);
            break;

        case TYPE_OP:
            switch (node->data) {

                case OP_ADD:
                    der = ADD(d(L), d(R));
                    break;

                case OP_SUB:
                    der = SUB(d(L), d(R));
                    break;

                case OP_MUL:
                    der = ADD(MUL(d(L), c(R)), MUL(c(L), d(R)));
                    break;

                case OP_DIV:
                    der = DIV(SUB(MUL(d(L), c(R)), MUL(c(L), d(R))), MUL(c(R), c(R)));
                    break;

                case OP_SIN:
                    der = MUL(COS(c(R)), d(R));
                    break;

                case OP_COS:
                    der = MUL(MUL(SIN(c(R)), CONST(-1)), d(R));
                    break;

                case OP_TAN:
                    der = DIV(d(R), POW(COS(c(R)), CONST(2)));
                    break;

                case OP_LN:
                    der = DIV(d(R), c(R));
                    break;

                case OP_EXP:
                    der = MUL(CopyTree(node), d(R));
                    break;

                case OP_POW: {
                    bool is_var_left = FindVar(node->left);
                    bool is_var_right = FindVar(node->right);

                    if (is_var_left == true && is_var_right == true) {
                        der = Differentiate(EXP(MUL(c(R), LN(c(L)))));
                    }
                    else if (is_var_left == false && is_var_right == true) {
                        der = MUL(MUL(CopyTree(node), LN(c(L))), d(R));
                    }
                    else if (is_var_left == true && is_var_right == false) {
                        der = MUL(MUL(c(R), POW(c(L), SUB(c(R), CONST(1)))), d(L));
                    }
                    else {
                        der = CONST(0);
                    }

                    break;
                }

                default:
                    fprintf(stderr, "Could not find operation %d\n", node->data);
                    return nullptr;
            }
            break;

        default:
            fprintf(stderr, "Could not differentiate node with type %d", node->type);
            return nullptr;
    }

    TexDiffBefore(node);
    TexDiffAfter(der);

    return der;
}

void MakeDerivative(Tree* tree) {
    assert(tree != nullptr);

    BeginReport(tree);

    Node* new_root = Differentiate(tree->root);

    TreeClear(tree);
    tree->root = new_root;
    SetParents(tree->root);

    Simplify(tree);

    EndReport(tree);
}

void Simplify(Tree* tree) {
    assert(tree != nullptr);

    TexSimplifyBegin(tree);
    int is_changed = 0;

    do {
        is_changed = 0;
        DeleteNeutrals(tree, tree->root, &is_changed);
    } while (is_changed != 0);
}

void DeleteNeutrals(Tree* tree, Node* node, int* is_changed) {
    if (node == nullptr) {
        return;
    }

    DeleteNeutrals(tree, node->left, is_changed);
    DeleteNeutrals(tree, node->right, is_changed);

    if (node->type == TYPE_OP) {
        switch (node->data) {
            case OP_ADD:

                DelAdd(tree, node, is_changed);
                break;

            case OP_SUB:
                DelSub(tree, node, is_changed);
                break;

            case OP_MUL:
                DelMul(tree, node, is_changed);
                break;

            case OP_DIV:
                DelDiv(tree, node, is_changed);
                break;

            case OP_POW:
                DelPow(tree, node, is_changed);
                break;

            case OP_LN:
                DelLn(tree, node, is_changed);
                break;

            case OP_EXP:
                DelExp(tree, node, is_changed);
                break;
        }
    }

}

void RebindNode(Tree* tree, Node* node, char side) {
    assert(node != nullptr);

    TexSimplifyBefore(node);

    if (node->parent == nullptr) {
        if (side == 'r') {
            tree->root = node->right;
        }
        else {
            tree->root = node->left;
        }
        TexSimplifyAfter(tree->root);
    }
    else if (node->parent->left == node) {
        if (side == 'r') {
            node->parent->left = node->right;
        }
        else {
            node->parent->left = node->left;
        }
        TexSimplifyAfter(node->parent->left);
    }
    else {
        if (side == 'r') {
            node->parent->right = node->right;
        }
        else {
            node->parent->right = node->left;
        }
        TexSimplifyAfter(node->parent->right);
    }
}

void MakeList(Node* node, type_t value) {
    TexSimplifyBefore(node);
    node->data = value;
    node->type = TYPE_CONST;
    node->left = nullptr;
    node->right = nullptr;
    TexSimplifyAfter(node);
}

void DelAdd(Tree* tree, Node* node, int* is_changed) {
    if (node != nullptr) {
        if (LT == TYPE_CONST && RT == TYPE_CONST) {
            *is_changed = 1;
            MakeList(node, LD + RD);
        }
        else if (LT == TYPE_CONST && LD == 0) {
            *is_changed = 1;
            RebindNode(tree, node, 'r');
        }
        else if (RT == TYPE_CONST && RD == 0) {
            *is_changed = 1;
            RebindNode(tree, node, 'l');
        }
    }
}

void DelSub(Tree* tree, Node* node, int* is_changed) {
    if (node != nullptr) {
        if (LT == TYPE_CONST && RT == TYPE_CONST) {
            *is_changed = 1;
            MakeList(node, LD - RD);
        }
        else if (LT == TYPE_CONST && LD == 0) {
            *is_changed = 1;
            RebindNode(tree, node, 'r');
        }
        else if (RT == TYPE_CONST && RD == 0) {
            *is_changed = 1;
            RebindNode(tree, node, 'l');
        }
    }
}

void DelMul(Tree* tree, Node* node, int* is_changed) {
    if (node != nullptr) {
        if (LT == TYPE_CONST && RT == TYPE_CONST) {
            *is_changed = 1;
            MakeList(node, LD * RD);
        }
        else if (LT == TYPE_CONST && LD == 1) {
            *is_changed = 1;
            RebindNode(tree, node, 'r');
        }
        else if (RT == TYPE_CONST && RD == 1) {
            *is_changed = 1;
            RebindNode(tree, node, 'l');
        }
        else if ((RT == TYPE_CONST && RD == 0) || (LT == TYPE_CONST && LD == 0)) {
            *is_changed = 1;
            MakeList(node, 0);
        }
    }
}

void DelDiv(Tree* tree, Node* node, int* is_changed) {
     if (node != nullptr) {
        if (RT == TYPE_CONST && RD == 0) {
            fprintf(stderr, "Devision by zero!!!!\n");
            return;
        }
        else if (LT == TYPE_CONST && RT == TYPE_CONST && (LD % RD == 0)) {
            *is_changed = 1;
            MakeList(node, LD / RD);
        }
        else if (RT == TYPE_CONST && RD == 1) {
            *is_changed = 1;
            RebindNode(tree, node, 'l');
        }
        else if (LT == TYPE_CONST && LD == 0) {
            *is_changed = 1;
            MakeList(node, 0);
        }
    }
}

void DelPow(Tree* tree, Node* node, int* is_changed) {
     if (node != nullptr) {
        if (LT == TYPE_CONST && LD == 0) {
            *is_changed = 1;
            MakeList(node, 0);
        }
        else if (LT == TYPE_CONST && LD == 1) {
            *is_changed = 1;
            MakeList(node, 1);
        }
        else if (RT == TYPE_CONST && RD == 0) {
            *is_changed = 1;
            MakeList(node, 1);
        }
        else if (RT == TYPE_CONST && RD == 1) {
            *is_changed = 1;
            RebindNode(tree, node, 'l');
        }
    }
}

void DelExp(Tree* tree, Node* node, int* is_changed) {
    if (node != nullptr) {
        if (RT == TYPE_CONST && RD == 0) {
            *is_changed = 1;
            MakeList(node, 1);
        }
    }
}

void DelLn(Tree* tree, Node* node, int* is_changed) {
    if (node != nullptr) {
        if (RT == TYPE_CONST && RD == 1) {
            *is_changed = 1;
            MakeList(node, 0);
        }
    }
}

#undef P node->parent
#undef L node->left
#undef R node->right
#undef RD node->right->data
#undef LD node->left->data
#undef LT node->left->type
#undef RT node->right->type
#undef NT node->type
#undef ND node->data

#undef d(node)
#undef c(node)
#undef CONST(value)
#undef ADD(left, right)
#undef SUB(left, right)
#undef MUL(left, right)
#undef DIV(left, right)
#undef COS(right)
#undef SIN(right)
#undef LN(right)
#undef EXP(right)
#undef POW(left, right)
#undef TAN(right)
