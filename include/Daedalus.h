#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#ifndef __DAEDALUS_H__
#define __DAEDALUS_H__

#define MAX_LINE_LENGTH        1024
#define MAX_FILENAME_LENGTH    256
#define MAX_NAME_LENGTH        32
#define MAX_DESCRIPTION_LENGTH 256
#define MAX_INPUT_LENGTH       16

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define STRCPY( dest, src ) strncpy( dest, src, sizeof( dest ) ); dest[sizeof(dest) - 1] = '\0'
#define STRNCPY(dest, src, n) strncpy(dest, src, n); dest[n - 1] = '\0'

#define RANDF(lower, upper) (((float)rand() / (float)(RAND_MAX)) * (upper - lower)) + lower

#define MAP( value, start0, start1, end0, end1 ) ( ( value - start0 ) * ( ( end1 - end0 ) / ( start1 - start0 ) ) + end0 )

#define PI 3.14159265

typedef struct _dVec2_t
{
  float x, y;
} dVec2_t;

typedef struct _dVec3_t
{
  float x, y, z;
} dVec3_t;

typedef struct _dVec4_t
{
  float x;
  float y;
  float z;
  float w;
} dVec4_t;

typedef struct _dMat4x4_t
{
  float m[16];
} dMat4x4_t;

typedef struct _dKinematicBody
{
  dVec3_t position;
  dVec3_t velocity;
  dVec3_t acceleration;
  dVec3_t force;
  float mass;
} dKinematicBody_t;

typedef struct
{
  dVec3_t points[3];
} dTriangle_t;

typedef struct
{
  dTriangle_t *triangles;
  int numberOfTriangles;
} dMesh_t;

typedef struct _dLinkedList_t
{
  void *data;
  char buffer[MAX_FILENAME_LENGTH];
  struct _dLinkedList_t *next;
} dLinkedList_t;

typedef struct _dQuadTree_t
{
  float rect[4];
  int capacity;
  dLinkedList_t *objects;
} dQuadTree_t;

typedef struct
{
  size_t capacity;
  size_t count;
  size_t element_size;
  void* data;
} dArray_t;

typedef struct {    // dString_t
    char* str;      // The actual string buffer
    size_t alloced; // Total allocated memory
    size_t len;     // Current string length
} dString_t;

/* Vector Math Float */
float d_Sqrtf( float number ); //Quake fast inverse square root

float d_LengthOfVec2f( const dVec2_t vec ); //Length of a vector 2f
float d_DistanceVec2f( const dVec2_t a, const dVec2_t b ); //Distance between two vector 2fs
float d_DotProductVec2f( const dVec2_t a, const dVec2_t b ); //Dot product between two vector 2fs
float d_CrossProductVec2f( const dVec2_t a, const dVec2_t b ); //Cross product of two vector 2fs
void  d_AddTwoVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b ); //Add two vector 2fs
void  d_SubTwoVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b ); //Subtract two vector 2fs
void  d_GetVec2fFromAnglef( dVec2_t *output, float angle );
void  d_ScaleMultiplyVec2f( dVec2_t *output, const dVec2_t vec, const float value ); //Multiply a vector 2f by a value
void  d_ScaleDivideVec2f( dVec2_t *output, const dVec2_t vec, const float value ); //Divide a vector 2f by a value
void  d_LimitVec2f( dVec2_t *output, const dVec2_t a, const float value ); //Limit a vector 2f within a range
void  d_NormalizeVec2f( dVec2_t *output, const dVec2_t vec ); //Normalize a vector 2f
void  d_CreateNormalVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b ); //Create a normal vector from two vector 2fs
void  d_FindIntersectionVec2f( dVec2_t *output, const dVec2_t lineA0, const dVec2_t lineA1, const dVec2_t lineB0, const dVec2_t lineB1 ); //Find the intersection between two vector 2fs

float d_LengthOfVec3f( const dVec3_t vec ); //Length of a vector 3f
float d_DistanceVec3f( const dVec3_t a, const dVec3_t b ); //Distance between two vector 3fs
float d_DotProductVec3f( const dVec3_t a, const dVec3_t b ); //Dot product between two vector 3fs
void  d_CrossProductVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b ); //Cross product of two vector 3fs
void  d_AddTwoVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b ); //Add two vector 3fs
void  d_SubTwoVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b ); //Subtract two vector 3fs
void  d_ScaleMultiplyVec3f( dVec3_t *output, const dVec3_t vec, const float value ); //Multiply a vector 3f by a value
void  d_ScaleDivideVec3f( dVec3_t *output, const dVec3_t vec, const float value ); //Divide a vector 3f by a value
void  d_LimitVec3f( dVec3_t *output, const dVec3_t a, const float value ); //Limit a vector 3f within a range
void  d_NormalizeVec3f( dVec3_t *output, const dVec3_t vec ); //Normalize a vector 3f
void  d_CreateNormalVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b, const dVec3_t c ); //Create a normal vector from two vector 3fs
void  d_NormalizeVec4f( dVec4_t *output, const dVec4_t vec );

/* Matrix Operations */
void d_MatrixClearf( dMat4x4_t *matrix ); //Clear a 4x4 matrix to an identity matrix
void d_MatrixRotateXf( dMat4x4_t *matrix, const float angle_rad ); //Rotate matrix by angle in radians about the x axis
void d_MatrixRotateYf( dMat4x4_t *matrix, const float angle_rad ); //Rotate matrix by angle in radians about the y axis
void d_MatrixRotateZf( dMat4x4_t *matrix, const float angle_rad ); //Rotate matrix by angle in radians about the z axis
void d_MatrixCreateProjectionf( dMat4x4_t *matrix, const float aspect_ratio, const float fov, const float near, const float far ); //Create a projection matrix
void d_MatrixMultiplyf( dMat4x4_t *output, const dMat4x4_t a, const dMat4x4_t b ); //Multiply two 4x4 Matrices together
void d_MatrixTranslateVec3f( dMat4x4_t *matrix, const dVec3_t vec ); //Translate a matrix by a 3D point
void d_MatrixTranslateVec4f( dMat4x4_t *matrix, const dVec4_t vec ); //Translate a matrix by a 4D point
void d_MatrixMultiplyVec3f( dVec3_t *output, dMat4x4_t matrix, const dVec3_t vec ); //Transform a 3D point into a 4x4 matrix

void d_MatrixInverseTransformVec3f( dVec3_t *output, const dMat4x4_t matrix, dVec3_t vec ); //Inverse transform 3D point into a 4x4 matrix
void d_MatrixMultiplyVec4f( dVec4_t *output, const dMat4x4_t matrix, const dVec4_t vec4 ); //Transform a 4D point into a 4x4 matrix

/* Matrix Creation */
void d_MatrixXYf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
void d_MatrixXZf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
void d_MatrixYXf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
void d_MatrixYZf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
void d_MatrixZXf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
void d_MatrixZYf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );

/* Linked List */
dLinkedList_t *d_CreateLinkedList( void *data, char *name, size_t size );
void *d_GetDataInLinkedListByIndex( dLinkedList_t *head, int index );
void *d_GetDataInLinkedListByName( dLinkedList_t *head, char *name );
void d_RemoveNodeInLinkedListByIndex( dLinkedList_t *head, int index );
void d_RemoveNodeInLinkedListByName( dLinkedList_t *head, char *name );
void d_ClearLinkedList( dLinkedList_t *head );
void d_PushBack( dLinkedList_t *head, void *data, char *name, size_t size );
void d_PushFront( dLinkedList_t **head, void *data, char *name, size_t size );
void *d_PopBack( dLinkedList_t *head );
void *d_PopFront( dLinkedList_t **head );
void d_PrintLinkedList( dLinkedList_t *head );
int  d_GetLengthOfLinkedList( dLinkedList_t *head );

/* Quad Tree */
dQuadTree_t *d_CreateQuadtree( float *rect, int capacity );
void d_InsertObjectInQuadtree( dQuadTree_t *tree, void *object );
void d_SubdivideQuadtree( dQuadTree_t *tree );

/* Kinematic Body 2D  */
void d_CreateKinmaticBody( dKinematicBody_t *output, const dVec2_t position, const dVec2_t velocity, const dVec2_t acceleration, const float mass );

/* Strings */
char* d_CreateStringFromFile(const char *filename);
dString_t *d_InitString(void);
void d_DestroyString(dString_t* sb);
void d_AppendString(dString_t* sb, const char* str, size_t len);
void d_AppendChar(dString_t* sb, char c);
void d_AppendInt(dString_t* sb, int val);
void d_ClearString(dString_t* sb);
void d_TruncateString(dString_t* sb, size_t len);
void d_DropString(dString_t* sb, size_t len);
size_t d_GetStringLength(const dString_t* sb);
const char* d_PeekString(const dString_t* sb);
char* d_DumpString(const dString_t* sb, size_t* len);
dString_t* d_InitStringFrom(const char* initial_content);
dString_t* d_InitStringFromString(const dString_t* src);
bool d_CopyString(dString_t* dest, const dString_t* src);


// Additional String Utils
void d_FormatString(dString_t* sb, const char* format, ...);
void d_TemplateString(dString_t* sb, const char* tmplt, const char** keys, const char** values, int count);
void d_CapitalizeString(dString_t* sb);
void d_TitleCaseString(dString_t* sb);
void d_AppendProgressBar(dString_t* sb, int current, int max, int width, char fill_char, char empty_char);
void d_PadCenterString(dString_t* sb, const char* text, int width, char pad_char);
void d_PadLeftString(dString_t* sb, const char* text, int width, char pad_char);
void d_PadRightString(dString_t* sb, const char* text, int width, char pad_char);
void d_RepeatString(dString_t* sb, char character, int count);

// Pythonic String Utils
void d_JoinStrings(dString_t* sb, const char** strings, int count, const char* separator);
char** d_SplitString(const char* text, const char* delimiter, int* count);
void d_FreeSplitString(char** result, int count); // Helper to free the result
void d_SliceString(dString_t* sb, const char* text, int start, int end);

// Pythonic String Utils
void d_StringJoin(dString_t* sb, const char** strings, int count, const char* separator);
char** d_StringSplit(const char* text, const char* delimiter, int* count);
void d_StringFreeSplit(char** result, int count); // Helper to free the result
void d_StringSlice(dString_t* sb, const char* text, int start, int end);

/* Dynamic Arrays */
dArray_t* d_InitArray( size_t capacity, size_t element_size );
void d_AppendArray( dArray_t* array, void* data );
void* d_GetDataFromArrayByIndex( dArray_t* array, size_t count );
void* d_PopDataFromArray( dArray_t* array );
int d_ResizeArray( dArray_t* array, size_t new_capacity );
void d_DestroyArray( dArray_t* array );

#endif
