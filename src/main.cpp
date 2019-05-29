//============================================================================
// Name        : ASEloader.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "LoaderASE.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "SDLGL.h"
#include <SDL2/SDL_image.h>
#include <math.h>

#define TEST_RENDERER             1

#define OLD                       0

typedef struct
{
  GLdouble X;
  GLdouble Y;
  GLdouble Z;
} Vector3d;

Vector3d MP;

LoaderASE_t * Test2;
LoaderASE_t * Test3;
SDL_Window *mainWindow;
SDL_GLContext mainContext;

SDL_GL * mainwin;
GLuint tx1[3];

float a = 0;
float b = 0;
int c = 0;

Vector3f lp1, lp2, lp3, U, V, N;
Vector3f fp1, fp2, fp3;
float nn;

int CalcNormal = 1;
int FlyLight = 1;

GLfloat light_position[] =
{ -50.0, 150.0, 30.0, 0.0 };

GLfloat camera_position[] =
{ 50, 80, 100 };

GLuint LoadTexture(const char * name)
{
  SDL_Surface *TextureImage[1];

  if (!(TextureImage[0] = IMG_Load(name)))
  {
    std::string err = "Error loading texture \"";
    err += name;
    err += "\", ";
    err += SDL_GetError();
    throw err;
  }

//  image.flipVertically();

  GLuint texture = 0;
//  glGenTextures(1, &texture);
//  glBindTexture(GL_TEXTURE_2D, texture);
////  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, image.getSize().x, image.getSize().y, GL_RGBA, GL_UNSIGNED_BYTE, /*image.getPixelsPtr()*/);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glGenTextures(1, &texture);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glBindTexture(GL_TEXTURE_2D, texture);
//  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureImage[0]->w, TextureImage[0]->h, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->pixels);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureImage[0]->w, TextureImage[0]->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureImage[0]->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);

//  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, TextureImage[0]->w, TextureImage[0]->h, GL_RGBA, GL_UNSIGNED_BYTE, TextureImage[0]->pixels);
//  glGetError();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  SDL_FreeSurface(TextureImage[0]);

  return texture;
}

void GetMousePos3D(int x, int y, Vector3d * M3d)
{
  GLint viewport[4];
  GLdouble modelview[16];
  GLdouble projection[16];

  GLfloat winX, winY, winZ;
  GLdouble posX, posY, posZ;

  glGetDoublev( GL_MODELVIEW_MATRIX, modelview);
  glGetDoublev( GL_PROJECTION_MATRIX, projection);
  glGetIntegerv( GL_VIEWPORT, viewport);

  winX = (float) x;
  winY = (float) viewport[3] - (float) y;
  glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

  gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

  M3d->X = posX;
  M3d->Y = posY;
  M3d->Z = posZ;
}

void MousePos(int x, int y)
{
  GetMousePos3D(x, y, &MP);
}

void userwin(void)
{
  /* GUI */
  if (nk_begin(mainwin->ctx, "Runtime info", nk_rect(10, 10, 180, 480), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
  {
    enum
    {
      EASY, HARD
    };
    static int op = EASY;
    static int property = 20;

//    nk_layout_row_static(mainwin->ctx, 30, 80, 1);
//    if (nk_button_label(mainwin->ctx, "button"))
//      fprintf(stdout, "button pressed\n");
//    nk_layout_row_dynamic(mainwin->ctx, 30, 2);
//    if (nk_option_label(mainwin->ctx, "easy", op == EASY))
//      op = EASY;
//    if (nk_option_label(mainwin->ctx, "hard", op == HARD))
//      op = HARD;
//    nk_layout_row_dynamic(mainwin->ctx, 25, 1);
//    nk_property_int(mainwin->ctx, "Compression:", 0, &property, 100, 10, 1);
    nk_layout_row_dynamic(mainwin->ctx, 20, 1);
    nk_checkbox_label(mainwin->ctx, "Calc normales", &CalcNormal);

    nk_layout_row_dynamic(mainwin->ctx, 20, 1);
    nk_label(mainwin->ctx, "Light:", NK_TEXT_LEFT);

    nk_layout_row_dynamic(mainwin->ctx, 20, 1);
    nk_checkbox_label(mainwin->ctx, "FLy light", &FlyLight);

    nk_layout_row_dynamic(mainwin->ctx, 20, 1);
    nk_property_int(mainwin->ctx, "Angle:", 0, &c, 360, 10, 1);
//    nk_layout_row_dynamic(mainwin->ctx, 20, 1);
//    nk_property_float(mainwin->ctx, "Y:", -500, &light_position[1], 500, 10, 1);
//    nk_layout_row_dynamic(mainwin->ctx, 20, 1);
//    nk_property_float(mainwin->ctx, "Z:", -500, &light_position[2], 500, 10, 1);

    nk_layout_row_dynamic(mainwin->ctx, 20, 1);
    nk_label(mainwin->ctx, "Camera:", NK_TEXT_LEFT);

    nk_layout_row_dynamic(mainwin->ctx, 20, 1);
    nk_property_float(mainwin->ctx, "X:", -500, &camera_position[0], 500, 10, 1);
    nk_layout_row_dynamic(mainwin->ctx, 20, 1);
    nk_property_float(mainwin->ctx, "Y:", -500, &camera_position[1], 500, 10, 1);
    nk_layout_row_dynamic(mainwin->ctx, 20, 1);
    nk_property_float(mainwin->ctx, "Z:", -500, &camera_position[2], 500, 10, 1);

    nk_layout_row_dynamic(mainwin->ctx, 20, 1);
    nk_label(mainwin->ctx, "Mouse:", NK_TEXT_LEFT);

    nk_layout_row_dynamic(mainwin->ctx, 20, 1);
    nk_property_double(mainwin->ctx, "X:", -500, &MP.X, 500, 10, 1);
    nk_layout_row_dynamic(mainwin->ctx, 20, 1);
    nk_property_double(mainwin->ctx, "Y:", -500, &MP.Y, 500, 10, 1);
    nk_layout_row_dynamic(mainwin->ctx, 20, 1);
    nk_property_double(mainwin->ctx, "Z:", -500, &MP.Z, 500, 10, 1);

//    nk_layout_row_dynamic(mainwin->ctx, 25, 1);
//    if (nk_combo_begin_color(mainwin->ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx), 400)))
//    {
//      nk_layout_row_dynamic(mainwin->ctx, 120, 1);
//      bg = nk_color_picker(mainwin->ctx, bg, NK_RGBA);
//      nk_layout_row_dynamic(mainwin->ctx, 25, 1);
//      bg.r = nk_propertyf(mainwin->ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
//      bg.g = nk_propertyf(mainwin->ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
//      bg.b = nk_propertyf(mainwin->ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
//      bg.a = nk_propertyf(mainwin->ctx, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
//      nk_combo_end(ctx);
//    }
  }
  nk_end(mainwin->ctx);

}

void DrawModel(LoaderASE_t * obj, int textureid)
{
  Mesh_t * mesh;

  for (unsigned int gm = 0; gm < obj->Geomobject.size(); gm++)
  {
    glBindTexture(GL_TEXTURE_2D, tx1[gm + textureid]);
    glBegin(GL_TRIANGLES);
    mesh = obj->Geomobject[gm]->Mesh;
    for (unsigned int i = 0; i < mesh->MeshFaceList->MeshFace.size(); i++)
    {
      lp1 = mesh->MeshVertexList->MeshVertex[mesh->MeshFaceList->MeshFace[i].A];
      lp2 = mesh->MeshVertexList->MeshVertex[mesh->MeshFaceList->MeshFace[i].B];
      lp3 = mesh->MeshVertexList->MeshVertex[mesh->MeshFaceList->MeshFace[i].C];

      if (CalcNormal)
      {
        U.X = lp2.X - lp1.X;
        U.Y = lp2.Y - lp1.Y;
        U.Z = lp2.Z - lp1.Z;
        V.X = lp3.X - lp1.X;
        V.Y = lp3.Y - lp1.Y;
        V.Z = lp3.Z - lp1.Z;

        N.X = U.Y * V.Z - U.Z * V.Y;
        N.Y = U.Z * V.X - U.X * V.Z;
        N.Z = U.X * V.Y - U.Y * V.X;
        nn = 0;
        if (fabs(N.X) > nn)
          nn = fabs(N.X);
        if (fabs(N.Y) > nn)
          nn = fabs(N.Y);
        if (fabs(N.Z) > nn)
          nn = fabs(N.Z);
        if (nn)
        {
          N.X /= nn;
          N.Y /= nn;
          N.Z /= nn;
        }
        N.X = -N.X;
        N.Y = -N.Y;
        N.Z = -N.Z;
      }
      else
      {
        N.X = mesh->MeshNormals->MeshFaceNormal[i].X;
        N.Y = mesh->MeshNormals->MeshFaceNormal[i].Z;
        N.Z = mesh->MeshNormals->MeshFaceNormal[i].Y;
      }

      fp1 = mesh->MeshTVertList->MeshTVert[mesh->MeshTFaceList->MeshTFace[i].A];
      fp2 = mesh->MeshTVertList->MeshTVert[mesh->MeshTFaceList->MeshTFace[i].B];
      fp3 = mesh->MeshTVertList->MeshTVert[mesh->MeshTFaceList->MeshTFace[i].C];

      glTexCoord2f(fp1.X, 1.0 - fp1.Y);
      glNormal3f(N.X, N.Y, N.Z);
      glVertex3f(lp1.X, lp1.Y, lp1.Z);
      glTexCoord2f(fp2.X, 1.0 - fp2.Y);
      glVertex3f(lp2.X, lp2.Y, lp2.Z);
      glTexCoord2f(fp3.X, 1.0 - fp3.Y);
      glVertex3f(lp3.X, lp3.Y, lp3.Z);

    }
    glEnd();
  }

}

void main2(void);

#if defined(_WIN32)
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char **argv)
#endif
{
#if TEST_RENDERER
  main2();

#endif

#if OLD
  SetOpenGLAttributes();
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  return 0;

  mainWindow = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 450, SDL_WINDOW_OPENGL /*| SDL_WINDOW_FULLSCREEN*/);

  if (!mainWindow)
  return false;

  mainContext = SDL_GL_CreateContext(mainWindow);

  glAlphaFunc(GL_GREATER, 0.0);
  glEnable(GL_ALPHA_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#else
  mainwin = new SDL_GL();
  mainwin->Init(800, 600);

#endif

  SDL_GL_SetSwapInterval(1);
  IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_WEBP);

  glewInit();

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glClearDepth(1.f);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.f, 16.0 / 9.0/*1.f*/, 1.f, 5000.f);
  glEnable(GL_TEXTURE_2D);

//  Test2 = new LoaderASE_t("models/minivan.ASE");
//  Test2 = new LoaderASE_t("models/tank2.ASE");
//  tx1 = LoadTexture("models/tank2.png");
//  Test2 = new LoaderASE_t("models/Hex.ASE");
//  tx1 = LoadTexture("models/Hex.png");
  Test2 = new LoaderASE_t();
  Test2->LoadFile("models/brn2.ASE");
  Test3 = new LoaderASE_t();
  Test3->LoadFile("models/palm.ASE");
  tx1[0] = LoadTexture("models/brn.png");
  tx1[1] = LoadTexture("models/teapot.png");
  tx1[2] = LoadTexture("models/palm.png");

  glEnable( GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);

  GLfloat mat_specular[] =
  { 1.0, 1.0, 1.0, 1.0 };
  GLfloat mat_shininess[] =
  { 50.0 };
  GLfloat light_position[] =
  { -500.0, 150.0, 30.0, 0.0 };

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
//  glEnable( GL_POLYGON_SMOOTH );
//  glEnable( GL_MULTISAMPLE );

  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
//  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  GLfloat light_ambient[] =
  { 0.0, 0.0, 0.0, 1.0 };
  GLfloat light_diffuse[] =
  { 1.0, 1.0, 1.0, 1.0 };
  GLfloat light_specular[] =
  { 1.0, 1.0, 1.0, 1.0 };
//  GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 2.0);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.5);

  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);

#if OLD
  while (1)
#else
  mainwin->Run();
}
void draw2(void)
#endif
{
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  a = a + 0.01;
  b = b + 0.006;

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(camera_position[0], camera_position[1], camera_position[2], 0, 0, 0, 0, 1, 0);
//    gluLookAt(100.0 * sin(a), 60 * sin(b), 100.0 * cos(a), 0, 0, 0, 0, 1, 0);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glEnable(GL_LIGHTING);

  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0, /*(i % 10) / 10.0*/1.0, 1.0);

  glPushMatrix();

  DrawModel(Test2, 0);

  glTranslatef(0.0, 0.0, 150.0);

  DrawModel(Test3, 2);

  glPopMatrix();

  glDisable(GL_TEXTURE_2D);

  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINES);

  glVertex3f(light_position[0], light_position[1], light_position[2]);
  glVertex3f(0, 0, 0);

  glEnd();

//    light_position[0] = light_position[0] + 1;
  light_position[0] = 0;
  light_position[1] = 100 * sin(c * M_PI / 180);
  light_position[2] = 100 * cos(c * M_PI / 180);

  if (FlyLight)
    c = (c + 1) % 360;

  glDisable(GL_LIGHTING);

#if OLD
//    glFlush();
  SDL_GL_SwapWindow(mainwin->win);
  SDL_PumpEvents();
#endif
}
#if OLD
return 0;
}
#endif

#if TEST_RENDERER
#include "Renderer.h"
#include "Palm.h"
#include "Sand.h"
#include "Box01.h"

#define MAX_PALMS                       1

Renderer_t * Renderer;
Palm_t * TestPalm[100];
Camera_t * Cam1;
Camera_t * Cam2;
Sand_t * Sand;
Box01_t *Box[5];

float Distance(V3f P1, V3f P2)
{
  float r;
  r = sqrtf((P2.X - P1.X) * (P2.X - P1.X) + (P2.Y - P1.Y) * (P2.Y - P1.Y) + (P2.Z - P1.Z) * (P2.Z - P1.Z));
  return r;
}

void main2(void)
{
  Renderer = new Renderer_t();

  Renderer->Init("Test engine", 800, 600, false);

  Renderer->InitPhysics();


  SDL_GL_SetSwapInterval(1);

  Renderer->LoadObject("Palm", "models/palm.ASE");
  Renderer->LoadObject("Sand", "models/sand.ASE");
  Renderer->LoadObject("Camera", "models/Camera.ASE");
  Renderer->LoadObject("Box01", "models/box01.ASE");

  for (size_t t = 0; t < MAX_PALMS; t++)
  {
    TestPalm[t] = new Palm_t(Renderer);
    TestPalm[t]->Pos.X = (rand() % 150 - 75) * 5;
    TestPalm[t]->Pos.Z = (rand() % 150 - 75) * 5;
    Renderer->AddModel("Palm", TestPalm[t]);
  }

  Sand = new Sand_t(Renderer);
  Renderer->AddModel("Sand", Sand);

  Box[0] = new Box01_t(Renderer);
  Renderer->AddModel("Box01", Box[0]);
  Box[1] = new Box01_t(Renderer);
  Renderer->AddModel("Box01", Box[1]);
  Box[2] = new Box01_t(Renderer);
  Renderer->AddModel("Box01", Box[2]);

  Box[3] = new Box01_t(Renderer);
  Renderer->AddModel("Box01", Box[3]);

  Cam1 = new Camera_t(Renderer);
  Renderer->AddCamera(Cam1);
  Cam2 = new Camera_t(Renderer);
  Renderer->AddCamera(Cam2);

  unsigned int i;
  float a, b;
  float dx, dy, dz;
  i = 0;
  a = 45;
  b = 90;
//  Cam1->SetVisible(true);
//  Cam2->SetVisible(false);

// ***** Test for physics *************************************************************




  // Constant physics time step
  const float timeStep = 1.0 / 60.0;

  long double previousFrameTime = (SDL_GetTicks() - 16) / 1000.0f;

  // Get the current system time
  long double currentFrameTime = SDL_GetTicks() / 1000.0f;

  // Compute the time difference between the two frames
  long double mDeltaTime  = currentFrameTime - previousFrameTime;
  long double accumulator = 0;

  // Update the previous time
  previousFrameTime = currentFrameTime;

  // Add the time difference in the accumulator
  accumulator += mDeltaTime;

  // While there is enough accumulated time to take
  // one or several physics steps
  while (accumulator >= timeStep) {

      // Update the Dynamics world with a constant time step
    Renderer->GetWorld()->update(timeStep);

      // Decrease the accumulated time
      accumulator -= timeStep;
  }

  // Compute the time interpolation factor
  rp3d::decimal factor = accumulator / timeStep;



// ***** Test for physics *************************************************************

#define MAX_ACC   1.87f

  while (1)
  {
    Renderer->GetWorld()->update(rp3d::decimal(0.016));

    Cam2->Pos.X = 250.0f * sinf(a * M_PI / 180.0f);
    Cam2->Pos.Z = 250.0f * cosf(a * M_PI / 180.0f);
    Cam2->Pos.Y = 75 + 70 * sinf(b * M_PI / 180.0f);

//    Cam1->ViewTo.X = Cam2->Pos.X;
//    Cam1->ViewTo.Y = Cam2->Pos.Y;
//    Cam1->ViewTo.Z = Cam2->Pos.Z;

//    Cam1->Pos.X = Cam2->ViewTo.X + (Cam2->Pos.X - Cam2->ViewTo.X) / 2.0f;
//    Cam1->Pos.Y = Cam2->ViewTo.Y + (Cam2->Pos.Y - Cam2->ViewTo.Y) / 2.0f;
//    Cam1->Pos.Z = Cam2->ViewTo.Z + (Cam2->Pos.Z - Cam2->ViewTo.Z) / 2.0f;

//    dx = (Cam1->Pos.X - Cam1->ViewTo.X) / 20.0f;
//    dy = (Cam1->Pos.Y - Cam1->ViewTo.Y) / 20.0f;
//    dz = (Cam1->Pos.Z - Cam1->ViewTo.Z) / 20.0f;
//    if (dx > MAX_ACC) dx = MAX_ACC;
//    if (dx < -MAX_ACC) dx = -MAX_ACC;
//    if (dy > MAX_ACC) dy = MAX_ACC;
//    if (dy < -MAX_ACC) dy = -MAX_ACC;
//    if (dz > MAX_ACC) dz = MAX_ACC;
//    if (dz < -MAX_ACC) dz = -MAX_ACC;

//    Cam1->Pos.X -= dx;
//    Cam1->Pos.Y -= dy;
//    Cam1->Pos.Z -= dz;

//    a = a + 0.5;
//    b = b + 0.8;

    if (i >= MAX_PALMS)
      i = 0;
    TestPalm[i]->Pos.X += ((rand() % 50) - 25) / 25.0f;

    i++;

    Renderer->DrawScene();
    Renderer->UpdateScreen();
    SDL_PumpEvents();
  }
}

#endif
