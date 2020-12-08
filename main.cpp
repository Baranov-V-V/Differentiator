#include "Differentiator_header.h"
#include "TreeBuild.cpp"
#include "Differentiator_func.cpp"
#include "ReadWrite_func.cpp"

int main() {
    Tree tree = {};
    TreeConstruct(&tree);

    BuildTree(&tree);
    MakeDerivative(&tree);

    TreeDestruct(&tree);
    return 0;
}
