#!/usr/bin/python
# Variant Expansion * see README.md and LICENSE

import sys

def push(s, v):
  s.append(v)
def peek(s):
  return s[len(s)-1]
def pop(s):
  return s.pop()
def exch(s):
  assert len(s) <= 2
  s[-2:] = s[-1],s[-2]

def append(s, c):
  s.append(c)
def trunc(s, n):
  del s[n:]
def join(s):
  return ''.join(s)


def parse(text, down, side):
  """Fill the down and side arrays with link indices into text"""
  stk = []                   # stack of OPEN and PIPE indices
  first_top_level_pipe = -1  # index of PIPE outside OPEN/CLOSE
  for i,c in enumerate(text):
    if c=='[':
      push(stk, i)  # remember bracket position
      continue
    if c=='|':
      if len(stk) > 0:
        j = peek(stk)
        side[j] = i
      else:
        first_top_level_pipe = i  # next[-1] = i
      push(stk, i)  # remember pipe position
      continue
    if c==']':
      while len(stk) > 0:
        j = pop(stk)
        down[j] = i+1
        if text[j] == '[': break
      down[i] = i+1
      continue
    down[i] = i+1

  # Implicitly close unmatched opening brackets and end of text:
  while len(stk) > 0:
    j = pop(stk)
    down[j] = len(text)

  return first_top_level_pipe


def traverse(text, down, side, stk, acc):
  """traverse dag defined by down and side indices, yield variants"""
  n = len(text)
  while len(stk) > 0:
    index, reset = pop(stk)
    trunc(acc, reset)
    while index < n:
      if text[index] == '[':
        reset = len(acc)
        if side[index] > 0:  # two or more variants
          push(stk, (side[index]+1, reset))
          index += 1
        else:  # optional part: treat [x] as [|x]
          push(stk, (index+1, reset))
          index = down[index]
        continue
      if text[index] == '|':
        if side[index] > 0:
          push(stk, (side[index]+1, reset))
          exch(stk)  # ensure variant ordering
        index = down[index]
        continue
      if text[index] == ']':
        index += 1
        continue
      append(acc, text[index])
      index += 1
    if len(acc) > 0:
      yield join(acc)  # emit non-empty variant


def expand(text):
  n = len(text)
  down, side = [0]*n, [0]*n
  pipe = parse(text, down, side)
  acc = [] # variant accumulator
  stk = [] # dag traversal stack
  if pipe > 0: # 1st top level pipe
    push(stk, (pipe+1, 0))
  push(stk, (0, 0))
  for v in traverse(text, down, side, stk, acc):
    print(v)


def main(argv=None):
  if argv is None:
    argv = sys.argv
  me = argv[0]
  for arg in argv[1:]:
    expand(arg)
  return 0


if __name__ == "__main__":
  sys.exit(main())

