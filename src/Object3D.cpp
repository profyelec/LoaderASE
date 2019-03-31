/*
 * Object3D.cpp
 *
 *  Created on: Mar 31, 2019
 *      Author: tim
 */

#include "Object3D.h"
#include <cstdio>
#include "iostream"

Object3D::Object3D(char * filename)
{
  FILE *f = fopen(filename, "r");
  char lstr[1024];

  PState = psWaitKeyStart;

  while (!feof(f))
  {
    if (fgets(lstr, sizeof(lstr), f))
      if (ProcessLine(lstr))
      {
        std::cout << "Error parse in: " << lstr << std::endl;
        break;
      }
  }
  fclose(f);
}

int Object3D::ProcessLine(char* str)
{
  char c;

  while (*str)
  {
    c = *str++;
    switch(PState)
    {
    case psWaitKeyStart:
      if ((c != ' ') && (c != '\t') && (c != '\r') && (c != '\n') && (c != '*'))
        return 1;
      if (c == '*')
      {
        PState = psGetKeyName;
        KeyName = "";
      }
      break;
    case psGetKeyName:
      if ((c != ' ') && (c != '\t'))
      {
        KeyName += c;
        break;
      }
      PState = psValidateKey;
      /* no break */
    case psValidateKey:
      std::cout << "Found a \"" << KeyName << "\"" << std::endl;
      if (StateDict.find(KeyName) != StateDict.end())
        PState = StateDict.find(KeyName)->second;
      else
        return 2;
      break;
    case psExportVer:
    case psComment:
    case psScene:
      if ((c == '\r') || (c == '\n'))
      {
        PState = psWaitKeyStart;
        return 0;
      }
      break;
    }
  }

  return 0;
}

Object3D::~Object3D()
{
  // TODO Auto-generated destructor stub
}

