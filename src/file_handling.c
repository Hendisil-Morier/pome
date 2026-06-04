#include "error.h"
#include "editor_datatype.h"
#include "gap_buffer.h"
#include <stdio.h>

Status load_file(Editor* editor)
{
  if (editor == nullptr)
    return FAILURE;

  const char* filename = editor->filename;
  if (filename == nullptr)
    return FAILURE;

  FILE *file = fopen(filename, "r");
  if (file == nullptr)
  {
    perror("Unable to open file.");
    return FAILURE;
  }

  int c;
  while((c = fgetc(file)) != EOF)
    insert_gap_buffer(editor->buffer, c);

  if (ferror(file))
  {
    perror("I/O error when reading.");
    goto clean_up;
  }

  fclose(file);
  return SUCCESS;

clean_up:
  fclose(file);
  return FAILURE;
}

Status save_file(Editor* editor)
{
  if (editor == nullptr)
    return FAILURE;

  const char* filename = editor->filename;
  if (filename == nullptr)
    return FAILURE;

  FILE *file = fopen(filename, "w");
  if (file == nullptr)
  {
    perror("Unable to open file.");
    return FAILURE;
  }

  gap_buffer* buffer = editor->buffer;
  size_t logic_text_len = buffer->gap_start + (buffer->capacity - buffer->gap_end);

  for (size_t i = 0; i < logic_text_len; i++)
  {
    char c = gb_char_at(buffer, i);
    fputc(c, file);
  }

  if (ferror(file))
  {
    perror("I/O error when writing.");
    goto clean_up;
  }

  fclose(file);
  return SUCCESS;

clean_up:
  fclose(file);
  return FAILURE;
}
