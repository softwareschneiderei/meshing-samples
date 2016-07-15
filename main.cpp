#include <cmath>
#include <SDL.h>
#include <SDL_opengl.h>
#include <gl/GLU.h>
#include <vector>
#include <tuple>
#include <map>
#include <array>

struct v3
{
  v3(float x, float y, float z)
  {
    data[0]=x;
    data[1]=y;
    data[2]=z;
  }

  v3(float v=0.f) : v3(v,v,v)
  {
  }

  operator const float*() const
  {
    return data;
  }

  v3& operator+=(v3 const& rhs)
  {
    for (int i=0; i<3; ++i)
      data[i]+=rhs[i];
    return *this;
  }

  v3& operator*=(float rhs)
  {
    for (int i=0; i<3; ++i)
      data[i]*=rhs;
    return *this;
  }

  float squared() const
  {
    float result=0.f;
    for (int i=0; i<3; ++i)
      result+=data[i]*data[i];
    return result;
  }

  float data[3];
};

v3 operator+(v3 lhs, v3 const& rhs)
{
  return lhs+=rhs;
}

v3 operator*(v3 lhs, float rhs)
{
  return lhs*=rhs;
}

v3 operator/(v3 lhs, float rhs)
{
  return lhs*=(1.f/rhs);
}

v3 normalize(v3 rhs)
{
  return rhs/std::sqrt(rhs.squared());
}

using Index=std::uint32_t;

struct Triangle
{
  Index vertex[3];
};

using TriangleList=std::vector<Triangle>;
using VertexList=std::vector<v3>;

namespace icosahedron
{
const float X=.525731112119133606f;
const float Z=.850650808352039932f;
const float N=0.f;

static const VertexList vertices=
{
  {-X,N,Z}, {X,N,Z}, {-X,N,-Z}, {X,N,-Z},
  {N,Z,X}, {N,Z,-X}, {N,-Z,X}, {N,-Z,-X},
  {Z,X,N}, {-Z,X, N}, {Z,-X,N}, {-Z,-X, N}
};

static const TriangleList triangles=
{
  {0,4,1},{0,9,4},{9,5,4},{4,5,8},{4,8,1},
  {8,10,1},{8,3,10},{5,3,8},{5,2,3},{2,7,3},
  {7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
  {6,1,10},{9,0,11},{9,11,2},{9,2,5},{7,2,11}
};
}

using Lookup=std::map<std::pair<Index, Index>, Index>;

Index vertex_for_edge(Lookup& lookup,
  VertexList& vertices, Index first, Index second)
{
  Lookup::key_type key(first, second);
  if (key.first>key.second)
    std::swap(key.first, key.second);

  auto inserted=lookup.insert({key, vertices.size()});
  if (inserted.second)
  {
    auto& edge0=vertices[first];
    auto& edge1=vertices[second];
    auto point=normalize(edge0+edge1);
    vertices.push_back(point);
  }

  return inserted.first->second;
}

TriangleList subdivide(VertexList& vertices,
  TriangleList triangles)
{
  Lookup lookup;
  TriangleList result;

  for (auto&& each:triangles)
  {
    std::array<Index, 3> mid;
    for (int edge=0; edge<3; ++edge)
    {
      mid[edge]=vertex_for_edge(lookup, vertices,
        each.vertex[edge], each.vertex[(edge+1)%3]);
    }

    result.push_back({each.vertex[0], mid[0], mid[2]});
    result.push_back({each.vertex[1], mid[1], mid[0]});
    result.push_back({each.vertex[2], mid[2], mid[1]});
    result.push_back({mid[0], mid[1], mid[2]});
  }

  return result;
}

using IndexedMesh=std::pair<VertexList, TriangleList>;

IndexedMesh make_icosphere(int subdivisions)
{
  VertexList vertices=icosahedron::vertices;
  TriangleList triangles=icosahedron::triangles;

  for (int i=0; i<subdivisions; ++i)
  {
    triangles=subdivide(vertices, triangles);
  }

  return{vertices, triangles};
}

void InitGL()
{
  glShadeModel(GL_SMOOTH);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glCullFace(GL_BACK);
  //glEnable(GL_CULL_FACE);
}

int SetupViewport(int width, int height)
{
  if (height==0) {
    height=1;
  }

  auto ratio=(GLfloat)width/(GLfloat)height;
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, ratio, 0.1f, 100.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  return 1;
}

void RenderMesh(VertexList const& vertices, TriangleList const& triangles)
{
  glBegin(GL_TRIANGLES);

  for (auto&& triangle : triangles)
  {
    for (int c=0; c<3; ++c)
    {
      glVertex3fv(vertices[triangle.vertex[c]]);
    }
  }

  glEnd();
}

void Render(float angle, VertexList const& vertices, TriangleList const& triangles)
{
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  glTranslatef(0.f, 0.0f, -6.0f);
  glRotatef(angle, 0.f, 1.f, 0.f);
  glColor3f(0.f, 0.f, 0.f);

  RenderMesh(vertices, triangles);
}

int CALLBACK WinMain(
  _In_ HINSTANCE hInstance,
  _In_ HINSTANCE hPrevInstance,
  _In_ LPSTR     lpCmdLine,
  _In_ int       nCmdShow
  )
{
  // Initialize SDL's Video subsystem
  if (SDL_Init(SDL_INIT_VIDEO)<0)
  {
    return -1;
  }

  // Create our window centered at 512x512 resolution

  auto window=SDL_CreateWindow(
    "My Window",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    800,
    600,
    SDL_WINDOW_OPENGL
    );

  auto context=SDL_GL_CreateContext(window);

  InitGL();

  SetupViewport(800, 600);

  VertexList vertices;
  TriangleList triangles;

  std::tie(vertices, triangles)=make_icosphere(2);

  bool quit=false;
  float angle=0.f;
  while (!quit) 
  {
    SDL_Event e;
    while (SDL_PollEvent(&e)!=0)
    {
      if (e.type==SDL_QUIT) { quit=true; }
    }

    Render(angle, vertices, triangles);
    SDL_GL_SwapWindow(window);
    angle+=0.1f;

  }
  SDL_Quit();

  return 0;
}
