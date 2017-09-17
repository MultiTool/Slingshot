#include <iostream>

using namespace std;

#include "Matrix.hpp"

int main() {
  cout << "Hello world!" << endl;
  MatrixPtr mat; VectPtr vec0,vec1;
  mat = new Matrix(5,5);
  mat->Init_Identity();
  vec0 = new Vect(5);
  vec0->Print_Me(); printf("\n\n");
  vec1 = new Vect(5);
  mat->Print_Me();  printf("\n\n");
  if (false) {
    mat->Mutate();  mat->Print_Me();  printf("\n\n");
    mat->Mutate();  mat->Print_Me();  printf("\n\n");
    mat->Mutate();  mat->Print_Me();  printf("\n\n");
  }
  //mat->MultFire(vec0, vec1);
  mat->Iterate(vec0, vec1);
  vec1->Print_Me();

  delete mat; delete vec0; delete vec1;
  return 0;
}
