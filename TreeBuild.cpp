#include "Differentiator_header.h"

#define SYMBOL input->data[input->ofs]


ExitCodes BuildTree(Tree* tree) {
    assert(tree != nullptr);

    char* file_name = (char*) calloc(100, sizeof(char));
    assert(file_name != nullptr);

    FILE* fp = nullptr;
    #ifdef DEFAULT_INPUT
    fp = stdin;
    #else
    fprintf(stderr, "Пожалуйста введите имя файла для загрузки\n");
    scanf("%s", file_name);

    FILE* fp = fopen(file_name, "r");
    if (fp == nullptr) {
        fprintf(stderr, "Файл ненайден, проверьте корректность имени\n");
        return EXIT_OK;
    }
    #endif

    Input input = {};
    input.data = ReadFile(fp);
    input.ofs = 0;

    TreeClear(tree);
    tree->root = GetGlobal(&input);

    SetParents(tree->root);

    return EXIT_OK;
}

Node* SyntaxError(Input* input, char* message) {
    assert(input != nullptr);

    fprintf(stderr, "Syntax error in pos %d\n", input->ofs);
    fprintf(stderr, "%s", message);
    fprintf(stderr, "%s\n", input->data);

    for (int i = 0; i < input->ofs; ++i) {
        fprintf(stderr, " ");
    }
    fprintf(stderr, "^\n");

    return nullptr;
}

Node* GetNatural(Input* input) {
    assert(input);
    int value = 0;

    int ofs_begin = input->ofs;
    while ('0' <= SYMBOL && '9' >= SYMBOL) {
        value = value * 10 + SYMBOL - '0';
        ++input->ofs;
    }
    if (ofs_begin == input->ofs) {
        return SyntaxError(input, "");
    }

    return NodeConstruct(TYPE_CONST, value, nullptr, nullptr, nullptr);
}

Node* GetPrimary(Input* input) {
    assert(input);

    if (SYMBOL == '(') {
        ++input->ofs;
        Node* value = GetExpression(input);
        if (value == nullptr) {
            return nullptr;
        }
        if (SYMBOL != ')') {
            return SyntaxError(input, "Required ')'\n");
        }
        input->ofs++;
        return value;
    }
    else if (isalpha(SYMBOL) != 0 && isalpha(input->data[input->ofs + 1]) == 0) {
        int variable = SYMBOL;
        if (variable != 'x') {
            return SyntaxError(input, "Variable name is not 'x', other names are not supported yet\n");
        }
        ++input->ofs;

        return NodeConstruct(TYPE_VAR, variable, nullptr, nullptr, nullptr);
    }
    else if (isalpha(SYMBOL) != 0) {
        int op_type = GetOperation(input);
        if (op_type == OP_ERR) {
            return SyntaxError(input, "Unknown operation\n");
        }

        Node* value = GetPrimary(input);
        if (value == nullptr) {
            return nullptr;
        }

        return NodeConstruct(TYPE_OP, op_type, nullptr, nullptr, value);
    }
    else {
        return GetDouble(input);
    }
}

Node* GetExpression(Input* input) {
    assert(input);

    Node* value = GetTerminal(input);
    if (value == nullptr) {
        return nullptr;
    }

    while (SYMBOL == '+' || SYMBOL == '-') {
        int operation = SYMBOL;
        ++input->ofs;
        Node* tmp_node = GetTerminal(input);
        if (tmp_node == nullptr) {
            return nullptr;
        }
        if (operation == '+') {
            value = NodeConstruct(TYPE_OP, OP_ADD, nullptr, value, tmp_node);
        }
        else {
            value = NodeConstruct(TYPE_OP, OP_SUB, nullptr, value, tmp_node);
        }
    }

    return value;
}

Node* GetTerminal(Input* input) {
    assert(input);

    Node* value = GetPow(input);
    if (value == nullptr) {
        return nullptr;
    }

    while (SYMBOL == '*' || SYMBOL == '/') {
        int operation = SYMBOL;
        ++input->ofs;
        Node* tmp_node = GetPow(input);
        if (tmp_node == nullptr) {
            return nullptr;
        }
        if (operation == '*') {
            value = NodeConstruct(TYPE_OP, OP_MUL, nullptr, value, tmp_node);
        }
        else {
            value = NodeConstruct(TYPE_OP, OP_DIV, nullptr, value, tmp_node);
        }
    }
    return value;
}

Node* GetGlobal(Input* input) {
    assert(input);

    Node* root = GetExpression(input);
    if (root == nullptr) {
        return nullptr;
    }

    if (SYMBOL != '\0') {
        return SyntaxError(input, "Expected end of expression\n");
    }

    return root;
}

Node* GetPow(Input* input) {
    assert(input);

    Node* value = GetPrimary(input);
    if (value == nullptr) {
        return nullptr;
    }

    if (SYMBOL == '^') {
        ++input->ofs;
        Node* tmp_node = GetPrimary(input);
        if (tmp_node == nullptr) {
            return nullptr;
        }
        return NodeConstruct(TYPE_OP, OP_POW, nullptr, value, tmp_node);
    }

    return value;
}

Node* GetDouble(Input* input) {
    assert(input);

    Node* decimal_part = GetNatural(input);
    if (decimal_part == nullptr) {
        return nullptr;
    }

    if (SYMBOL != '.') {
        return decimal_part;
    }
    ++input->ofs;
    int old_ofs = input->ofs;

    Node* fractial_part = GetNatural(input);
    if (fractial_part == nullptr) {
        return nullptr;
    }

    Node* value = NodeConstruct(TYPE_CONST, decimal_part->data + fractial_part->data / pow(10, input->ofs - old_ofs), nullptr, nullptr, nullptr);

    //del decimal_part
    //del fractial_part

    return value;
}

int GetOperation(Input* input) {

    char* tmp_func = (char*) calloc(100, sizeof(char));

    sscanf(input->data + input->ofs, "%[a-z]s", tmp_func);

    input->ofs += strlen(tmp_func); //+1

    for (int i = UNARY_BEGIN; i < OPERATIONS_COUNT; ++i) {
        if (strcmp(tmp_func, operation_names[i]) == 0) {
            free(tmp_func);
            return i;
        }
    }

    free(tmp_func);
    return OP_ERR;
}


