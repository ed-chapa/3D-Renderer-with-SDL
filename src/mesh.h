#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

///////////////////////////////////////////////
// Constants for the cube vertices and faces //
///////////////////////////////////////////////
#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6 * 2)

/////////////////////////////////////////////////////
// These are used for the cube, might delete later //
/////////////////////////////////////////////////////
extern vec3_t cube_vertices[N_CUBE_VERTICES];
extern face_t cube_faces[N_CUBE_FACES];

////////////////////////////////////
// Struct for dynamic size meshes //
////////////////////////////////////
typedef struct {
    vec3_t* vertices;   // dynamic array of vertices
    face_t* faces;      // dynamic array of faces
    vec3_t rotation;    // rotation of the mesh with x, y and z values
    vec3_t scale;       // scale of x, y, and z components
    vec3_t translation; // translation of x, y, and z components
} mesh_t;

extern mesh_t mesh; // this stores mesh data

/////////////////////////////////////////////////////
// Functions for loading data into the mesh object //
/////////////////////////////////////////////////////
void load_cube_mesh_data(void);
void load_obj_file_data(char* filename);

#endif