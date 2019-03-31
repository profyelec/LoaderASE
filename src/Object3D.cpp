/*
 * Object3D.cpp
 *
 *  Created on: Mar 31, 2019
 *      Author: tim
 */

#include "Object3D.h"
#include <cstdio>
#include <iostream>
#include <string>

Object3D::Object3D(char * filename)
{
  FILE *f = fopen(filename, "r");
  char lstr[1024];

  PState = psWaitKeyStart;
  Level.push_back(plRoot);

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

Object3D::ParseError_t Object3D::ProcessLine(char* str)
{
  ParseError_t lres = peOk;
  char c;
  std::map <std::string, DictStates_t>::const_iterator it;

  c = *str++;
  while (c && (lres == peOk))
  {
    switch(PState)
    {
    case psWaitKeyStart:
      if ((c != ' ') && (c != '\t') && (c != '\r') && (c != '\n') && (c != '*') && (c != '}'))
      {
        lres = peInvalidChar;
        continue;
      }
      else if (c == '*')
      {
        PState = psGetKeyName;
        KeyName = "";
      }
      else if (c == '}')
      {
        if (Level.back() != plRoot)
        {
          std::cout << "Close class" << std::endl;
          Level.pop_back();
        }
        else
        {
          lres = peInvalidLevel;
          continue;
        }
      }
      break;
    case psGetKeyName:
      if ((c == ' ') || (c == '\t'))
      {
        PState = psValidateKey;
        continue;
      }
      KeyName += c;
      break;

    case psValidateKey:
      std::cout << "Found a \"" << KeyName << "\"" << std::endl;
      it = NodesDict.find(KeyName);
      if (it != NodesDict.end())
      {
        Node = it->second.Node;
        PState = it->second.NewState;
        Value = "";
      }
      else
        lres = peUnknownKey;
      continue;
    case ps_Num:
      if (Value.length() && ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n')))
      {
        std::cout << "Value: " << Value << std::endl;
        switch (Level.back())
        {
        case plRoot:
          switch (Node)
          {
          case knExportVer:
            PState = psWaitKeyStart;
            continue;
          default:
            std::cout << "Invalid Node: " << Node << std::endl;
            lres = peInvalidNode;
            continue;
          }
          break;
        case plScene:
          switch (Node)
          {
          case knSceneFirstFrame:
            Scene->FirstFrame = std::stoi(Value);
            break;
          case knSceneLastFrame:
            Scene->LastFrame = std::stoi(Value);
            break;
          case knSceneFrameSpeed:
            Scene->FrameSpeed = std::stoi(Value);
            break;
          case knSceneTicksPerFrame:
            Scene->TicksPerFrame = std::stoi(Value);
            break;
          default:
            std::cout << "Invalid Node: " << Node << std::endl;
            lres = peInvalidNode;
            continue;
          }
          if (lres == peOk)
          {
            PState = psWaitKeyStart;
            continue;
          }
          break;
        default:
          std::cout << "Invalid level: " << Level.back() << std::endl;
          lres = peInvalidLevel;
          continue;
        }
        Value = "";
      }
      else if ((c != ' ') && (c != '\t'))
      {
        Value += c;
      }
      break;
    case ps_Str:
      if (!Value.length())
      {
        if ((c == ' ') || (c == '\t'))
          break;
        else if (c != '"')
          lres = peInvalidChar;
        else
          Value += c;
      }
      else if (c == '"')
      {
        // End of string
        Value.erase(0, 1);
        std::cout << "Value (str): " << Value << std::endl;
        switch (Level.back())
        {
        case plRoot:
          switch (Node)
          {
          case knComment:
            break;
          default:
            std::cout << "Invalid Node: " << Node << std::endl;
            lres = peInvalidNode;
            continue;
          }
          break;
        case plScene:
          switch (Node)
          {
          case knSceneFileName:
            Scene->FileName = Value;
            break;
          default:
            std::cout << "Invalid Node: " << Node << std::endl;
            lres = peInvalidNode;
            continue;
          }
          break;
        default:
          std::cout << "Invalid level: " << Level.back() << std::endl;
          lres = peInvalidLevel;
          continue;
        }
        Value = "";
        PState = psWaitKeyStart;
      }
      else
        Value += c;
      break;

    case ps_SubClass:
      if ((c == ' ') || (c == '\t'))
        break;
      if (c != '{')
        lres = peInvalidChar;
      else
      {
        std::cout << "Open class" << std::endl;
        switch(Node)
        {
        case knScene:
          Level.push_back(plScene);
          PState = psWaitKeyStart;
          Scene = new Scene_t;
          break;
        case knMaterialList:
          Level.push_back(plMaterialList);
          PState = psWaitKeyStart;
          // Create list or map materials?
          break;
        default:
          lres = peInvalidNode;
          break;
        }
      }
      break;
    case ps_Float:
    case ps_Float2:
    case ps_Float3:
      if (Value.length() && ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n')))
      {
        std::cout << "Value (float" << PState << "): " << Value << std::endl;
        switch (Level.back())
        {
        case plScene:
          switch (Node)
          {
          case knSceneBkgStatic:
            switch (PState)
            {
            case ps_Float:
              Scene->BkgStatic.B = std::stof(Value);
              PState = psWaitKeyStart;
              break;
            case ps_Float2:
              Scene->BkgStatic.G = std::stof(Value);
              PState = ps_Float;
              break;
            case ps_Float3:
              Scene->BkgStatic.R = std::stof(Value);
              PState = ps_Float2;
              break;
            }
            break;
          case knSceneAmbientStatic:
            switch (PState)
            {
            case ps_Float:
              Scene->AmbientStatic.B = std::stof(Value);
              PState = psWaitKeyStart;
              break;
            case ps_Float2:
              Scene->AmbientStatic.G = std::stof(Value);
              PState = ps_Float;
              break;
            case ps_Float3:
              Scene->AmbientStatic.R = std::stof(Value);
              PState = ps_Float2;
              break;
            }
            break;
          default:
            std::cout << "Invalid Node: " << Node << std::endl;
            lres = peInvalidNode;
            continue;
          }
          break;
        default:
          std::cout << "Invalid level: " << Level.back() << std::endl;
          lres = peInvalidLevel;
          continue;
        }
        Value = "";
      }
      else if ((c != ' ') && (c != '\t'))
      {
        Value += c;
      }
      break;

    default:
      std::cout << "Unknown state: " << PState << std::endl;
      lres = peUnknownState;
      continue;
    }

    c = *str++;
  }

  return lres;
}

Object3D::~Object3D()
{
  // TODO Auto-generated destructor stub
}

