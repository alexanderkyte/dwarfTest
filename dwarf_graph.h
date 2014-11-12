#ifndef DWARF_GRAPH
#define DWARF_GRAPH

#include <stdbool.h>

typedef struct HeapArray {
  void* contents;
  int capacity;
  int count;
} *Array;

void arrayAppend(Array array, void* item);
Array newHeapArray(int capacity);

typedef Dwarf_Off TypeKey;

#define DEFAULT_SCOPE_CONTENTS_SIZE 5
#define DEFAULT_SCOPE_CHILDREN_SIZE 2
typedef struct Scope {
  Array contents; // Contains variables
  Array children; //
  void* lowPC;
  void* highPC;
} Scope;

typedef struct Function {
  Scope* topScope;
#ifdef DEBUG
  char* dieName;
#endif
} Function;

typedef struct {
  Dwarf_Locdesc** location;
  int expression_count;
  TypeKey type;
} RootInfo;

int dwarf_read_root(Dwarf_Debug dbg, Dwarf_Die* child_die, RootInfo** info, Dwarf_Error* err);

int findFunction(void* PC); // find function with binary search of array

typedef enum {
  POINTER_TYPE,
  STRUCTURE_TYPE,
  UNION_TYPE,
  ARRAY_TYPE,
} TypeCategory;

// handle typedefs in variable traversal
  /* TYPEDEF_TYPE, */
// Both non-pointer types?
/* DW_TAG_base_type */
/* DW_TAG_const_type */

// Remove pointers like typedefs?

typedef struct {
  int layersOfIndirection;
  TypeKey targetType;
} PointerInfo;

#define DEFAULT_STRUCT_MEMBER_LIST_SIZE 10

typedef struct StructMember {
  int offset;
  TypeKey type;
} StructMember;

typedef struct {
  // These two are parallel arrays
  Array members;
} StructInfo;

typedef struct {
  TypeKey* alternatives;
} UnionInfo;

typedef struct {
  TypeKey elementTypes;
  int count;
} ArrayInfo;

typedef struct Type {
  TypeCategory category;
  TypeKey key;
#ifdef DEBUG
  char* dieName;
#endif
  union {
    PointerInfo* pointerInfo;
    StructInfo* structInfo;
    UnionInfo* unionInfo;
    ArrayInfo* pointerArrayInfo;
  } info;
} Type;

int dwarf_read_function(Dwarf_Debug dbg, Dwarf_Die* fn_die, Function** fun, Dwarf_Error* err);
int dwarf_read_scope(Dwarf_Debug dbg, Dwarf_Die* top_die, Scope** top_scope, Dwarf_Error* err);
int dwarf_read_struct(Dwarf_Debug dbg, Dwarf_Die* type_die, StructInfo** structInfo, Dwarf_Error* err);
int dwarf_read_union(Dwarf_Debug dbg, Dwarf_Die* type_die, UnionInfo** unionInfo, Dwarf_Error* err);
int dwarf_read_pointer(Dwarf_Debug dbg, Dwarf_Die* type_die, PointerInfo** pointerInfo, Dwarf_Error* err);
int dwarf_read_array(Dwarf_Debug dbg, Dwarf_Die* type_die, ArrayInfo** arrayInfo, Dwarf_Error* err);
int dwarf_read_member_offset(Dwarf_Debug dbg, Dwarf_Die* member_die, int* offset, Dwarf_Error* err);

#define INITIAL_TYPE_LIST_SIZE 20
#define INITIAL_FUNCTION_LIST_SIZE 20

typedef struct {
  Array types;
  Array functions;
} GCContext;

int finalizeTypes(GCContext* context);

typedef struct {
  unw_cursor_t cursor;
  int function_key;
  Dwarf_Addr pc;
  Dwarf_Addr sp;
} LiveFunction;

typedef struct {
  int count;
  int capacity;
  LiveFunction *stack;
} CallStack;

typedef struct {
  Array roots; // Fiiled with RootInfo pointers.
} Roots;

#endif