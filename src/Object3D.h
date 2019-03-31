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

class Object3D
{
private:
  int ProcessLine(char * str);

  typedef enum {
    psWaitKeyStart, psGetKeyName, psValidateKey, psExportVer, psComment, psScene
  }ParseState_t;

  const std::map <std::string, ParseState_t> StateDict = {
      {"3DSMAX_ASCIIEXPORT", psExportVer},
      {"COMMENT", psComment},
      {"SCENE", psScene}
  };

  ParseState_t PState;
  std::string KeyName;

public:
  Object3D(char * filename);
  virtual ~Object3D();
};

#endif /* OBJECT3D_H_ */
