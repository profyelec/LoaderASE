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

Object3D::Object3D(const std::string & filename)
{
  FILE *f = fopen(filename.c_str(), "r");
  char lstr[1024];
  ParseError_t err;

  PState = psWaitKeyStart;
  Level.push_back(plRoot);

  while (!feof(f))
  {
    if (fgets(lstr, sizeof(lstr), f))
      if ((err = ProcessLine(lstr)))
      {
        std::cout << "Error: ";
        switch (err)
        {
        case peInvalidChar:
          std::cout << "Invalid char";
          break;
        case peInvalidLevel:
          std::cout << "Invlaid level";
          break;
        case peInvalidNode:
          std::cout << "Invalid node";
          break;
        case peUnknownNode:
          std::cout << "Unknown node";
          break;
        case peUnknownState:
          std::cout << "Unknown state";
          break;
        default:
          break;
        }
        std::cout << std::endl << "Parse stopped in: " << lstr << std::endl;
        break;
      }
  }
  fclose(f);
}

Object3D::ParseError_t Object3D::ProcessLine(char* str)
{
  ParseError_t lres = peOk;
  std::map<std::string, DictStates_t>::const_iterator it;
  int index;
  char c;

  c = *str++;
  while (c && (lres == peOk))
  {
    switch (PState)
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
        NodeName = "";
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
      NodeName += c;
      break;

    case psValidateKey:
      std::cout << "Found a \"" << NodeName << "\"" << std::endl;
      it = NodesDict.find(NodeName);
      if (it != NodesDict.end())
      {
        Node = it->second.Node;
        PState = it->second.NewState;
        Value = "";
      }
      else
        lres = peUnknownNode;
      continue;
    case ps_Num:
      if (Value.length() && ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n')))
      {
        std::cout << "Value: " << Value << std::endl;
        index = 0;
        lres = OnSetNum(std::stoi(Value), index, Node, Level.back());
        Value = "";
        PState = psWaitKeyStart;
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
        lres = OnSetStr(Value, 0, Node, Level.back());
        Value = "";
        PState = psWaitKeyStart;
      }
      else
        Value += c;
      break;

    case ps_Enum:
      if (!Value.length())
      {
        if ((c == ' ') || (c == '\t'))
          break;
        Value += c;
      }
      else if ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'))
      {
        // End of string
        std::cout << "Value (str): " << Value << std::endl;
        lres = OnSetEnum(Value, 0, Node, Level.back());
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
        switch (Node)
        {
        case knScene:
          Level.push_back(plScene);
          PState = psWaitKeyStart;
          Scene = new Scene_t();
          break;
        case knMaterialList:
          Level.push_back(plMaterialList);
          PState = psWaitKeyStart;
          MaterialList = new MaterialList_t();
          break;
        case knMapDiffuse:
          Level.push_back(plMapDiffuse);
          PState = psWaitKeyStart;
          MaterialList->Material.back().second->DiffuseMap = new Map_t();
          break;
        default:
          lres = peInvalidNode;
          break;
        }
      }
      break;

    case ps_SubClassId:
      if (Value.length() && ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n') || (c == '{')))
      {
        if (c == '{')
        {
          std::cout << "Value: " << Value << std::endl;
          switch (Node)
          {
          case knMaterial:
            Level.push_back(plMaterial);
            break;
          default:
            lres = peInvalidNode;
            break;
          }
          if (lres == peOk)
            lres = OnSetSubClassID(std::stoi(Value), Node, Level.back());

          Value = "";
          PState = psWaitKeyStart;
        }
      }
      else if ((c != ' ') && (c != '\t'))
      {
        Value += c;
      }
      break;

    case ps_Float:
    case ps_Float2:
    case ps_Float3:
      if (Value.length() && ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n')))
      {
        std::cout << "Value (float" << PState << "): " << Value << std::endl;

        index = 0;
        if (PState == ps_Float)
          PState = psWaitKeyStart;
        switch (Level.back())
        {
        case plScene:
        case plMaterial:
          switch (Node)
          {
          case knSceneBkgStatic:
          case knSceneAmbientStatic:
          case knMaterialAmbient:
          case knMaterialDiffuse:
          case knMaterialSpecular:
            switch (PState)
            {
            case ps_Float3:
              index = 0;
              PState = ps_Float2;
              break;
            case ps_Float2:
              index = 1;
              PState = ps_Float;
              break;
            case ps_Float:
              index = 2;
              break;
            default:
              break;
            }
            break;
          default:
            break;
          }
          break;
        default:
          break;
        }

        lres = OnSetFloat(std::stof(Value), index, Node, Level.back());
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

Object3D::ParseError_t Object3D::OnSetNum(int value, int index, KeyNodes_t node, ParseLevel_t level)
{
  ParseError_t err = peOk;
  switch (level)
  {
  case plRoot:
    switch (node)
    {
    case knExportVer:
      break;
    default:
      err = peInvalidNode;
      break;
    }
    break;
  case plScene:
    switch (Node)
    {
    case knSceneFirstFrame:
      Scene->FirstFrame = value;
      break;
    case knSceneLastFrame:
      Scene->LastFrame = value;
      break;
    case knSceneFrameSpeed:
      Scene->FrameSpeed = value;
      break;
    case knSceneTicksPerFrame:
      Scene->TicksPerFrame = value;
      break;
    default:
      err = peInvalidNode;
      break;
    }
    break;
  case plMaterialList:
    switch (Node)
    {
    case knMaterialCount:
      MaterialList->Count = value;
      break;
    default:
      err = peInvalidNode;
      break;
    }
    break;
  case plMaterial:
    switch (node)
    {
    default:
      err = peInvalidNode;
      break;
    }
    break;
  case plMapDiffuse:
    switch (node)
    {
    case knMapSubNo:
      MaterialList->Material.back().second->DiffuseMap->SubNo = value;
      break;
    default:
      err = peInvalidNode;
      break;
    }
    break;
  default:
    err = peInvalidLevel;
    break;
  }

  return err;
}

Object3D::ParseError_t Object3D::OnSetStr(const std::string value, int index, KeyNodes_t node, ParseLevel_t level)
{
  ParseError_t err = peOk;

  switch (level)
  {
  case plRoot:
    switch (Node)
    {
    case knComment:
      break;
    default:
      err = peInvalidNode;
      break;
    }
    break;
  case plScene:
    switch (Node)
    {
    case knSceneFileName:
      Scene->FileName = value;
      break;
    default:
      err = peInvalidNode;
      break;
    }
    break;
  case plMaterial:
    switch (node)
    {
    case knMaterialName:
      MaterialList->Material.back().second->Name = value;
      break;
    case knMaterialClass:
      MaterialList->Material.back().second->Class = value;
      break;
    default:
      err = peUnknownNode;
      break;
    }
    break;
  case plMapDiffuse:
    switch (node)
    {
    case knMapName:
      MaterialList->Material.back().second->DiffuseMap->Name = value;
      break;
    case knMapClass:
      MaterialList->Material.back().second->DiffuseMap->Class = value;
      break;
    case knMapBitmap:
      MaterialList->Material.back().second->DiffuseMap->Bitmap = value;
      break;
    default:
      err = peUnknownNode;
      break;
    }
    break;
  default:
    err = peInvalidLevel;
    break;
  }

  return err;
}

Object3D::ParseError_t Object3D::OnSetFloat(float value, int index, KeyNodes_t node, ParseLevel_t level)
{
  ParseError_t err = peOk;

  switch (level)
  {
  case plScene:
    switch (node)
    {
    case knSceneBkgStatic:
      switch (index)
      {
      case 0:
        Scene->BkgStatic.B = value;
        break;
      case 1:
        Scene->BkgStatic.G = value;
        break;
      case 2:
        Scene->BkgStatic.R = value;
        break;
      }
      break;
    case knSceneAmbientStatic:
      switch (index)
      {
      case 0:
        Scene->AmbientStatic.B = value;
        break;
      case 1:
        Scene->AmbientStatic.G = value;
        break;
      case 2:
        Scene->AmbientStatic.R = value;
        break;
      }
      break;
    default:
      err = peInvalidNode;
      break;
    }
    break;
  case plMaterial:
    switch (node)
    {
    case knMaterialAmbient:
      switch (index)
      {
      case 0:
        MaterialList->Material.back().second->Ambient.B = value;
        break;
      case 1:
        MaterialList->Material.back().second->Ambient.G = value;
        break;
      case 2:
        MaterialList->Material.back().second->Ambient.R = value;
        break;
      }
      break;
    case knMaterialDiffuse:
      switch (index)
      {
      case 0:
        MaterialList->Material.back().second->Diffuse.B = value;
        break;
      case 1:
        MaterialList->Material.back().second->Diffuse.G = value;
        break;
      case 2:
        MaterialList->Material.back().second->Diffuse.R = value;
        break;
      }
      break;
    case knMaterialSpecular:
      switch (index)
      {
      case 0:
        MaterialList->Material.back().second->Specular.B = value;
        break;
      case 1:
        MaterialList->Material.back().second->Specular.G = value;
        break;
      case 2:
        MaterialList->Material.back().second->Specular.R = value;
        break;
      }
      break;
    case knMaterialShine:
      MaterialList->Material.back().second->Shine = value;
      break;
    case knMaterialShineStrength:
      MaterialList->Material.back().second->ShineStrength = value;
      break;
    case knMaterialTransparency:
      MaterialList->Material.back().second->Transparency = value;
      break;
    case knMaterialWiresize:
      MaterialList->Material.back().second->Wiresize = value;
      break;
    case knMaterialXPFalloff:
      MaterialList->Material.back().second->XPFalloff = value;
      break;
    case knMaterialSelfIllum:
      MaterialList->Material.back().second->SelfIllum = value;
      break;
    default:
      err = peInvalidNode;
      break;
    }
    break;
  case plMapDiffuse:
    switch (node)
    {
    case knMapAmount:
      MaterialList->Material.back().second->DiffuseMap->Amount = value;
      break;
    default:
      err = peInvalidNode;
      break;
    }
    break;

  default:
    err = peInvalidLevel;
    break;
  }

  return err;
}

Object3D::ParseError_t Object3D::OnSetSubClassID(int value, KeyNodes_t node, ParseLevel_t level)
{
  ParseError_t err = peOk;

  switch (level)
  {
  case plMaterial:
    switch (Node)
    {
    case knMaterial:
      MaterialList->Material.push_back(std::make_pair(value, new Material_t()));
      break;
    default:
      err = peInvalidNode;
      break;
    }
    break;
  default:
    err = peInvalidLevel;
    break;
  }

  return err;
}

Object3D::ParseError_t Object3D::OnSetEnum(const std::string value, int index, KeyNodes_t node, ParseLevel_t level)
{
  ParseError_t err = peOk;

  switch (level)
  {
  case plMaterial:
    switch (node)
    {
    case knMaterialShading:
      MaterialList->Material.back().second->Shading = Value;
      break;
    case knMaterialFalloff:
      MaterialList->Material.back().second->Falloff = Value;
      break;
    case knMaterialXPType:
      MaterialList->Material.back().second->XPType = Value;
      break;
    default:
      err = peUnknownNode;
      break;
    }
    break;
  default:
    err = peInvalidLevel;
    break;
  }

  return err;
}

Object3D::~Object3D()
{
  // TODO Auto-generated destructor stub
}

