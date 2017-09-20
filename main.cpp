#include <iostream>

using namespace std;

//#include "Matrix.hpp"
//#include "Synapse.hpp"
#include "Node.hpp"
#include "Cluster.hpp"
#include "Pop.hpp"
#include "Stack.hpp"

int main() {
  cout << "Hello world!" << endl;
  Synapse syn();
  MatrixPtr mat; VectPtr vec0,vec1;
  mat = new Matrix(5,5);
  mat->Init_Identity();
  vec0 = new Vect(5);
  vec0->Print_Me(); printf("\n\n");
  vec1 = new Vect(5);
  mat->Print_Me();  printf("\n\n");
  if (false) {
    mat->Mutate_Me();  mat->Print_Me();  printf("\n\n");
    mat->Mutate_Me();  mat->Print_Me();  printf("\n\n");
    mat->Mutate_Me();  mat->Print_Me();  printf("\n\n");
  }

  mat->Iterate(vec0, vec1);
  vec1->Print_Me();

  delete mat; delete vec0; delete vec1;

  NodePtr nd = new Node();
  delete nd;

  return 0;
}
