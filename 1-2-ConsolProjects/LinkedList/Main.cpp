#include <cstdlib>
#include "Cell.h"

void main()
{
  Link pCell3 = new Cell(3, NULL);
  Link pCell2 = new Cell(2, pCell3);
  Link pCell1 = new Cell(1, pCell2);
}
