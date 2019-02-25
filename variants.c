/* Variant Expansion * see README.md and LICENSE */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXIT_MEM 1  /* cannot allocate memory */

static char *me = "variants";

/*** Stack ***/

typedef struct Stack {
  int *data;
  int size, top;
} Stack;

static void stack_init(Stack *pStk, int initsize);
static void stack_grow(Stack *pStk, int newsize);
static void stack_free(Stack *pStk);
static void stack_push(Stack *pStk, int value);
static int stack_peek(Stack *pStk);
static int stack_pop(Stack *pStk);
static int stack_count(Stack *pStk);

static void stack_init(Stack *pStk, int initsize)
{
  assert(pStk != NULL);
  pStk->data = NULL;
  pStk->size = 0;
  pStk->top = 0;
  stack_grow(pStk, initsize);
}

static void stack_grow(Stack *pStk, int newsize)
{
  int *newdata;
  assert(pStk != NULL);
  if (newsize < 4) newsize = 4;
  newdata = realloc(pStk->data, newsize);
  if (!newdata) {
    perror("Cannot stack_grow");
    exit(EXIT_MEM);
  }
  pStk->data = newdata;
  pStk->size = newsize;
}

static void stack_free(Stack *pStk)
{
  assert(pStk != NULL);
  if (pStk->data) free(pStk->data);
  pStk->data = NULL;
  pStk->size = 0;
  pStk->top = 0;
}

static void stack_push(Stack *pStk, int value)
{
  assert(pStk != NULL);
  if(pStk->top >= pStk->size){
    stack_grow(pStk, 2*pStk->size);
  }
  pStk->data[pStk->top++] = value;
}

static int stack_peek(Stack *pStk)
{
  assert(pStk != NULL);
  assert(pStk->top > 0);
  return pStk->data[pStk->top-1];
}

static int stack_pop(Stack *pStk)
{
  assert(pStk != NULL);
  assert(pStk->top > 0);
  return pStk->data[--pStk->top];
}

static int stack_count(Stack *pStk)
{
  assert(pStk != NULL);
  return pStk->top;
}

/*** Buffer ***/

typedef struct Buffer {
  char *data;
  int size, len;
} Buffer;

static void buffer_init(Buffer *pBuf, int initsize);
static void buffer_grow(Buffer *pBuf, int newsize);
static void buffer_free(Buffer *pBuf);
static void buffer_append(Buffer *pBuf, char c);
static void buffer_truncate(Buffer *pBuf, int len);
static int buffer_length(Buffer *pBuf);
static const char *buffer_string(Buffer *pBuf); // ptr to \0 term'd string

static void buffer_init(Buffer *pBuf, int initsize)
{
  assert(pBuf!=NULL);
  pBuf->data = NULL;
  pBuf->size = 0;
  pBuf->len = 0;
  buffer_grow(pBuf, initsize);
}

static void buffer_grow(Buffer *pBuf, int newsize)
{
  char *newdata;
  assert(pBuf!=NULL);
  if (newsize < 4) newsize = 4;
  newdata = realloc(pBuf->data, newsize);
  if (!newdata) {
    perror("Cannot buffer_grow");
    exit(EXIT_MEM);
  }
  pBuf->data = newdata;
  pBuf->size = newsize;
}

static void buffer_free(Buffer *pBuf)
{
  assert(pBuf!=NULL);
  if(pBuf->data) free(pBuf->data);
  pBuf->data = NULL;
  pBuf->size = 0;
  pBuf->len = 0;
}

static void buffer_append(Buffer *pBuf, char c)
{
  assert(pBuf!=NULL);
  if(pBuf->len >= pBuf->size){
    buffer_grow(pBuf, 2*pBuf->size);
  }
  pBuf->data[pBuf->len++] = c;
}

static void buffer_truncate(Buffer *pBuf, int len)
{
  assert(pBuf!=NULL);
  assert(len<=pBuf->len);
  pBuf->len = len;
}

static int buffer_length(Buffer *pBuf)
{
  assert(pBuf!=NULL);
  return pBuf->len;
}

static const char *buffer_string(Buffer *pBuf)
{
  assert(pBuf!=NULL);
  if(pBuf->data[pBuf->len] != '\0'){
    buffer_append(pBuf, '\0');
  }
  return pBuf->data;
}

/*** Debugging ***/

static void
printposn(const char *prefix, int n)
{
  int i;
  if (prefix) fprintf(stderr, "%s:", prefix);
  for (i=0; i<n; i++) fprintf(stderr, " %X", i);
  fprintf(stderr, "\n");
}

static void
printtext(const char *prefix, const char *s, int n)
{
  int i;
  if (prefix) fprintf(stderr, "%s:", prefix);
  for (i=0; i<n; i++) fprintf(stderr, " %c", s[i]);
  fprintf(stderr, "\n");
}

static void
printlnks(const char *prefix, const int *a, int n)
{
  int i;
  if (prefix) fprintf(stderr, "%s:", prefix);
  for (i=0; i<n; i++) fprintf(stderr, a[i]?" %X":" -", a[i]);
  fprintf(stderr, "\n");
}

/*** Variant Expansion ***/

static void expand(const char *text, int len);
static int parse(const char *text, int len, int *down, int *side);
static void traverse(const char *text, int len,
                     const int *down, const int *side, int i, Buffer *buf);

static void
expand(const char *text, int len)
{
  int pipe, reset;
  int *down, *side;
  Buffer buf;

  down = calloc(len, sizeof(int));
  if (!down) { exit(EXIT_MEM); }
  side = calloc(len, sizeof(int));
  if (!side) { free(down); exit(EXIT_MEM); }

  // posn: 0 1 2 3 4 5 6 7 8 9 A B
  // text: a [ b [ c ] | d | e ] $
  // down: : B : 6 : : B : B : :     one after closing bracket
  // down: 1 : 3 : 5 6 : 8 : A B     down[i] > i for all i
  // side: - 6 - - - - 8 - - - -     index of next pipe

  pipe = parse(text, len, down, side);

//  printposn("posn", len);
//  printtext("text", text, len);
//  printlnks("down", down, len);
//  printlnks("side", side, len);

  buffer_init(&buf, len);

  // Expand the first top-level variant:
  reset = buffer_length(&buf);
  traverse(text, len, down, side, 0, &buf);

  // Expand remaining top-level variants:
  while (pipe > 0) {
    buffer_truncate(&buf, reset);
    traverse(text, len, down, side, pipe+1, &buf);
    pipe = side[pipe];
  }

  buffer_free(&buf);

  free(side);
  free(down);
}

static int
parse(const char *text, int len, int *down, int *side)
{
  Stack stk;                     // stack of OPEN and PIPE indices
  int first_top_level_pipe = -1; // index of PIPE outside OPEN/CLOSE
  int i;                         // index into text

  stack_init(&stk, 0);

  for (i=0; i < len; i++) {
    switch (text[i]) {
      case '[':
        stack_push(&stk, i); // remember bracket position
        break;
      case '|':
        if (stack_count(&stk) > 0) {
          int j = stack_peek(&stk);
          side[j] = i;
        }
        else {
          first_top_level_pipe = i; // side[-1] = i
        }
        stack_push(&stk, i); // remember pipe position
        break;
      case ']':
        while (stack_count(&stk) > 0) {
          int j = stack_pop(&stk);
          down[j] = i+1;
          if (text[j]=='[') break;
        }
        down[i] = i+1;
        break;
      default:
        down[i] = i+1;
        break;
    }
  }

  // Implicitly close unmatched opening brackets at end of text:

  while (stack_count(&stk) > 0) {
    int j = stack_pop(&stk);
    down[j] = i;
  }

  stack_free(&stk);

  return first_top_level_pipe;
}

static void
traverse(const char *text, int len, const int *down, const int *side,
         int i, Buffer *buf)
{
  int reset;

  while (i < len) {
    switch (text[i]) {
    case '[':
      reset = buffer_length(buf); // remember prefix length
      traverse(text, len, down, side, i+1, buf);
      if (side[i] <= 0) { // optional part: [x]
        buffer_truncate(buf, reset); // truncate to prefix
        traverse(text, len, down, side, down[i], buf);
      }
      else while (side[i] > 0) { // variants: [x|y...]
        i = side[i];
        buffer_truncate(buf, reset); // truncate to prefix
        traverse(text, len, down, side, i+1, buf);
      }
      return; // all done by recursive calls
    case '|':
      i = down[i];
      break;
    case ']':
      i += 1;
      break;
    default:
      buffer_append(buf, text[i]);
      i += 1;
      break;
    }
  }

  if (buffer_length(buf) > 0) {
    puts(buffer_string(buf)); // emit non-empty variant
  }
}

int
main(int argc, char **argv)
{
  if (argv && *argv) me = *argv++;
  else return 127; // invalid invocation

  while (*argv) {
    int len = strlen(*argv);
    expand(*argv, len);
    argv++; argc--; // shift
  }

  return 0;
}
