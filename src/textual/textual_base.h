#ifndef TEXTUAL_BASE_H
# define TEXTUAL_BASE_H


typedef U32 TextType;
enum
{
  TextType_Null          = 0,

  TextType_Unmeaningfull = 1,
  TextType_Title         = 2,
  TextType_Heading1      = 3,
  TextType_Heading2      = 4,
  TextType_Heading3      = 5,
  TextType_Paragraph     = 6,
  TextType_Link          = 7,
  TextType_Structural    = 8,
  TextType_Esthetic      = 9,
  TextType_Visual        = 8,
  TextType_Code          = 10,
  TextType_Date          = 11,
  TextType_Parent_List   = 11,
  TextType_Item_List     = 11,
};

typedef struct TextualFormat TextualFormat;
struct TextualFormat
{
  String8 pre;
  String8 post;
};

typedef struct Textual Textual;
struct Textual
{
  Textual      *next_sibbling;
  String8       text;
  TextType      type;
  TextualFormat join;
};

typedef struct TextualNode TextualNode;
struct TextualNode
{
  TextualNode *next;
  Textual     textual;
};

typedef struct TextualList TextualList;
struct TextualList
{
  TextualNode *first;
  TextualNode *last;
  U64         node_count;
};
// TODO: Use it for file html parsing
// TODO: it will not belong here
// TODO: better naming
typedef U32 FileFormatType;
enum
{
  FileFormatType_Null  = (1 << 0),
  FileFormatType_HTML  = (1 << 1),
  FileFormatType_TXT   = (1 << 2),
};

typedef struct TextualTable TextualTable;
struct TextualTable
{
  Textual *textuals;
  U64       count;
};

internal TextualTable*
textual_get_table(Arena *arena, Textual *raw_table, U64 size)
{
    TextualTable *table = push_array(arena, TextualTable, 1);
    table->count = size;
    table->textuals = raw_table;
    // NOTE: Check the table
    B32 has_title = 0;
    for(U64 idx = 0; idx < size; ++idx)
    {
      if(raw_table[idx].type == TextType_Title)
      {
        if(has_title)
        {
          // TODO: handle the error correctly (currently still compute)
          printf("ERROR: table has more than one title type\n");
          break;
        }
      }
    }
    return table;
}
#endif