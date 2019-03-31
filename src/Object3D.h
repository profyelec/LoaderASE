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

class Object3D
{
private:
  typedef enum {
    peOk = 0, peUnknownState, peUnknownKey, peInvalidChar, peInvalidLevel, peInvalidNode
  } ParseError_t;

  ParseError_t ProcessLine(char * str);

  typedef enum {
    psWaitKeyStart, psGetKeyName, psValidateKey, ps_Num, ps_Str,  ps_SubClass, ps_Float3, ps_Float2, ps_Float
  }ParseState_t;

  typedef enum {
     knExportVer, knComment,
     knScene, knSceneFileName, knSceneFirstFrame, knSceneLastFrame, knSceneFrameSpeed, knSceneTicksPerFrame,
     knSceneBkgStatic, knSceneAmbientStatic,
     knMaterialList
  } KeyNodes_t;

  typedef enum {
    plRoot, plScene, plMaterialList
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
      {"MATERIAL_LIST", {knMaterialList, ps_SubClass}}
  };

  std::list <ParseLevel_t> Level;
  std::string KeyName;
  std::string Value;
  ParseState_t PState;
  KeyNodes_t Node;


public:
  Object3D(char * filename);
  virtual ~Object3D();

  Scene_t * Scene;

};

#endif /* OBJECT3D_H_ */
