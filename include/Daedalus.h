#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
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

// =============================================================================
// LOGGING SYSTEM TYPES AND STRUCTURES
// =============================================================================

// Log levels enumeration
typedef enum {
    D_LOG_LEVEL_DEBUG = 0,
    D_LOG_LEVEL_INFO = 1,
    D_LOG_LEVEL_WARNING = 2,
    D_LOG_LEVEL_ERROR = 3,
    D_LOG_LEVEL_FATAL = 4,
    D_LOG_LEVEL_OFF = 5
} dLogLevel_t;

// Log entry structure - represents a single log message
typedef struct {
    dString_t* message;         // The log message content
    dLogLevel_t level;          // Severity level
    const char* file;           // Source file path
    int line;                   // Line number
    const char* function;       // Function name
    double timestamp;           // Unix timestamp
    const char* context;        // Optional context/module name
    uint32_t thread_id;         // Thread that generated the log
} dLogEntry_t;

// Log handler function pointer - processes log entries
typedef void (*dLogHandler_t)(const dLogEntry_t* entry, void* user_data);

// Handler registration structure
typedef struct {
    dLogHandler_t handler;      // Handler function
    void* user_data;            // User data passed to handler
    dLogLevel_t min_level;      // Minimum level this handler processes
} dLogHandlerReg_t;

// Logger configuration
typedef struct {
    dLogLevel_t default_level;      // Default log level
    bool include_timestamp;         // Include timestamp in output
    bool include_file_info;         // Include file/line info
    bool include_function;          // Include function name
    bool include_thread_id;         // Include thread ID
    bool colorize_output;           // Use ANSI colors for console
    const char* timestamp_format;   // strftime format string
    const char* context_separator;  // Separator for contexts (default: "::")
} dLogConfig_t;

// Filter rule types
typedef enum {
    D_LOG_FILTER_DIRECTORY,     // Match directory path
    D_LOG_FILTER_PREFIX,        // Match filename prefix
    D_LOG_FILTER_SUFFIX,        // Match filename suffix
    D_LOG_FILTER_EXACT,         // Match exact filename
    D_LOG_FILTER_GLOB,          // Match glob pattern
    D_LOG_FILTER_REGEX          // Match regex pattern
} dLogFilterType_t;

// Individual filter rule
typedef struct {
    dLogFilterType_t type;          // Type of filter
    const char* pattern;            // Pattern to match
    size_t pattern_len;             // Pre-computed pattern length
    dLogLevel_t level;              // Log level for matches
    uint32_t priority;              // Higher priority overrides lower
    uint32_t pattern_hash;          // Pre-computed hash for fast matching
    bool recursive;                 // For directory filters
} dLogFilterRule_t;

// Fast filter engine for zero-overhead logging
typedef struct {
    // Bloom filter for fast negative matches
    uint64_t bloom_filter[256];

    // Sorted array of rules by priority
    dArray_t* rules;                // Array of dLogFilterRule_t

    // LRU cache for recent path lookups
    struct {
        uint32_t path_hash;
        dLogLevel_t level;
        uint64_t last_access;
    } cache[1024];

    // Cache statistics
    uint32_t cache_hits;
    uint32_t cache_misses;
} dLogFilterEngine_t;

// Global configuration for compile-time optimization
#define D_MAX_SOURCE_FILES 4096
#define D_LOG_BLOOM_SIZE 256
#define D_LOG_CACHE_SIZE 1024

typedef struct {
    // Pre-computed log levels indexed by file ID (O(1) lookup)
    dLogLevel_t file_log_levels[D_MAX_SOURCE_FILES];

    // Filter engine for dynamic rules
    dLogFilterEngine_t* filter_engine;

    // Default level for unknown files
    dLogLevel_t default_level;

    // Global enable flag
    bool logging_enabled;

    // Statistics
    uint64_t total_logs_processed;
    uint64_t total_logs_suppressed;
} dLogGlobalConfig_t;

// Log statistics for monitoring
typedef struct {
    uint64_t logs_by_level[D_LOG_LEVEL_OFF];   // Count per level
    uint64_t logs_suppressed;                    // Total suppressed
    uint64_t logs_rate_limited;                  // Rate limited count
    double total_log_time;                       // Time spent logging
    uint32_t handler_errors;                     // Handler failures
} dLogStats_t;

// Main logger structure
typedef struct {
    dLogConfig_t config;            // Logger configuration
    dArray_t* handlers;             // Array of dLogHandlerReg_t
    dArray_t* contexts;             // Stack of context strings
    dString_t* format_buffer;       // Thread-local format buffer
    dLogFilterEngine_t* filters;    // Filter rules engine
    void* mutex;                    // Optional mutex for thread safety
    dLogStats_t* stats;             // Per-logger statistics
    bool is_global;                 // Is this the global logger
} dLogger_t;

// Log builder for zero-allocation complex messages
typedef struct {
    dString_t* buffer;              // Message buffer
    dLogLevel_t level;              // Log level
    dLogger_t* logger;              // Target logger
    const char* file;               // Source file
    int line;                       // Line number
    const char* function;           // Function name
    bool committed;                 // Prevent double commits
} dLogBuilder_t;

// Structured log builder for key-value logging
typedef struct {
    dLogBuilder_t base;             // Inherits from log builder
    dArray_t* fields;               // Array of key-value pairs
    bool in_json_mode;              // Output as JSON
} dLogStructured_t;

// Log context for hierarchical logging
typedef struct dLogContext_t {
    const char* name;               // Context name
    dLogger_t* logger;              // Associated logger
    uint64_t start_time;            // For timing contexts
    struct dLogContext_t* parent;   // Parent context for nesting
} dLogContext_t;

// Rate limiter for preventing log spam
typedef struct {
    uint32_t message_hash;          // Hash of the message
    uint32_t count;                 // Times logged
    uint32_t max_count;             // Maximum allowed
    double time_window;             // Time window in seconds
    double first_log_time;          // First log timestamp
    double last_log_time;           // Last log timestamp
} dLogRateLimit_t;

// Log filter configuration builder
typedef struct {
    dArray_t* rules;                // Building list of rules
    uint32_t next_priority;         // Auto-incrementing priority
} dLogFilterBuilder_t;



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
/*
 * Create a new string builder
 *
 * `dString_t*` - Pointer to new string builder, or NULL on allocation failure
 *
 * -- Must be destroyed with d_DestroyString() to free memory
 * -- Initial capacity is 32 bytes but will grow automatically
 * -- MEMORY OWNERSHIP: Caller owns returned pointer and must free it
 * -- ALLOCATION STRATEGY: Uses calloc() for structure, malloc() for buffer
 * -- INITIALIZATION STATE: Buffer is null-terminated empty string
 * -- FAILURE MODES: Returns NULL if either allocation fails
 * -- CLEANUP ON FAILURE: Automatically frees partial allocations
 * -- MINIMUM CAPACITY: Always starts with d_string_builder_min_size bytes
 * -- RELATED FUNCTIONS: Must pair with d_DestroyString() to prevent leaks
 * -- POSTCONDITIONS: On success, returns valid string builder with len=0, alloced>=32
 */
dString_t *d_InitString(void);
/*
 * Destroy a string builder and free its memory
 *
 * `sb` - Pointer to string builder to destroy
 *
 * -- After calling this function, the pointer is invalid and should not be used
 * -- Calling with NULL is safe and does nothing
 * -- MEMORY CLEANUP: Frees both internal buffer and structure itself
 * -- DOUBLE-FREE PROTECTION: Safe to call multiple times with same pointer
 * -- DANGLING POINTER RISK: Caller must set pointer to NULL after calling
 * -- RELATED FUNCTIONS: Paired with d_InitString() for complete lifecycle
 * -- THREAD SAFETY: Not thread-safe if multiple threads access same builder
 * -- DEBUG CONSIDERATION: Consider zeroing memory in debug builds
 * -- PRECONDITIONS: sb must be NULL or valid pointer from d_InitString()
 */
void d_DestroyString(dString_t* sb);
/*
 * Add a string to the string builder
 *
 * `sb` - Pointer to string builder
 * `str` - String to append (must be null-terminated if len is 0)
 * `len` - Length of string to append, or 0 to use strlen()
 *
 * -- If len is 0, strlen() is called to determine the length
 * -- If len > 0, exactly len characters are copied (partial strings allowed)
 * -- Does nothing if sb or str is NULL, or if str is empty
 * -- SELF-APPEND SAFETY: Handles case where str points within sb->str buffer
 * -- BUFFER MANAGEMENT: Automatically grows buffer using d_StringBuilderEnsureSpace()
 * -- NULL BYTE HANDLING: When len > 0, can append strings containing null bytes
 * -- PERFORMANCE: O(len) for copying, O(len) for strlen() when len=0
 * -- MEMORY OVERLAP: Uses memmove() for safe overlapping memory operations
 * -- EDGE CASES: Empty strings with len=0 are detected and skipped
 * -- POINTER INVALIDATION: May invalidate existing pointers to sb->str
 * -- THREAD SAFETY: Not thread-safe, caller must synchronize access
 * -- PRECONDITIONS: sb must be valid, str must be valid or NULL
 * -- POSTCONDITIONS: sb->len increased by actual characters appended, always null-terminated
 */
void d_AppendString(dString_t* sb, const char* str, size_t len);
/*
 * Set the content of an existing dString_t to a new value
 *
 * @param string: Pointer to existing dString_t structure
 * @param content: C-string content to copy into the dString_t
 * @param flags: Optional flags for string handling behavior
 * @return: Success/failure indicator
 * -- Frees existing string content if any
 * -- Allocates new memory for content and copies data
 * -- Handles null pointers gracefully
 * -- Updates internal string length and capacity
 */
int d_SetString(dString_t* string, const char* content, int flags);
/*
 * Add a limited portion of a string to the string builder
 *
 * `sb` - Pointer to string builder
 * `str` - String to append (must be null-terminated if max_len exceeds actual string)
 * `max_len` - Maximum number of characters to append
 *
 * -- Appends at most max_len characters from str
 * -- If str is shorter than max_len, appends the entire string
 * -- Safe with NULL pointers (does nothing)
 * -- Does not require str to be null-terminated if max_len <= actual length
 * -- Useful for controlled truncation and substring operations
 * -- Handles self-append scenarios safely (appending string to itself)
 * -- Uses same memory management as d_AppendString for consistency
 * -- Performance: O(min(max_len, strlen(str))) for length calculation
 * -- Thread safety: Not thread-safe, caller must synchronize access
 */
void d_AppendStringN(dString_t* sb, const char* str, size_t max_len);
/*
 * Add a single character to the string builder
 *
 * `sb` - Pointer to string builder
 * `c` - Character to append
 *
 * -- Does nothing if sb is NULL
 * -- Can append any character including '\0' (though this may confuse string functions)
 */
void d_AppendChar(dString_t* sb, char c);
/*
 * Add an integer to the string builder as a decimal string
 *
 * `sb` - Pointer to string builder
 * `val` - Integer value to append
 *
 * -- Does nothing if sb is NULL
 * -- Uses snprintf internally for safe conversion
 * -- Supports full 32-bit integer range including negative values
 */
void d_AppendInt(dString_t* sb, int val);
/*
 * Append a floating-point number to the string builder
 *
 * `sb` - Pointer to string builder
 * `val` - Float value to append
 * `decimals` - Number of decimal places to show (0-10)
 *
 * -- Does nothing if sb is NULL
 * -- If decimals is negative, uses 6 decimal places (default)
 * -- If decimals > 10, caps at 10 decimal places
 * -- Uses standard printf formatting for floating-point representation
 */
void d_AppendFloat(dString_t* sb, float val, int decimals);
/*
 * Clear the string builder content
 *
 * `sb` - Pointer to string builder
 *
 * -- Does nothing if sb is NULL
 * -- Memory is not freed, only the length is reset to 0
 * -- Buffer capacity remains unchanged for efficient reuse
 */
void d_ClearString(dString_t* sb);
/*
 * Truncate the string builder to a specific length
 *
 * `sb` - Pointer to string builder
 * `len` - New length (must be <= current length)
 *
 * -- Does nothing if sb is NULL or len >= current length
 * -- Memory is not freed, only the length is reduced
 * -- The string remains null-terminated at the new length
 */
void d_TruncateString(dString_t* sb, size_t len);
/*
 * Remove characters from the beginning of the string builder
 *
 * `sb` - Pointer to string builder
 * `len` - Number of characters to remove from the beginning
 *
 * -- Does nothing if sb is NULL or len is 0
 * -- If len >= current length, the string builder is cleared completely
 * -- Uses memmove for safe overlapping memory operation
 */
void d_DropString(dString_t* sb, size_t len);
/*
 * Get the current length of the string builder content
 *
 * `sb` - Pointer to string builder
 *
 * `size_t` - Current length in characters, or 0 if sb is NULL
 *
 * -- Return value does not include the null terminator
 * -- Safe to call with NULL pointer (returns 0)
 */
size_t d_GetStringLength(const dString_t* sb);
/*
 * Get a read-only pointer to the string builder's content
 *
 * `sb` - Pointer to string builder
 *
 * `const char*` - Pointer to internal string, or NULL if sb is NULL
 *
 * -- Do not modify the returned string or free the pointer
 * -- The pointer becomes invalid after any modification to the string builder
 * -- The returned string is always null-terminated
 * -- Safe to call with NULL pointer (returns NULL)
 */
const char* d_PeekString(const dString_t* sb);
/*
 * Create a copy of the string builder's content
 *
 * `sb` - Pointer to string builder
 * `len` - Optional pointer to receive the length of the returned string
 *
 * `char*` - Newly allocated copy of the string, or NULL on error
 *
 * -- The caller is responsible for freeing the returned pointer
 * -- If len is not NULL, it receives the length of the string (excluding null terminator)
 * -- Returns NULL if sb is NULL or memory allocation fails
 * -- The returned string is always null-terminated
 */
char *d_DumpString(const dString_t* sb, size_t* len);
/*
 * Add formatted text to the string builder using printf-style formatting
 *
 * `sb` - Pointer to string builder
 * `format` - Printf-style format string
 * `...` - Variable arguments corresponding to format specifiers
 *
 * -- Does nothing if sb or format is NULL
 * -- Uses vsnprintf internally for safe formatting
 * -- Supports all standard printf format specifiers (%d, %s, %f, etc.)
 * -- Automatically calculates required space and grows buffer as needed
 * -- Appends formatted text to existing content (does not replace)
 */
void d_FormatString(dString_t* sb, const char* format, ...);
/*
 * Add text with template substitution to the string builder
 *
 * `sb` - Pointer to string builder
 * `tmplt` - Template string with placeholders in {key} format
 * `keys` - Array of key strings to match against placeholders
 * `values` - Array of replacement values corresponding to keys
 * `count` - Number of key-value pairs
 *
 * -- Does nothing if sb or tmplt is NULL
 * -- Placeholders must be in format {keyname} with no spaces
 * -- Keys are matched exactly (case-sensitive)
 * -- If a placeholder has no matching key, it is left unchanged
 * -- Keys longer than 255 characters are treated as literal text
 * -- Supports nested braces by treating unmatched { as literal characters
 */
void d_TemplateString(dString_t* sb, const char* tmplt, const char** keys, const char** values, int count);
/*
 * Add an ASCII progress bar to the string builder
 *
 * `sb` - Pointer to string builder
 * `current` - Current progress value
 * `max` - Maximum progress value
 * `width` - Width of the progress bar (excluding brackets)
 * `fill_char` - Character to use for filled portions
 * `empty_char` - Character to use for empty portions
 *
 * -- Does nothing if sb is NULL, width <= 0, or max <= 0
 * -- Progress bar format: [████████----] where █ is fill_char and - is empty_char
 * -- If current > max, the bar is filled completely
 * -- If current < 0, the bar is empty
 * -- Total visual width is width + 2 (for brackets)
 */
void d_AppendProgressBar(dString_t* sb, int current, int max, int width, char fill_char, char empty_char);
/*
  * Add text padded to the center with specified character to reach target width
  *
  * `sb` - Pointer to string builder
  * `text` - Text to center (must be null-terminated)
  * `width` - Target total width including padding
  * `pad_char` - Character to use for padding
  *
  * -- Does nothing if sb or text is NULL
  * -- If text length >= width, adds text without padding
  * -- Text is centered with padding distributed evenly on both sides
  * -- If padding cannot be evenly distributed, left side gets one less character
  * -- Example: d_PadCenterString(sb, "Hi", 6, '.') produces "..Hi.."
  * -- Example: d_PadCenterString(sb, "Hi", 7, '.') produces "..Hi..."
  * -- Commonly used for centered headers and titles in ASCII interfaces
  */
void d_PadCenterString(dString_t* sb, const char* text, int width, char pad_char);
/*
  * Add text padded to the left with specified character to reach target width
  *
  * `sb` - Pointer to string builder
  * `text` - Text to pad (must be null-terminated)
  * `width` - Target total width including padding
  * `pad_char` - Character to use for padding
  *
  * -- Does nothing if sb or text is NULL
  * -- If text length >= width, adds text without padding
  * -- Padding is added to the left side of the text
  * -- Example: d_PadLeftString(sb, "Hi", 5, '.') produces "...Hi"
  * -- Commonly used for right-aligned text in tables and menus
  */
void d_PadLeftString(dString_t* sb, const char* text, int width, char pad_char);
/*
  * Add text padded to the right with specified character to reach target width

  * `sb` - Pointer to string builder
  * `text` - Text to pad (must be null-terminated)
  * `width` - Target total width including padding
  * `pad_char` - Character to use for padding
  *
  * -- Does nothing if sb or text is NULL
  * -- If text length >= width, adds text without padding
  * -- Padding is added to the right side of the text
  * -- Example: d_PadRightString(sb, "Hi", 5, '.') produces "Hi..."
  * -- Commonly used for left-aligned text in tables and menus
  */
void d_PadRightString(dString_t* sb, const char* text, int width, char pad_char);
/*
 * Add repeated characters to the string builder
 *
 * `sb` - Pointer to string builder
 * `character` - Character to repeat
 * `count` - Number of times to repeat the character
 *
 * -- Does nothing if sb is NULL or count <= 0
 * -- Efficiently adds multiple copies of the same character
 * -- Used internally by progress bar and padding functions
 */
void d_RepeatString(dString_t* sb, char character, int count);

// Pythonic String Utils
/*
  * Join an array of strings with a separator (like Python's str.join())
  *
  * `sb` - Pointer to string builder
  * `strings` - Array of string pointers to join
  * `count` - Number of strings in the array
  * `separator` - String to insert between each element
  *
  * -- Does nothing if sb is NULL or count <= 0
  * -- If strings array is NULL, does nothing
  * -- NULL strings in the array are treated as empty strings
  * -- If separator is NULL, strings are joined without separation
  * -- Example: d_JoinStrings(sb, {"a", "b", "c"}, 3, ", ") produces "a, b, c"
  * -- Commonly used for creating comma-separated lists, file paths, etc.
  */
void d_JoinStrings(dString_t* sb, const char** strings, int count, const char* separator);
/*
  * Extract a substring using Python-style slice notation (like str[start:end])
  *
  * `sb` - Pointer to string builder
  * `text` - Source string to slice (must be null-terminated)
  * `start` - Starting index (inclusive, negative values count from end)
  * `end` - Ending index (exclusive, negative values count from end, -1 means end of string)
  *
  * -- Does nothing if sb or text is NULL
  * -- Negative indices count from the end: -1 is last character, -2 is second-to-last, etc.
  * -- If start >= end (after resolving negative indices), no text is added
  * -- Indices are clamped to valid range [0, string_length]
  * -- Example: d_SliceString(sb, "Hello", 1, 4) produces "ell"
  * -- Example: d_SliceString(sb, "Hello", -3, -1) produces "llo"
  * -- Example: d_SliceString(sb, "Hello", 0, -1) produces "Hello"
  */
void d_SliceString(dString_t* sb, const char* text, int start, int end);
/* Dynamic Arrays */
/*
 * Create a new dynamic array with specified capacity and element size
 *
 * `capacity` - Initial number of elements the array can hold
 * `element_size` - Size in bytes of each element (e.g., sizeof(int))
 *
 * `dArray_t*` - Pointer to new dynamic array, or NULL on allocation failure
 *
 * -- Must be destroyed with d_DestroyArray() to free memory
 * -- Initial count is 0 even though capacity may be larger
 * -- Elements can be any type as long as element_size is correct
 * -- Capacity of 0 is allowed but array cannot store elements until resized
 */

 /* Logging System */

 // =============================================================================
 // CORE LOGGING FUNCTIONS
 // =============================================================================

 /*
  * Log a message at the specified level
  *
  * `level` - Log level (DEBUG, INFO, WARNING, ERROR, FATAL)
  * `message` - Message string to log
  *
  * -- Does nothing if global logger is not initialized
  * -- Message is filtered based on current log level and file filters
  * -- Automatically adds timestamp and other metadata based on configuration
  */
 void d_Log(dLogLevel_t level, const char* message);

 /*
  * Log a formatted message at the specified level (printf-style)
  *
  * `level` - Log level (DEBUG, INFO, WARNING, ERROR, FATAL)
  * `format` - Printf-style format string
  * `...` - Variable arguments corresponding to format specifiers
  *
  * -- Does nothing if global logger is not initialized or format is NULL
  * -- Uses thread-local buffer to avoid allocations
  * -- Supports all standard printf format specifiers
  */
 void d_LogF(dLogLevel_t level, const char* format, ...);

 /*
  * Log with explicit file/line/function information
  *
  * `level` - Log level
  * `file` - Source file path
  * `line` - Line number
  * `func` - Function name
  * `message` - Message to log
  *
  * -- Internal function typically called through macros
  * -- Provides full source location context
  */
 void d_LogEx(dLogLevel_t level, const char* file, int line, const char* func, const char* message);

 /*
  * Log with explicit file/line/function and formatting
  *
  * `level` - Log level
  * `file` - Source file path
  * `line` - Line number
  * `func` - Function name
  * `format` - Printf-style format string
  * `...` - Variable arguments
  *
  * -- Internal function typically called through macros
  * -- Combines source location with formatted output
  */
 void d_LogExF(dLogLevel_t level, const char* file, int line, const char* func, const char* format, ...);

 // =============================================================================
 // CONVENIENCE LOGGING FUNCTIONS
 // =============================================================================

 /*
  * Log a debug message
  *
  * `message` - Message to log
  *
  * -- Convenience wrapper for d_Log(D_LOG_LEVEL_DEBUG, message)
  * -- Typically compiled out in release builds
  */
 void d_LogDebug(const char* message);

 /*
  * Log an info message
  *
  * `message` - Message to log
  *
  * -- Convenience wrapper for d_Log(D_LOG_LEVEL_INFO, message)
  * -- Standard informational logging
  */
 void d_LogInfo(const char* message);

 /*
  * Log a warning message
  *
  * `message` - Message to log
  *
  * -- Convenience wrapper for d_Log(D_LOG_LEVEL_WARNING, message)
  * -- For potentially problematic situations
  */
 void d_LogWarning(const char* message);

 /*
  * Log an error message
  *
  * `message` - Message to log
  *
  * -- Convenience wrapper for d_Log(D_LOG_LEVEL_ERROR, message)
  * -- For recoverable error conditions
  */
 void d_LogError(const char* message);

 /*
  * Log a fatal message
  *
  * `message` - Message to log
  *
  * -- Convenience wrapper for d_Log(D_LOG_LEVEL_FATAL, message)
  * -- For unrecoverable errors
  * -- May trigger application shutdown depending on configuration
  */
 void d_LogFatal(const char* message);

 // Formatted versions
 void d_LogDebugF(const char* format, ...);
 void d_LogInfoF(const char* format, ...);
 void d_LogWarningF(const char* format, ...);
 void d_LogErrorF(const char* format, ...);
 void d_LogFatalF(const char* format, ...);

 // =============================================================================
 // LOGGER MANAGEMENT
 // =============================================================================

 /*
  * Create a new logger with the specified configuration
  *
  * `config` - Logger configuration settings
  *
  * `dLogger_t*` - Pointer to new logger, or NULL on allocation failure
  *
  * -- Must be destroyed with d_DestroyLogger() to free memory
  * -- Can be set as global logger with d_SetGlobalLogger()
  * -- Multiple loggers can exist simultaneously for different subsystems
  */
 dLogger_t* d_CreateLogger(dLogConfig_t config);

 /*
  * Destroy a logger and free all associated resources
  *
  * `logger` - Pointer to logger to destroy
  *
  * -- Safe to call with NULL pointer
  * -- If this is the global logger, global logging is disabled
  * -- All handlers are removed and their resources freed
  */
 void d_DestroyLogger(dLogger_t* logger);

 /*
  * Set a logger as the global default logger
  *
  * `logger` - Logger to set as global (can be NULL to disable)
  *
  * -- Previous global logger is not destroyed (caller's responsibility)
  * -- NULL disables global logging
  * -- All d_Log* functions use the global logger
  */
 void d_SetGlobalLogger(dLogger_t* logger);

 /*
  * Get the current global logger
  *
  * `dLogger_t*` - Pointer to global logger, or NULL if none set
  *
  * -- Do not destroy the returned logger without unsetting it first
  * -- Can be used to temporarily modify logger configuration
  */
 dLogger_t* d_GetGlobalLogger(void);

 // =============================================================================
 // CONFIGURATION AND FILTERING
 // =============================================================================

 /*
  * Create a new filter configuration builder
  *
  * `dLogFilterBuilder_t*` - New filter builder, or NULL on error
  *
  * -- Must be destroyed with d_DestroyFilterBuilder()
  * -- Use to build complex filtering rules before applying
  */
 dLogFilterBuilder_t* d_CreateFilterBuilder(void);

 /*
  * Add a directory-based filter rule
  *
  * `builder` - Filter builder
  * `path` - Directory path to match (e.g., "src/items/")
  * `level` - Minimum log level for this directory
  *
  * -- Path should use forward slashes on all platforms
  * -- Trailing slash determines if rule is recursive
  * -- Higher priority than previously added rules
  */
 void d_FilterBuilder_AddDirectory(dLogFilterBuilder_t* builder, const char* path, dLogLevel_t level);

 /*
  * Add a file prefix filter rule
  *
  * `builder` - Filter builder
  * `prefix` - Filename prefix to match (e.g., "test_")
  * `level` - Minimum log level for matching files
  *
  * -- Matches beginning of filename only, not full path
  * -- Case-sensitive matching
  */
 void d_FilterBuilder_AddPrefix(dLogFilterBuilder_t* builder, const char* prefix, dLogLevel_t level);

 /*
  * Add a file suffix filter rule
  *
  * `builder` - Filter builder
  * `suffix` - Filename suffix to match (e.g., "Math.c")
  * `level` - Minimum log level for matching files
  *
  * -- Matches end of filename including extension
  * -- Period in suffix is matched literally
  */
 void d_FilterBuilder_AddSuffix(dLogFilterBuilder_t* builder, const char* suffix, dLogLevel_t level);

 /*
  * Apply filter configuration to a logger
  *
  * `logger` - Logger to configure (NULL for global logger)
  * `builder` - Filter builder with rules
  *
  * -- Replaces any existing filter configuration
  * -- Builder can be reused after applying
  * -- Pre-computes hashes and optimizes for fast runtime checks
  */
 void d_FilterBuilder_Apply(dLogger_t* logger, dLogFilterBuilder_t* builder);

 /*
  * Load filter configuration from a string
  *
  * `logger` - Logger to configure (NULL for global logger)
  * `config_str` - Configuration string in DSL format
  *
  * `int` - 0 on success, non-zero on parse error
  *
  * -- Format: "path/pattern: LEVEL" separated by semicolons or newlines
  * -- Example: "src/all: INFO; test_*: OFF; *Math.c: WARNING"
  * -- Comments start with # and continue to end of line
  */
 int d_FilterBuilder_FromString(dLogger_t* logger, const char* config_str);

 /*
  * Destroy a filter builder and free resources
  *
  * `builder` - Filter builder to destroy
  *
  * -- Safe to call with NULL pointer
  * -- Does not affect already-applied configurations
  */
 void d_DestroyFilterBuilder(dLogFilterBuilder_t* builder);

 /*
  * Set the minimum log level for a logger
  *
  * `logger` - Logger to configure (NULL for global logger)
  * `level` - Minimum level to log
  *
  * -- Messages below this level are discarded
  * -- File-specific filters can override to allow lower levels
  */
 void d_SetLogLevel(dLogger_t* logger, dLogLevel_t level);

 /*
  * Get the current log level for a logger
  *
  * `logger` - Logger to get level from (NULL for global logger)
  *
  * Returns: Current minimum log level for the logger
  *
  * -- Returns the effective log level that determines message filtering
  * -- File-specific filters may override this for specific sources
  */
 dLogLevel_t d_GetLogLevel(dLogger_t* logger);

 /*
  * Enable or disable logging globally
  *
  * `enabled` - true to enable, false to disable
  *
  * -- When disabled, all logging compiles to near-zero overhead
  * -- Affects all loggers, not just global logger
  * -- Useful for performance-critical sections
  */
 void d_SetLoggingEnabled(bool enabled);

 /*
  * Check if logging is globally enabled
  *
  * Returns: true if logging is enabled, false if disabled
  *
  * -- Returns the current global logging state
  * -- When false, all logging operations are no-ops
  */
 bool d_IsLoggingEnabled(void);

 // =============================================================================
 // LOG HANDLERS
 // =============================================================================

 /*
  * Add a log handler to a logger
  *
  * `logger` - Logger to add handler to (NULL for global logger)
  * `handler` - Handler function to call for each log entry
  * `user_data` - Arbitrary data passed to handler
  *
  * -- Multiple handlers can be added to a single logger
  * -- Handlers are called in order of registration
  * -- user_data can be NULL if handler doesn't need it
  */
 void d_AddLogHandler(dLogger_t* logger, dLogHandler_t handler, void* user_data);

 /*
  * Remove a log handler from a logger
  *
  * `logger` - Logger to remove handler from (NULL for global logger)
  * `handler` - Handler function to remove
  *
  * -- Removes first matching handler
  * -- Does nothing if handler not found
  */
 void d_RemoveLogHandler(dLogger_t* logger, dLogHandler_t handler);

 /*
  * Built-in console log handler
  *
  * `entry` - Log entry to output
  * `user_data` - Unused (pass NULL)
  *
  * -- Outputs to stdout/stderr based on level
  * -- Supports ANSI color codes if configured
  * -- Thread-safe output
  */
 void d_ConsoleLogHandler(const dLogEntry_t* entry, void* user_data);

 /*
  * Built-in file log handler
  *
  * `entry` - Log entry to output
  * `user_data` - FILE* to write to
  *
  * -- Writes formatted log entries to file
  * -- user_data must be valid FILE* opened for writing
  * -- Caller responsible for opening/closing file
  */
 void d_FileLogHandler(const dLogEntry_t* entry, void* user_data);

 /*
  * Built-in string builder log handler
  *
  * `entry` - Log entry to output
  * `user_data` - dString_t* to append to
  *
  * -- Appends formatted entries to string builder
  * -- Useful for capturing logs in tests
  * -- user_data must be valid dString_t*
  */
 void d_StringLogHandler(const dLogEntry_t* entry, void* user_data);

 // =============================================================================
 // LOG BUILDER PATTERN
 // =============================================================================

 /*
  * Begin building a log message
  *
  * `level` - Log level for the message
  *
  * `dLogBuilder_t*` - Log builder instance, or NULL on error
  *
  * -- Must call d_LogBuilder_End() or d_LogBuilder_Commit() to send
  * -- Reuses thread-local buffers for zero allocation
  * -- Can chain multiple append operations
  */
 dLogBuilder_t* d_LogBegin(dLogLevel_t level);

 /*
  * Append a string to the log builder
  *
  * `builder` - Log builder instance
  * `text` - Text to append
  *
  * `dLogBuilder_t*` - Same builder for chaining
  *
  * -- Returns NULL if builder is NULL
  * -- Can chain: d_LogBegin(INFO)->Append("Hello")->Append(" World")->End()
  */
 dLogBuilder_t* d_LogBuilder_Append(dLogBuilder_t* builder, const char* text);

 /*
  * Append an integer to the log builder
  *
  * `builder` - Log builder instance
  * `value` - Integer value to append
  *
  * `dLogBuilder_t*` - Same builder for chaining
  */
 dLogBuilder_t* d_LogBuilder_AppendInt(dLogBuilder_t* builder, int value);

 /*
  * Append a float to the log builder
  *
  * `builder` - Log builder instance
  * `value` - Float value to append
  * `decimals` - Number of decimal places (0-10)
  *
  * `dLogBuilder_t*` - Same builder for chaining
  */
 dLogBuilder_t* d_LogBuilder_AppendFloat(dLogBuilder_t* builder, float value, int decimals);

 /*
  * Append formatted text to the log builder
  *
  * `builder` - Log builder instance
  * `format` - Printf-style format string
  * `...` - Variable arguments
  *
  * `dLogBuilder_t*` - Same builder for chaining
  */
 dLogBuilder_t* d_LogBuilder_AppendF(dLogBuilder_t* builder, const char* format, ...);

 /*
  * Finish building and send the log message
  *
  * `builder` - Log builder instance
  *
  * -- Sends the built message through normal logging pipeline
  * -- Builder is automatically cleaned up after this call
  * -- Do not use builder after calling End()
  */
 void d_LogBuilder_End(dLogBuilder_t* builder);

 /*
  * Alias for d_LogBuilder_End() for clearer intent
  */
 void d_LogBuilder_Commit(dLogBuilder_t* builder);

 // =============================================================================
 // STRUCTURED LOGGING
 // =============================================================================

 /*
  * Begin building a structured log entry
  *
  * `level` - Log level for the message
  *
  * `dLogStructured_t*` - Structured log builder, or NULL on error
  *
  * -- Must call d_LogStructured_Commit() to send
  * -- Outputs in key=value or JSON format
  * -- Ideal for machine-parseable logs
  */
 dLogStructured_t* d_LogStructured(dLogLevel_t level);

 /*
  * Add a string field to structured log
  *
  * `builder` - Structured log builder
  * `key` - Field name
  * `value` - Field value
  *
  * `dLogStructured_t*` - Same builder for chaining
  */
 dLogStructured_t* d_LogStructured_Field(dLogStructured_t* builder, const char* key, const char* value);

 /*
  * Add an integer field to structured log
  *
  * `builder` - Structured log builder
  * `key` - Field name
  * `value` - Integer value
  *
  * `dLogStructured_t*` - Same builder for chaining
  */
 dLogStructured_t* d_LogStructured_FieldInt(dLogStructured_t* builder, const char* key, int value);

 /*
  * Add a float field to structured log
  *
  * `builder` - Structured log builder
  * `key` - Field name
  * `value` - Float value
  * `decimals` - Decimal places (0-10)
  *
  * `dLogStructured_t*` - Same builder for chaining
  */
 dLogStructured_t* d_LogStructured_FieldFloat(dLogStructured_t* builder, const char* key, float value, int decimals);

 /*
  * Set structured log output format
  *
  * `builder` - Structured log builder
  * `json` - true for JSON format, false for key=value format
  *
  * `dLogStructured_t*` - Same builder for chaining
  */
 dLogStructured_t* d_LogStructured_SetFormat(dLogStructured_t* builder, bool json);

 /*
  * Finish and send the structured log
  *
  * `builder` - Structured log builder
  *
  * -- Formats and sends the structured data
  * -- Builder is cleaned up after this call
  */
 void d_LogStructured_Commit(dLogStructured_t* builder);

 /*
  * Add a boolean field to structured log
  *
  * `builder` - Structured log builder
  * `key` - Field name
  * `value` - Boolean value
  *
  * `dLogStructured_t*` - Same builder for chaining
  */
 dLogStructured_t* d_LogStructured_FieldBool(dLogStructured_t* builder, const char* key, bool value);

 /*
  * Add a timestamp field to structured log
  *
  * `builder` - Structured log builder
  * `key` - Field name
  *
  * `dLogStructured_t*` - Same builder for chaining
  *
  * -- Adds current timestamp in ISO 8601 format
  * -- Timestamp is captured when this function is called
  */
 dLogStructured_t* d_LogStructured_FieldTimestamp(dLogStructured_t* builder, const char* key);

 /*
  * Clone an existing structured log for reuse
  *
  * `source` - Source structured log to clone
  *
  * `dLogStructured_t*` - New structured log with copied fields, or NULL on error
  *
  * -- Creates a new structured log with all fields from source
  * -- Useful for creating logs with shared context
  * -- Source log remains unchanged and usable
  */
 dLogStructured_t* d_LogStructured_Clone(dLogStructured_t* source);

 // =============================================================================
 // LOG CONTEXTS
 // =============================================================================

 /*
  * Push a new logging context onto the stack
  *
  * `name` - Context name (e.g., "Physics", "Renderer")
  *
  * `dLogContext_t*` - Context handle, or NULL on error
  *
  * -- All subsequent logs include this context
  * -- Contexts can be nested
  * -- Must call d_PopLogContext() to remove
  */
 dLogContext_t* d_PushLogContext(const char* name);

 /*
  * Pop a logging context from the stack
  *
  * `context` - Context handle from d_PushLogContext()
  *
  * -- Removes the context from subsequent logs
  * -- Safe to call with NULL
  * -- Contexts must be popped in LIFO order
  */
 void d_PopLogContext(dLogContext_t* context);

 /*
  * Log the execution time of a context when popped
  *
  * `context` - Context to enable timing for
  *
  * -- When context is popped, logs execution time
  * -- Useful for performance profiling
  */
 void d_LogContext_EnableTiming(dLogContext_t* context);

 // =============================================================================
 // CONDITIONAL AND RATE-LIMITED LOGGING
 // =============================================================================
 // Rate limiting behavior flags
 typedef enum {
     // Hashes the final, fully-rendered message. Prone to issues with formatted strings.
     D_LOG_RATE_LIMIT_FLAG_HASH_FINAL_MESSAGE = 0,
     // Hashes the format string itself. Correctly limits repeated formatted calls.
     D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING = 1
 } dLogRateLimitFlag_t;
 /*
  * Log only if condition is true
  *
  * `condition` - Boolean condition to check
  * `level` - Log level if condition is true
  * `message` - Message to log
  *
  * -- Zero overhead when condition is false
  * -- Useful for debug assertions and conditional warnings
  */
 void d_LogIf(bool condition, dLogLevel_t level, const char* message);

 /*
  * Log with printf formatting only if condition is true
  *
  * `condition` - Boolean condition to check
  * `level` - Log level if condition is true
  * `format` - Printf-style format string
  * `...` - Variable arguments
  */
 void d_LogIfF(bool condition, dLogLevel_t level, const char* format, ...);

 /*
  * Log with rate limiting to prevent spam
  *
  * `level` - Log level
  * `max_count` - Maximum logs allowed in time window
  * `time_window` - Time window in seconds
  * `message` - Message to log
  *
  * -- Prevents identical messages from flooding logs
  * -- Uses message hash for comparison
  * -- Resets counter after time window expires
  */
 void d_LogRateLimited(dLogLevel_t level, uint32_t max_count, double time_window, const char* message);

 /*
  * Rate-limited logging with formatting
  *
  * `flag` - Determines what to hash for rate limiting (the format string or the final message)
  * `level` - Log level
  * `max_count` - Maximum logs allowed in time window
  * `time_window` - Time window in seconds
  * `format` - Printf-style format string
  * `...` - Variable arguments
  */
 void d_LogRateLimitedF(dLogRateLimitFlag_t flag, dLogLevel_t level, uint32_t max_count, double time_window, const char* format, ...);


 // =============================================================================
 // UTILITY FUNCTIONS
 // =============================================================================

 /*
  * Get string representation of log level
  *
  * `level` - Log level enum value
  *
  * `const char*` - String like "DEBUG", "INFO", etc.
  *
  * -- Returns "UNKNOWN" for invalid levels
  * -- Useful for custom handlers
  */
 const char* d_LogLevel_ToString(dLogLevel_t level);

 /*
  * Parse log level from string
  *
  * `str` - String like "DEBUG", "INFO", etc.
  *
  * `dLogLevel_t` - Parsed level, or D_LOG_LEVEL_INFO on error
  *
  * -- Case-insensitive parsing
  * -- Accepts full names or single letters (D, I, W, E, F)
  */
 dLogLevel_t d_LogLevel_FromString(const char* str);

 /*
  * Get ANSI color code for log level
  *
  * `level` - Log level
  *
  * `const char*` - ANSI escape sequence, or empty string if colors disabled
  *
  * -- Returns escape codes like "\033[31m" for red
  * -- Used by console handler for colored output
  */
 const char* d_LogLevel_GetColor(dLogLevel_t level);

 /*
  * Get current timestamp as Unix time
  *
  * `double` - Current time in seconds since epoch
  *
  * -- High precision timestamp for log entries
  * -- Platform-independent implementation
  */
 double d_GetTimestamp(void);

 /*
  * Format timestamp into string
  *
  * `buffer` - Output buffer
  * `buffer_size` - Size of buffer
  * `timestamp` - Unix timestamp
  * `format` - strftime format string (NULL for ISO-8601)
  *
  * -- Safe string formatting with bounds checking
  * -- Default format: "YYYY-MM-DD HH:MM:SS.mmm"
  */
 void d_FormatTimestamp(char* buffer, size_t buffer_size, double timestamp, const char* format);

 /*
  * Get logging statistics
  *
  * `logger` - Logger to get stats from (NULL for global)
  * `stats` - Output structure to fill
  *
  * -- Provides counts of logs by level, suppressed logs, etc.
  * -- Useful for monitoring and debugging
  */
 void d_GetLogStats(dLogger_t* logger, dLogStats_t* stats);

 /*
  * Reset logging statistics
  *
  * `logger` - Logger to reset stats for (NULL for global)
  *
  * -- Clears all counters back to zero
  * -- Does not affect configuration
  */
 void d_ResetLogStats(dLogger_t* logger);

 /*
  * Resets the global rate limiter cache, clearing all tracked messages.
  *
  * -- FOR TESTING AND DEBUGGING USE ONLY.
  * -- This function is not thread-safe and should be called when no other
  * threads are performing logging.
  * -- It clears all history, allowing rate-limited messages to be logged again.
  */
 void d_ResetRateLimiterCache(void);

 /*
  * Gets the current number of unique messages being tracked by the rate limiter.
  *
  * `size_t` - The number of entries in the rate limiter's cache.
  *
  * -- FOR TESTING AND DEBUGGING USE ONLY.
  * -- Can be used to verify that the cache is growing as expected.
  */
 size_t d_GetRateLimiterCacheEntryCount(void);
 // =============================================================================
 // ZERO-OVERHEAD MACROS
 // =============================================================================

 // These macros provide zero overhead when logging is disabled at compile time
 // They automatically capture file, line, and function information

 #ifndef D_LOG_CURRENT_FILE_ID
     #define D_LOG_CURRENT_FILE_ID (__COUNTER__)
 #endif

 #ifdef D_LOGGING_ENABLED
     #define D_LOG(level, msg) \
         do { \
             extern dLogGlobalConfig_t g_log_config; \
             if ((level) >= g_log_config.file_log_levels[D_LOG_CURRENT_FILE_ID]) { \
                 d_LogEx(level, __FILE__, __LINE__, __func__, msg); \
             } \
         } while(0)

     #define D_LOGF(level, fmt, ...) \
         do { \
             extern dLogGlobalConfig_t g_log_config; \
             if ((level) >= g_log_config.file_log_levels[D_LOG_CURRENT_FILE_ID]) { \
                 d_LogExF(level, __FILE__, __LINE__, __func__, fmt, __VA_ARGS__); \
             } \
         } while(0)
 #else
     #define D_LOG(level, msg) ((void)0)
     #define D_LOGF(level, fmt, ...) ((void)0)
 #endif

 // Convenience macros for common log levels
 #define D_LOG_DEBUG(msg) D_LOG(D_LOG_LEVEL_DEBUG, msg)
 #define D_LOG_INFO(msg) D_LOG(D_LOG_LEVEL_INFO, msg)
 #define D_LOG_WARNING(msg) D_LOG(D_LOG_LEVEL_WARNING, msg)
 #define D_LOG_ERROR(msg) D_LOG(D_LOG_LEVEL_ERROR, msg)
 #define D_LOG_FATAL(msg) D_LOG(D_LOG_LEVEL_FATAL, msg)

 #define D_LOG_DEBUGF(fmt, ...) D_LOGF(D_LOG_LEVEL_DEBUG, fmt, __VA_ARGS__)
 #define D_LOG_INFOF(fmt, ...) D_LOGF(D_LOG_LEVEL_INFO, fmt, __VA_ARGS__)
 #define D_LOG_WARNINGF(fmt, ...) D_LOGF(D_LOG_LEVEL_WARNING, fmt, __VA_ARGS__)
 #define D_LOG_ERRORF(fmt, ...) D_LOGF(D_LOG_LEVEL_ERROR, fmt, __VA_ARGS__)
 #define D_LOG_FATALF(fmt, ...) D_LOGF(D_LOG_LEVEL_FATAL, fmt, __VA_ARGS__)

 // Builder pattern macro
 #define D_LOG_BUILD(level) d_LogBegin(level)

 // Timed block macro
 #define D_LOG_TIMED_BLOCK(name) \
     for (dLogContext_t* _ctx = d_PushLogContext(name); _ctx; d_PopLogContext(_ctx), _ctx = NULL) \
         for (d_LogContext_EnableTiming(_ctx); _ctx; _ctx = NULL)


dArray_t* d_InitArray( size_t capacity, size_t element_size );
/*
 * Append an element to the end of the dynamic array
 *
 * `array` - Pointer to dynamic array
 * `data` - Pointer to element data to copy into the array
 *
 * -- Does nothing if array or data is NULL
 * -- Copies element_size bytes from data into the array
 * -- Fails silently if array is at full capacity (count == capacity)
 * -- Elements are stored contiguously in memory for cache efficiency
 * -- Use d_ResizeArray() to increase capacity before appending if needed
 */
void d_AppendArray( dArray_t* array, void* data );

/*
 * Get a pointer to an element at the specified index
 *
 * `array` - Pointer to dynamic array
 * `index` - Zero-based index of element to retrieve
 *
 * `void*` - Pointer to element data, or NULL if index is out of bounds
 *
 * -- Returns NULL if array is NULL or index >= count
 * -- Returned pointer is valid until the array is modified or destroyed
 * -- Caller can read/write through the returned pointer safely
 * -- Use appropriate casting: int* ptr = (int*)d_GetDataFromArrayByIndex(array, 0)
 * -- Index must be less than count, not capacity (only counts appended elements)
 */
void* d_GetDataFromArrayByIndex( dArray_t* array, size_t index );
/*
 * Remove and return a pointer to the last element in the array (LIFO - Last In, First Out)
 *
 * `array` - Pointer to dynamic array
 *
 * `void*` - Pointer to the last element's data, or NULL if array is empty
 *
 * -- Returns NULL if array is NULL or empty (count == 0)
 * -- Decrements count but does not free memory (element data remains in buffer)
 * -- Returned pointer becomes invalid after next append or array modification
 * -- Implements stack-like behavior for dynamic arrays
 * -- Memory is not reallocated, only the count is decremented for efficiency
 */
void* d_PopDataFromArray( dArray_t* array );
/*
 * Resize the dynamic array's data buffer to accommodate more or fewer elements
 *
 * `array` - Pointer to dynamic array
 * `new_capacity` - New capacity in bytes (not elements)
 *
 * `int` - 0 on success, 1 on failure
 *
 * -- Returns 1 (error) if array is NULL or memory allocation fails
 * -- new_capacity is in bytes, not element count
 * -- To resize to N elements: d_ResizeArray(array, N * array->element_size)
 * -- Existing data is preserved up to the smaller of old/new sizes
 * -- Count is not automatically adjusted when shrinking (may exceed new capacity)
 * -- Use realloc() internally which may move data to new memory location
 * -- Array capacity is updated to new_capacity on success
 */
int d_ResizeArray( dArray_t* array, size_t new_capacity );
/*
 * Grow the dynamic array by adding additional capacity to current capacity
 *
 * `array` - Pointer to dynamic array
 * `additional_capacity` - Number of bytes to add to current capacity
 *
 * `int` - 0 on success, 1 on failure
 *
 * -- Convenience function that calls d_ResizeArray() internally
 * -- New total capacity = current capacity + additional_capacity
 * -- additional_capacity is in bytes, not element count
 * -- To grow by N elements: d_GrowArray(array, N * array->element_size)
 * -- Useful for incrementally expanding arrays without calculating new total size
 * -- Returns same error codes as d_ResizeArray()
 */
int d_GrowArray( dArray_t* array, size_t additional_capacity );
/*
 * Destroy a dynamic array and free all associated memory
 *
 * `array` - Pointer to dynamic array to destroy
 *
 * -- After calling this function, the pointer is invalid and should not be used
 * -- Frees both the data buffer and the array structure itself
 * -- Safe to call with NULL pointer (does nothing)
 * -- Does not call destructors for complex element types (caller responsibility)
 * -- For arrays of pointers, caller must free pointed-to objects before destroying array
 */
void d_DestroyArray( dArray_t* array );

// Turning Strings Into Dynamic Arrays
// src/dStrings-dArrays.c
/*
 * Split a string by delimiter into a dynamic array of string builders (like Python's str.split())
 *
 * `text` - The null-terminated string to be split.
 * `delimiter` - The null-terminated string to split by.
 *
 * `dArray_t*` - A new dynamic array containing dString_t* pointers, or NULL on error.
 *
 * -- Returns NULL if text or delimiter is NULL, or if memory allocation fails.
 * -- The caller must free the result using d_FreeSplitStringArray() to prevent memory leaks.
 * -- Each element in the array is a `dString_t*` which is also allocated and must be destroyed.
 * -- Empty strings between delimiters are included in the result as empty dString_t objects.
 * -- If the delimiter is not found, returns an array with a single element containing a copy of the original string.
 */
dArray_t* d_SplitString(const char* text, const char* delimiter);

/*
 * Free memory allocated by d_SplitString() for the dynamic array of strings.
 *
 * `string_array` - The dynamic array of dString_t* pointers returned by d_SplitString().
 *
 * -- Safe to call with a NULL pointer.
 * -- Frees each individual dString_t object, then the array data, and finally the array structure itself.
 * -- Must be called for every successful d_SplitString() call to prevent memory leaks.
 */
void d_FreeSplitStringArray(dArray_t* string_array);

#endif
