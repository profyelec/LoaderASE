//============================================================================
// Name        : ASEloader.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "Object3D.h"

Object3D * Test;

int main()
{
  Test = new Object3D("models/tank2.ASE");

  return 0;
}
