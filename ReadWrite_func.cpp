#include "Differentiator_header.h"

int FileLength(FILE* fp) {
    assert(fp != nullptr);

    fseek(fp, 0, SEEK_END);
    int pos = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    return pos;
}

char* ReadFile(FILE* fp) {
    assert(fp != nullptr);

    #ifndef DEFAULT_INPUT
    int text_size = FileLength(fp);
    char* buffer = (char*) calloc(text_size, sizeof(char));
    assert(buffer != nullptr);

    int real_text_size = fread(buffer, sizeof(char), text_size, fp);
    buffer[real_text_size] = '\0';
    #else
    char* buffer = (char*) calloc(MAX_STDIN_INPUT_LENGTH, sizeof(char));
    fgets(buffer, MAX_STDIN_INPUT_LENGTH, stdin);
    int real_text_size = strlen(buffer);
    buffer[real_text_size - 1] = '\0';
    #endif

    ClearSpace(buffer, real_text_size + 1);

    return buffer;
}

void ClearSpace(char* buffer, int size) {
    for (int i = 0, j = 0; i < size; ++i) {
        if (buffer[i] != ' ' && buffer[i] != '\n') {
            buffer[j++] = buffer[i];
        }
    }
}

ExitCodes MakeData(Tree* tree) {
    assert(tree != nullptr);

    char* file_name = (char*) calloc(100, sizeof(char));
    assert(file_name != nullptr);
    #ifdef DEFAULT_INPUT
    strcpy(file_name, "line.txt");
    #else
    fprintf(stderr, "Пожалуйста введите имя файла для загрузки\n");
    scanf("%s", file_name);
    #endif

    FILE* fp = fopen(file_name, "r");
    if (fp == nullptr) {
        fprintf(stderr, "Файл ненайден, проверьте корректность имени\n");
        return EXIT_OK;
    }

    char* buffer = (char*) calloc(10000, sizeof(char)); //заменить на конст
    //ReadFile(fp, buffer);

    TreeClear(tree);
    int pos = 0;

    tree->root = NodeConstructEmpty();

    ++pos;
    MakeNodes(tree, tree->root, buffer, &pos);

    return EXIT_OK;
}

void MakeNodes(Tree* tree, Node* node, char* buffer, int* pos) {
    assert(tree != nullptr);
    assert(node != nullptr);
    assert(buffer != nullptr);

    printf("pos: %d\n", *pos);

    while (buffer[*pos] == '(') {
        ++(*pos);
        node->left = NodeConstructEmpty();
        node->left->parent = node;
        node = node->left;
    }

    if (buffer[*pos] != '(' && buffer[*pos] != '\0') {  //buffer[*pos] != ')' &&
        type_t data_tmp = 0;
        TYPE type = GetData(buffer, pos, &data_tmp);
        if (type == TYPE_ERR) {
            return;
        }

        if (type == TYPE_NO) {
            printf("typeno\n");
            node = node->parent;
            ++(*pos);
            //NodeDestruct(node->left);
            node->left = nullptr;
        }
        else {
            PutData(tree, node, type, data_tmp);
        }
    }

    while (buffer[*pos] == ')') {
        ++(*pos);
        node = node->parent;
    }

    if (buffer[*pos] != ')' && buffer[*pos] != '(' && buffer[*pos] != '\0') {
        type_t data_tmp = 0;
        TYPE type = GetData(buffer, pos, &data_tmp);
        if (type == TYPE_ERR) {
            return;
        }

        if (type == TYPE_NO) {
            node = node->parent;
            ++(*pos);
            //NodeDestruct(node->left);
            node->left = nullptr;
        }
        else {
            PutData(tree, node, type, data_tmp);
        }
    }

    if (buffer[*pos] != '\0') {
        ++(*pos);
        node->right = NodeConstructEmpty();
        node->right->parent = node;
        MakeNodes(tree, node->right, buffer, pos);
    }
}

void PutData(Tree* tree, Node* node, TYPE type, type_t data) {
    assert(node != nullptr);

    node->data = data;
    node->type = type;

}

TYPE GetData(char* buffer, int* pos, type_t* data) {
    assert(buffer != nullptr);
    assert(pos != nullptr);
    assert(data != nullptr);

    char* tmp_data = (char*) calloc(10, sizeof(char));
    int symblos_read = 0;
    sscanf(buffer + *pos, "%[^()]s", tmp_data);
    //printf("scanned: <%s>\n", tmp_data); // debug
    int data_len = strlen(tmp_data);
    *pos += data_len;

    if (data_len == 0) { //nothing
        *data = -1;
        free(tmp_data);
        return TYPE_NO;
    }
    else if (data_len == 1 && isalpha(tmp_data[0])) { //variable
        *data = tmp_data[0];
        free(tmp_data);
        return TYPE_VAR;
    }
    else if (isdigit(tmp_data[0])) { //const
        free(tmp_data);
        *data = atoi(tmp_data);
        return TYPE_CONST;
    }
    else {  //operation
        *data = GetOperation(tmp_data);
        free(tmp_data);
        if (*data == OP_ERR) {
            fprintf(stderr, "Unknonw operation: <%s>", tmp_data);
            return TYPE_ERR;
        }
        return TYPE_OP;
    }
}

int GetOperation(char* data) {
    for (int i = 0; i < OPERATIONS_COUNT; ++i) {
        if (strcmp(data, operation_names[i]) == 0) {
            return i;
        }
    }

    return OP_ERR;
}

ExitCodes SaveData(Tree* tree) {
    assert(tree != nullptr);

    char* file_name = (char*) calloc(100, sizeof(char));
    assert(file_name != nullptr);
    #ifdef DEFAULT_SAVE
        strcpy(file_name, "Tree_save.txt");
    #else
    PrintAndSpeak("Пожалуйста введите имя файла для сохранения\n");
    scanf("%s", file_name);
    #endif

    FILE* fp = fopen(file_name, "w");
    assert(fp != nullptr);

    fprintf(fp, "(");
    PrintNodes(tree->root, fp);
    fprintf(fp, ")");

    free(file_name);
    fclose(fp);

    return EXIT_OK;
}

void PrintNodes(Node* node, FILE* fp) {
    assert(node != nullptr);
    assert(fp != nullptr);

    if (node->left != nullptr) {
        fprintf(fp, "(");
        PrintNodes(node->left, fp);
        fprintf(fp, ")");
    }

    switch (node->type) {
        case TYPE_NO:
            fprintf(fp, "nan");
            break;

        case TYPE_VAR:
            fprintf(fp, "%c", node->data);
            break;

        case TYPE_CONST:
            fprintf(fp, "%d", node->data);
            break;

        case TYPE_OP:
            if (node->data >= UNARY_BEGIN) {
                fprintf(fp, "()");
            }
            fprintf(fp, "%s", operation_names[node->data]);
            break;

        default:
            fprintf(stderr, "Unknown node type %d", node->type);
            break;

    }

    if (node->right != nullptr) {
        fprintf(fp, "(");
        PrintNodes(node->right, fp);
        fprintf(fp, ")");
    }
}

void TreeVertexPrint(Node* node, FILE* fp) {
    assert(fp != nullptr);

    if (node != nullptr) {
        TreeVertexPrint(node->left, fp);
        switch (node->type) {
            case TYPE_NO:
                fprintf(fp, "node%p [label =\"<nothing> EMPTY NODE\", color=\"grey14\", fillcolor=\"crimson\"]\n",
                        node);
                break;

            case TYPE_VAR:
                fprintf(fp, "node%p [label =\"<var> %c\", color=\"grey14\", fillcolor=\"mediumseagreen\"]\n",
                        node, node->data);
                break;

            case TYPE_CONST:
                fprintf(fp, "node%p [label =\"<const> %d\", color=\"grey14\", fillcolor=\"purple\"]\n",
                        node, node->data);
                break;

            case TYPE_OP:
                if (node->data >= UNARY_BEGIN) {
                    fprintf(fp, "node%p [label =\"<op> %s\", color=\"grey14\", fillcolor=\"skyblue\"]\n",
                            node, operation_names[node->data]);
                }
                else {
                    fprintf(fp, "node%p [label =\"<op> %s\", color=\"grey14\", fillcolor=\"mediumturquoise\"]\n",
                            node, operation_names[node->data]);
                }
                break;

            default:
                fprintf(stderr, "Unknown node type %d", node->type);
                break;

        }
        TreeVertexPrint(node->right, fp);
    }
}

void TreeEdgesPrint(Node* node, FILE* fp) {
    assert(fp != nullptr);

    if (node != nullptr) {
        TreeEdgesPrint(node->left, fp);
        if (node->left != nullptr) {
            fprintf(fp, "    node%p:sw -> node%p [color=\"navy\"];\n", node, node->left);
        }
        if (node->right != nullptr) {
            fprintf(fp, "    node%p:se -> node%p [color=\"crimson\"];\n", node, node->right);
        }
        TreeEdgesPrint(node->right, fp);
    }
}

void TreeVisualDump(Node* root, const char* file_name) {
    assert(root != NULL);
    assert(file_name != NULL);

    char* command_dot = (char*) calloc(100, sizeof(char));
    char* graph_file  = (char*) calloc(strlen(file_name) + 10, sizeof(char));

    strcpy(graph_file, file_name);
    strcat(graph_file, ".dot");
    FILE* fp = fopen(graph_file, "w");
    assert(fp != NULL);

    strcat(command_dot, "dot -Tpng ");   //graph_test1.dot -o Demo3.png"
    strcat(command_dot, graph_file);
    //strcat(command_dot, " -Gcharset = latin1");
    strcat(command_dot, " -o ");
    strcat(command_dot, file_name);
    strcat(command_dot, "_graph.png");

    fprintf(fp, "digraph structs {\n        node [shape=\"record\", style=\"filled\"];");  //rankdir = LR\n

    TreeVertexPrint(root, fp);
    TreeEdgesPrint(root, fp);

    fprintf(fp, "}");

    fclose(fp);

    fprintf(stderr, "graphing done with code: %d\n", system(command_dot));
}

ExitCodes RunDump(Tree* tree) {
    assert(tree != nullptr);

    char* file_name = (char*) calloc(100, sizeof(char));
    assert(file_name != nullptr);

    #ifdef DEFAULT_DUMP
        strcpy(file_name, "Tree_dump");
    #else
    printf("Пожалуйста введите имя файла для дампа\n");
    scanf("%s", file_name);
    #endif

    TreeVisualDump(tree->root, file_name);

    free(file_name);
    return EXIT_OK;
}

ExitCodes TexDump(Tree* tree) {
    assert(tree != nullptr);

    char* file_name = (char*) calloc(100, sizeof(char));
    assert(file_name != nullptr);

    #ifdef DEFAULT_TEX
        strcpy(file_name, "Formula");
    #else
    printf("Пожалуйста введите имя файла для дампа\n");
    scanf("%s", file_name);
    #endif

    TreeTexDump(tree->root, file_name);

    free(file_name);
    return EXIT_OK;
}

void TreeTexDump(Node* root, const char* file_name) {
    assert(root != NULL);
    assert(file_name != NULL);

    char* command_tex = (char*) calloc(200, sizeof(char));
    char* tex_file  = (char*) calloc(strlen(file_name) + 10, sizeof(char));
    char* new_dir   = (char*) calloc(200, sizeof(char));

    strcpy(new_dir, "C:\\Users\\VBAR\\Documents\\Programs_C\\Differentiator\\tmp");

    strcpy(command_tex, "pdflatex -aux-directory=");
    strcat(command_tex, new_dir);
    strcat(command_tex, " ");
    strcat(command_tex, output_tex);

    system(command_tex);
}

void BeginReport(Tree* tree) {
    assert(tree != nullptr);

    fp_tex = fopen(output_tex, "w");
    assert(fp_tex != nullptr);

    fprintf(fp_tex, "\\documentclass{article}\n\n\\usepackage{amsmath}\n\\usepackage{xcolor}\n\\usepackage[utf8]{inputenc}\n\\usepackage[T1]{fontenc}\n\\usepackage{breqn}\n");
    fprintf(fp_tex, "\\usepackage{vmargin}\n\\usepackage{hyperref}\n\\definecolor{urlcolor}{HTML}{330B03}\n\\hypersetup{pdfstartview=FitH,  linkcolor=linkcolor,urlcolor=urlcolor, colorlinks=true}\n\\setmarginsrb{3 cm}{2.5 cm}{3 cm}{2.5 cm}{1 cm}{1.5 cm}{1 cm}{1.5 cm}\n\\begin{document}\n");

    fprintf(fp_tex, "\\begin{center}\n{\\LARGE %s}\n\n\\end{center}\n", intro_message_begin);
    fprintf(fp_tex, "\\begin{center}\n{\\LARGE \\href{https://vk.com/baranov_v_v}{\\underline{%s}}}\n\n\\end{center}\n", initials_message);

    fprintf(fp_tex, "%s\n\n", intro_message_description);
    fprintf(fp_tex, "\\begin{center}\n{\\Large %s}\n\n", intro_message_end);

    fprintf(fp_tex, "\\begin{dmath}\n y = ");
    MakeTex(tree->root, fp_tex);
    fprintf(fp_tex, "\n\\end{dmath}\n");
};

void TexSimplifyBefore(Node* node) {
    fprintf(fp_tex, "%s:\n", simplify_messages_begin[rand() % simplify_messages_begin_count]);

    fprintf(fp_tex, "\\begin{dmath}\n y = ");
    MakeTex(node, fp_tex);
    fprintf(fp_tex, "\n\\end{dmath}\n");
};

void TexSimplifyAfter(Node* node) {
    fprintf(fp_tex, "%s:\n", simplify_messages_end[rand() % simplify_messages_end_count]);

    fprintf(fp_tex, "\\begin{dmath}\n y = ");
    MakeTex(node, fp_tex);
    fprintf(fp_tex, "\n\\end{dmath}\n");
};

void TexSimplifyBegin(Tree* tree) {
    assert(tree != nullptr);

    fprintf(fp_tex, "{\\LARGE}%s:\n", before_simplify_message);

    fprintf(fp_tex, "\\begin{dmath}\n y' = ");
    MakeTex(tree->root, fp_tex);
    fprintf(fp_tex, "\n\\end{dmath}\n");
};

Node* TexDiffBefore(Node* node) {
    fprintf(fp_tex, "%s\n", before_differentiation_messages[rand() % before_differentiation_messages_count]);

    fprintf(fp_tex, "\\begin{dmath}\n y = ");
    MakeTex(node, fp_tex);
    fprintf(fp_tex, "\n\\end{dmath}\n");

    return node;
};

Node* TexDiffAfter(Node* node) {
    fprintf(fp_tex, "%s\n", after_differentiation_messages[rand() % after_differentiation_messages_count]);

    fprintf(fp_tex, "\\begin{dmath}\n \\frac{dy}{dx} = ");
    MakeTex(node, fp_tex);
    fprintf(fp_tex, "\n\\end{dmath}\n");

    return node;
};

void EndReport(Tree* tree) {
    fprintf(fp_tex, "%s\n\n", outro_message);

    fprintf(fp_tex, "\\begin{dmath}\n y' = ");
    MakeTex(tree->root, fp_tex);
    fprintf(fp_tex, "\n\\end{dmath}\n\n{\\Large %s}\n\\end{center}\n", literature_message);

    fprintf(fp_tex, "\\begin{enumerate}\n");
    fprintf(fp_tex, "\\item %s\n", references_message1);
    fprintf(fp_tex, "\\item %s\n", references_message2);
    fprintf(fp_tex, "\\end{enumerate}\n");
    fprintf(fp_tex, "My \\href{https://vk.com/baranov_v_v}{\\underline{github}} repository");

    fprintf(fp_tex, "%s\n", references_thanks);

    fprintf(fp_tex, "\n\\end{document}");

    fclose(fp_tex);

    TreeTexDump(tree->root, output_tex);
};

void MakeTex(Node* node, FILE* fp) {
    assert(fp != nullptr);
    if (node == nullptr) {
        return;
    }

    switch (node->type) {
        case TYPE_VAR:
            fprintf(fp, "%c", node->data);
            break;

        case TYPE_CONST: {
            if (node->data < 0) {
                fprintf(fp, "(%d)", node->data);
            }
            else {
                fprintf(fp, "%d", node->data);
            }
            break;
            }

        case TYPE_OP: {
            switch (node->data) {
                case OP_ADD:
                    MakeTex(node->left, fp);
                    fprintf(fp, "+");
                    MakeTex(node->right, fp);
                    break;

                case OP_SUB:
                    MakeTex(node->left, fp);
                    fprintf(fp, "-");
                    MakeTex(node->right, fp);
                    break;

                case OP_MUL:
                    if (node->left->type == TYPE_OP && (node->left->data == OP_ADD || node->left->data == OP_SUB)) {
                        fprintf(fp, "(");
                        MakeTex(node->left, fp);
                        fprintf(fp, ")");
                    }
                    else {
                        MakeTex(node->left, fp);
                    }
                    fprintf(fp, "\\cdot ");
                    if (node->right->type == TYPE_OP && (node->right->data == OP_ADD || node->right->data == OP_SUB)) {
                        fprintf(fp, "(");
                        MakeTex(node->right, fp);
                        fprintf(fp, ")");
                    }
                    else {
                        MakeTex(node->right, fp);
                    }
                    break;

                case OP_DIV:
                    fprintf(fp, "\\dfrac");
                    fprintf(fp, "{");
                    MakeTex(node->left, fp);
                    fprintf(fp, "}");
                    fprintf(fp, "{");
                    MakeTex(node->right, fp);
                    fprintf(fp, "}");
                    break;

                case OP_POW:
                    if (node->left->type == TYPE_CONST || node->left->type == TYPE_VAR) {
                        MakeTex(node->left, fp);
                    }
                    else {
                        fprintf(fp, "(");
                        MakeTex(node->left, fp);
                        fprintf(fp, ")");
                    }

                    fprintf(fp, "^");

                    fprintf(fp, "{");
                    MakeTex(node->right, fp);
                    fprintf(fp, "}");
                    break;

                case OP_LN:
                    fprintf(fp, "\\ln ");
                    fprintf(fp, "(");
                    MakeTex(node->right, fp);
                    fprintf(fp, ")");
                    break;

                case OP_EXP:
                    fprintf(fp, "e^");
                    fprintf(fp, "{");
                    MakeTex(node->right, fp);
                    fprintf(fp, "}");
                    break;

                case OP_SIN:
                    fprintf(fp, "\\sin (");
                    MakeTex(node->right, fp);
                    fprintf(fp, ")");
                    break;

                case OP_COS:
                    fprintf(fp, "\\cos (");
                    MakeTex(node->right, fp);
                    fprintf(fp, ")");
                    break;

                case OP_TAN:
                    fprintf(fp, "\\tan (");
                    MakeTex(node->right, fp);
                    fprintf(fp, ")");
            }
            break;
        }

        default:
            fprintf(stderr, "Unknown node type %d", node->type);
            exit(1);
            break;

    }
}
