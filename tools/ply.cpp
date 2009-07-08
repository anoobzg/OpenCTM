#include <iostream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <rply.h>
#include "ply.h"

using namespace std;

typedef struct {
  Mesh * mMesh;
  long mFaceIdx;
  long mVertexIdx;
  long mNormalIdx;
  long mTexCoordIdx;
  long mColorIdx;
} PLYReaderState;


static int PLYFaceCallback(p_ply_argument argument)
{
  PLYReaderState * state;
  long dummy, length, valueIndex;
  ply_get_argument_user_data(argument, (void **) &state, &dummy);
  double value = ply_get_argument_value(argument);
  ply_get_argument_property(argument, NULL, &length, &valueIndex);
  if((valueIndex >= 0) && (valueIndex <= 2))
    state->mMesh->mIndices[state->mFaceIdx * 3 + valueIndex] = int(value);
  if(valueIndex == 2)
    ++ state->mFaceIdx;
  return 1;
}

static int PLYVertexCallback(p_ply_argument argument)
{
  PLYReaderState * state;
  long index;
  ply_get_argument_user_data(argument, (void **) &state, &index);
  double value = ply_get_argument_value(argument);
  switch(index)
  {
    case 0:
      state->mMesh->mVertices[state->mVertexIdx].x = float(value);
      break;
    case 1:
      state->mMesh->mVertices[state->mVertexIdx].y = float(value);
      break;
    case 2:
      state->mMesh->mVertices[state->mVertexIdx].z = float(value);
      ++ state->mVertexIdx;
      break;
  }
  return 1;
}

static int PLYNormalCallback(p_ply_argument argument)
{
  PLYReaderState * state;
  long index;
  ply_get_argument_user_data(argument, (void **) &state, &index);
  double value = ply_get_argument_value(argument);
  switch(index)
  {
    case 0:
      state->mMesh->mNormals[state->mNormalIdx].x = float(value);
      break;
    case 1:
      state->mMesh->mNormals[state->mNormalIdx].y = float(value);
      break;
    case 2:
      state->mMesh->mNormals[state->mNormalIdx].z = float(value);
      ++ state->mNormalIdx;
      break;
  }
  return 1;
}

static int PLYTexCoordCallback(p_ply_argument argument)
{
  PLYReaderState * state;
  long index;
  ply_get_argument_user_data(argument, (void **) &state, &index);
  double value = ply_get_argument_value(argument);
  switch(index)
  {
    case 0:
      state->mMesh->mTexCoords[state->mTexCoordIdx].u = float(value);
      break;
    case 1:
      state->mMesh->mTexCoords[state->mTexCoordIdx].v = float(value);
      ++ state->mTexCoordIdx;
      break;
  }
  return 1;
}

static int PLYColorCallback(p_ply_argument argument)
{
  PLYReaderState * state;
  long index;
  ply_get_argument_user_data(argument, (void **) &state, &index);
  double value = ply_get_argument_value(argument);
  switch(index)
  {
    case 0:
      state->mMesh->mColors[state->mColorIdx].x = float(value) / 255.0;
      break;
    case 1:
      state->mMesh->mColors[state->mColorIdx].y = float(value) / 255.0;
      break;
    case 2:
      state->mMesh->mColors[state->mColorIdx].z = float(value) / 255.0;
      ++ state->mColorIdx;
      break;
  }
  return 1;
}

/// Import a PLY file from a file.
void Import_PLY(const char * aFileName, Mesh &aMesh)
{
  PLYReaderState state;

  // Clear the mesh
  aMesh.Clear();

  // Initialize the state
  state.mMesh = &aMesh;
  state.mFaceIdx = 0;
  state.mVertexIdx = 0;
  state.mNormalIdx = 0;
  state.mTexCoordIdx = 0;
  state.mColorIdx = 0;

  // Open the PLY file
  p_ply ply = ply_open(aFileName, NULL);
  if(!ply)
    throw runtime_error("Unable to open PLY file.");
  if(!ply_read_header(ply))
    throw runtime_error("Invalid PLY file.");

  // Set face callback
  long faceCount = ply_set_read_cb(ply, "face", "vertex_indices", PLYFaceCallback, &state, 0);
  if(faceCount == 0)
    faceCount = ply_set_read_cb(ply, "face", "vertex_index", PLYFaceCallback, &state, 0);

  // Set vertex callback
  long vertexCount = ply_set_read_cb(ply, "vertex", "x", PLYVertexCallback, &state, 0);
  ply_set_read_cb(ply, "vertex", "y", PLYVertexCallback, &state, 1);
  ply_set_read_cb(ply, "vertex", "z", PLYVertexCallback, &state, 2);

  // Set normal callback
  long normalCount = ply_set_read_cb(ply, "vertex", "nx", PLYNormalCallback, &state, 0);
  ply_set_read_cb(ply, "vertex", "ny", PLYNormalCallback, &state, 1);
  ply_set_read_cb(ply, "vertex", "nz", PLYNormalCallback, &state, 2);

  // Set tex coord callback
  long texCoordCount = ply_set_read_cb(ply, "vertex", "s", PLYTexCoordCallback, &state, 0);
  ply_set_read_cb(ply, "vertex", "t", PLYTexCoordCallback, &state, 1);

  // Set color callback
  long colorCount = ply_set_read_cb(ply, "vertex", "red", PLYColorCallback, &state, 0);
  ply_set_read_cb(ply, "vertex", "green", PLYColorCallback, &state, 1);
  ply_set_read_cb(ply, "vertex", "blue", PLYColorCallback, &state, 2);

  // Sanity check
  if((faceCount < 1) || (vertexCount < 1))
    throw runtime_error("Empty PLY mesh - invalid file format?");

  // Prepare the mesh
  aMesh.mIndices.resize(faceCount * 3);
  aMesh.mVertices.resize(vertexCount);
  aMesh.mNormals.resize(normalCount);
  aMesh.mTexCoords.resize(texCoordCount);
  aMesh.mColors.resize(colorCount);

  // Read the PLY file
  if(!ply_read(ply))
    throw runtime_error("Unable to load PLY file.");

  // Close the PLY file
  ply_close(ply);
}

/// Export a PLY file to a file.
void Export_PLY(const char * aFileName, Mesh &aMesh)
{
  // Open the output file
  ofstream f(aFileName, ios_base::out | ios_base::binary);
  if(f.fail())
    throw runtime_error("Could not open output file.");

  // Write header
  f << "ply" << endl;
  f << "format ascii 1.0" << endl;
  if(aMesh.mComment.size() > 0)
    f << "comment " << aMesh.mComment << endl;
  f << "element vertex " << aMesh.mVertices.size() << endl;
  f << "property float x" << endl;
  f << "property float y" << endl;
  f << "property float z" << endl;
  if(aMesh.mTexCoords.size() > 0)
  {
    f << "property float s" << endl;
    f << "property float t" << endl;
  }
  if(aMesh.mNormals.size() > 0)
  {
    f << "property float nx" << endl;
    f << "property float ny" << endl;
    f << "property float nz" << endl;
  }
  if(aMesh.mColors.size() > 0)
  {
    f << "property uchar red" << endl;
    f << "property uchar green" << endl;
    f << "property uchar blue" << endl;
  }
  f << "element face " << aMesh.mIndices.size() / 3 << endl;
  f << "property list uchar int vertex_indices" << endl;
  f << "end_header" << endl;

  // Write vertices
  for(unsigned int i = 0; i < aMesh.mVertices.size(); ++ i)
  {
    f << aMesh.mVertices[i].x << " " <<
               aMesh.mVertices[i].y << " " <<
               aMesh.mVertices[i].z;
    if(aMesh.mTexCoords.size() > 0)
      f << " " << aMesh.mTexCoords[i].u << " " <<
                        aMesh.mTexCoords[i].v;
    if(aMesh.mNormals.size() > 0)
      f << " " << aMesh.mNormals[i].x << " " <<
                        aMesh.mNormals[i].y << " " <<
                        aMesh.mNormals[i].z;
    if(aMesh.mColors.size() > 0)
      f << " " << int(floorf(255.0f * aMesh.mColors[i].x + 0.5f)) << " " <<
                        int(floorf(255.0f * aMesh.mColors[i].y + 0.5f)) << " " <<
                        int(floorf(255.0f * aMesh.mColors[i].z + 0.5f));
    f << endl;
  }

  // Write faces
  for(unsigned int i = 0; i < aMesh.mIndices.size() / 3; ++ i)
    f << "3 " << aMesh.mIndices[i * 3] << " " <<
                       aMesh.mIndices[i * 3 + 1] << " " <<
                       aMesh.mIndices[i * 3 + 2] << endl;

  // Close the output file
  f.close();
}
