#pragma once

#include "TXLib.h"
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <locale.h>
#include <sys\stat.h>
#include <time.h>

#include "Messages.h"

#define DEFAULT_DUMP
#define DEFAULT_SAVE
#define DEFAULT_INPUT
#define DEFAULT_TEX

const char* my_dir = "C:\\Users\\VBAR\\Documents\\Programs_C\\Differentiator";
const char* output_tex = "report.tex";
FILE* fp_tex = nullptr;

typedef int64_t int_t;
typedef int type_t;

const int MAX_STDIN_INPUT_LENGTH = 1024;
const int USERS_INPUT_MAX_LENGTH = 30;
const int TEXT_MAX_LENGTH = 100;
const int MAX_VARIABLES_COUNT = 256;
const int UNARY_BEGIN = 5;

enum TYPE {
    TYPE_ERR   =-1,
    TYPE_NO    = 0,
    TYPE_VAR   = 1,
    TYPE_OP    = 2,
    TYPE_CONST = 3
};

enum OPERATIONS {
    OP_ERR = -1,
    OP_ADD =  0,
    OP_SUB =  1,
    OP_MUL =  2,
    OP_DIV =  3,
    OP_POW =  4,
    OP_SIN =  5,
    OP_COS =  6,
    OP_TAN =  7,
    OP_LN  =  8,
    OP_EXP =  9,
    OP_END
};

const int OPERATIONS_COUNT = OP_END;

static char* operation_names[] {
    "+",
    "-",
    "*",
    "/",
    "^",
    "sin",
    "cos",
    "tan",
    "ln",
    "exp",
};

typedef struct Input {
    char* data;
    int ofs;
} Input;

typedef struct Node {
    Node* left;
    Node* right;
    Node* parent;
    TYPE type;
    type_t data;
} Node;

typedef struct Tree {
    int* variables;
    Node* root;
} Tree;

enum ExitCodes {
    EXIT_OK = 0,
    BAD_INGAME_INPUT = 1,
    KEY_NOT_FOUND = 2,
};

//building tree and working with nodes
int FileLength(FILE* fp);
char* ReadFile(FILE* fp);
void ClearSpace(char* buffer, int size);
ExitCodes BuildTree(Tree* tree);
Node* SyntaxError(Input* input, char* message);
Node* GetNatural(Input* input);
Node* GetPrimary(Input* input);
Node* GetExpression(Input* input);
Node* GetTerminal(Input* input);
Node* GetGlobal(Input* input);
Node* GetPow(Input* input);
Node* GetDouble(Input* input);
int GetOperation(Input* input);
ExitCodes TreeClear(Tree* tree);
void PrintNodes(Node* node, FILE* fp);
int GetOperation(char* data);
void MakeNodes(Tree* tree, Node* node, char* buffer, int* pos);
TYPE GetData(char* buffer, int* pos, type_t* data);
void PutData(Tree* tree, Node* node, TYPE type, type_t data);
Node* NodeConstruct(TYPE type, type_t data, Node* parent, Node* left, Node* right);
void SetParents(Node* node);

//Dumping tree in GraphViz and LaTex
void MakeTex(Node* node, FILE* fp);
void TreeTexDump(Node* root, const char* file_name);
ExitCodes RunDump(Tree* tree);
void BeginReport(Tree* tree);
void TexSimplifyBefore(Node* node);
void TexSimplifyAfter(Node* node);
void EndReport(Tree* tree);
Node* TexDiffAfter(Node* node);
Node* TexDiffBefore(Node* node);
void TreeVisualDump(Node* root, const char* file_name);
void TexSimplifyBegin(Tree* tree);

//getting derivative and simplifying
Node* Differentiate(Node* node);
bool FindVar(Node* node);
void MakeDerivative(Tree* tree);
void DeleteNeutrals(Tree* tree, Node* node, int* is_changed);
void Simplify(Tree* tree);
void RebindNode(Tree* tree, Node* node, char side);
void MakeList(Node* node, type_t value);
void DelAdd(Tree* tree, Node* node, int* is_changed);
void DelSub(Tree* tree, Node* node, int* is_changed);
void DelMul(Tree* tree, Node* node, int* is_changed);
void DelDiv(Tree* tree, Node* node, int* is_changed);
void DelPow(Tree* tree, Node* node, int* is_changed);
void DelExp(Tree* tree, Node* node, int* is_changed);
void DelLn(Tree* tree, Node* node, int* is_changed);



