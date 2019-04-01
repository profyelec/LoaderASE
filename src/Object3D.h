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

class Map_t
{
public:
  std::string Name;
  std::string Class;
  int SubNo;
  float Amount;
  std::string Bitmap;
  std::string Type;
  float UVW_U_offset;
  float UVW_V_offset;
  float UVW_U_tiling;
  float UVW_V_tiling;
  float UVW_Angle;
  float UVW_Blur;
  float UVW_Blur_Offset;
  float UVW_Nouse_AMT;
  float UVW_Noise_size;
  float UVW_Noise_level;
  float UVW_Noise_phase;
  std::string BitmapFilter;
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
  struct
  {
    float R;
    float G;
    float B;
  } Diffuse;
  struct
  {
    float R;
    float G;
    float B;
  } Specular;
  float Shine;
  float ShineStrength;
  float Transparency;
  float Wiresize;
  std::string Shading;
  float XPFalloff;
  float SelfIllum;
  std::string Falloff;
  std::string XPType;
  Map_t * DiffuseMap;
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
    psWaitKeyStart, psGetKeyName, psValidateKey, ps_Num, ps_Str, ps_SubClass, ps_Float3, ps_Float2, ps_Float, ps_SubClassId, ps_Enum
  }ParseState_t;

  typedef enum {
     knExportVer, knComment,
     knScene, knSceneFileName, knSceneFirstFrame, knSceneLastFrame, knSceneFrameSpeed, knSceneTicksPerFrame,
     knSceneBkgStatic, knSceneAmbientStatic,
     knMaterialList, knMaterialCount, knMaterial, knMaterialName, knMaterialClass, knMaterialAmbient,
     knMaterialDiffuse, knMaterialSpecular, knMaterialShine, knMaterialShineStrength, knMaterialTransparency, knMaterialWiresize,
     knMaterialShading, knMaterialXPFalloff, knMaterialSelfIllum, knMaterialFalloff,
     knMaterialXPType,
     knMapDiffuse, knMapName, knMapClass, knMapSubNo, knMapAmount, knMapBitmap, knMapType,
     knMapUVW_UO, knMapUVW_VO, knMapUVW_UT, knMapUVW_VT, knMapUVW_Angle, knMapUVW_Blur, knMapUVW_Blur_Offset,
     knMapUVW_Nouse_AMT, knMapUVW_Noise_size, knMapUVW_Noise_level, knMapUVW_Noise_phase, knMapBitmapFilter
  } KeyNodes_t;

  typedef enum {
    plRoot, plScene, plMaterialList, plMaterial, plMapDiffuse
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
      {"MATERIAL_AMBIENT", {knMaterialAmbient, ps_Float3}},
      {"MATERIAL_DIFFUSE", {knMaterialDiffuse, ps_Float3}},
      {"MATERIAL_SPECULAR", {knMaterialSpecular, ps_Float3}},
      {"MATERIAL_SHINE", {knMaterialShine, ps_Float}},
      {"MATERIAL_SHINESTRENGTH", {knMaterialShineStrength, ps_Float}},
      {"MATERIAL_TRANSPARENCY", {knMaterialTransparency, ps_Float}},
      {"MATERIAL_WIRESIZE", {knMaterialWiresize, ps_Float}},
      {"MATERIAL_SHADING", {knMaterialShading, ps_Enum}},
      {"MATERIAL_XP_FALLOFF", {knMaterialXPFalloff, ps_Float}},
      {"MATERIAL_SELFILLUM", {knMaterialSelfIllum, ps_Float}},
      {"MATERIAL_FALLOFF", {knMaterialFalloff, ps_Enum}},
      {"MATERIAL_XP_TYPE", {knMaterialXPType, ps_Enum}},
      {"MAP_DIFFUSE", {knMapDiffuse, ps_SubClass}},
      {"MAP_NAME", {knMapName, ps_Str}},
      {"MAP_CLASS", {knMapClass, ps_Str}},
      {"MAP_SUBNO", {knMapSubNo, ps_Num}},
      {"MAP_AMOUNT", {knMapAmount, ps_Float}},
      {"BITMAP", {knMapBitmap, ps_Str}},
      {"MAP_TYPE", {knMapType, ps_Enum}},
      {"UVW_U_OFFSET", {knMapUVW_UO, ps_Float}},
      {"UVW_V_OFFSET", {knMapUVW_VO, ps_Float}},
      {"UVW_U_TILING", {knMapUVW_UT, ps_Float}},
      {"UVW_V_TILING", {knMapUVW_VT, ps_Float}},
      {"UVW_ANGLE", {knMapUVW_Angle, ps_Float}},
      {"UVW_BLUR", {knMapUVW_Blur, ps_Float}},
      {"UVW_BLUR_OFFSET", {knMapUVW_Blur_Offset,ps_Float}},
      {"UVW_NOUSE_AMT", {knMapUVW_Nouse_AMT, ps_Float}},
      {"UVW_NOISE_SIZE", {knMapUVW_Noise_size, ps_Float}},
      {"UVW_NOISE_LEVEL", {knMapUVW_Noise_level, ps_Float}},
      {"UVW_NOISE_PHASE", {knMapUVW_Noise_phase, ps_Float}},
      {"BITMAP_FILTER", {knMapBitmapFilter, ps_Enum}}

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
  ParseError_t OnSetEnum(const std::string value, int index, KeyNodes_t node, ParseLevel_t level);


public:
  Object3D(const std::string & filename);
  virtual ~Object3D();

  Scene_t * Scene;
  MaterialList_t * MaterialList;

};

#endif /* OBJECT3D_H_ */
