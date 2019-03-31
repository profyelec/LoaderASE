/*
 * Object3D.h
 *
 *  Created on: Mar 31, 2019
 *      Author: tim
 */

#ifndef OBJECT3D_H_
#define OBJECT3D_H_

#include <string>
#include <map>
#include <list>
#include <vector>

class Scene_t
{
public:
  std::string FileName;
  int FirstFrame;
  int LastFrame;
  int FrameSpeed;
  int TicksPerFrame;
  struct{
    float R;
    float G;
    float B;
  } BkgStatic;
  struct{
    float R;
    float G;
    float B;
  } AmbientStatic;
};

class Material_t
{
public:
  std::string Name;
  std::string Class;
  struct
  {
    float R;
    float G;
    float B;
  } Ambient;

};

class MaterialList_t
{
private:
// Example for find
//  bool isEqual(const std::pair<std::string, int>& element)
//  {
//      return element.first ==  User.name;
//  }
//  it = std::find_if( sortList.begin(), sortList.end(), isEqual );
public:
  int Count;
  std::vector<std::pair<int, Material_t *>> Material;
};

class Object3D
{
private:
  typedef enum {
    peOk = 0, peUnknownState, peUnknownNode, peInvalidChar, peInvalidLevel, peInvalidNode
  } ParseError_t;

  typedef enum {
    psWaitKeyStart, psGetKeyName, psValidateKey, ps_Num, ps_Str, ps_SubClass, ps_Float3, ps_Float2, ps_Float, ps_SubClassId
  }ParseState_t;

  typedef enum {
     knExportVer, knComment,
     knScene, knSceneFileName, knSceneFirstFrame, knSceneLastFrame, knSceneFrameSpeed, knSceneTicksPerFrame,
     knSceneBkgStatic, knSceneAmbientStatic,
     knMaterialList, knMaterialCount, knMaterial, knMaterialName, knMaterialClass, knMaterialAmbient
  } KeyNodes_t;

  typedef enum {
    plRoot, plScene, plMaterialList, plMaterial
  }ParseLevel_t;

  typedef struct {
    KeyNodes_t Node;
    ParseState_t NewState;
  } DictStates_t;

  const std::map <std::string, DictStates_t> NodesDict = {
      {"3DSMAX_ASCIIEXPORT", {knExportVer, ps_Num}},
      {"COMMENT", {knComment, ps_Str}},
      {"SCENE", {knScene, ps_SubClass}},
      {"SCENE_FILENAME", {knSceneFileName, ps_Str}},
      {"SCENE_FIRSTFRAME", {knSceneFirstFrame, ps_Num}},
      {"SCENE_LASTFRAME", {knSceneLastFrame, ps_Num}},
      {"SCENE_FRAMESPEED", {knSceneFrameSpeed, ps_Num}},
      {"SCENE_TICKSPERFRAME", {knSceneTicksPerFrame, ps_Num}},
      {"SCENE_BACKGROUND_STATIC", {knSceneBkgStatic, ps_Float3}},
      {"SCENE_AMBIENT_STATIC", {knSceneAmbientStatic, ps_Float3}},
      {"MATERIAL_LIST", {knMaterialList, ps_SubClass}},
      {"MATERIAL_COUNT", {knMaterialCount, ps_Num}},
      {"MATERIAL", {knMaterial, ps_SubClassId}},
      {"MATERIAL_NAME", {knMaterialName, ps_Str}},
      {"MATERIAL_CLASS", {knMaterialClass, ps_Str}},
      {"MATERIAL_AMBIENT", {knMaterialAmbient, ps_Float3}}
  };

  std::list <ParseLevel_t> Level;
  std::string NodeName;
  std::string Value;
  ParseState_t PState;
  KeyNodes_t Node;

  ParseError_t ProcessLine(char * str);
  ParseError_t OnSetNum(int value, int index, KeyNodes_t node, ParseLevel_t level);
  ParseError_t OnSetStr(const std::string value, int index, KeyNodes_t node, ParseLevel_t level);
  ParseError_t OnSetFloat(float value, int index, KeyNodes_t node, ParseLevel_t level);
  ParseError_t OnSetSubClassID(int value, KeyNodes_t node, ParseLevel_t level);


public:
  Object3D(const std::string & filename);
  virtual ~Object3D();

  Scene_t * Scene;
  MaterialList_t * MaterialList;

};

#endif /* OBJECT3D_H_ */
