#!/usr/bin/python
# Variant Expansion * see README.md and LICENSE

import sys

def push(s, v):
  s.append(v)
def peek(s):
  return s[len(s)-1]
def pop(s):
  return s.pop()

def append(s, c):
  s.append(c)
def trunc(s, n):
  del s[n:]
def join(s):
  return ''.join(s)


def parse(s, down, side):
  """Fill the down and side arrays with link indices into s"""
  stk = []                   # stack of OPEN and PIPE indices
  first_top_level_pipe = -1  # index of PIPE outside OPEN/CLOSE
  for i,c in enumerate(s):
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
        if s[j] == '[': break
      down[i] = i+1
      continue
    down[i] = i+1

  # Implicitly close unmatched opening brackets and end of text:
  while len(stk) > 0:
    j = pop(stk)
    down[j] = len(s)

  return first_top_level_pipe


def traverse(s, down, side, i, acc):
  """traverse the dag defined by down and side indices, emit variants"""
  n = len(s)
  while i < n:
    if s[i] == '[':
      reset = len(acc)  # remember prefix length
      if side[i] <= 0:  # optional part: [x]
        traverse(s, down, side, down[i], acc)
        trunc(acc, reset)  # truncate to prefix
        traverse(s, down, side, i+1, acc)
      else:
        traverse(s, down, side, i+1, acc)
        while side[i] > 0:  # variants: [x|y...]
          i = side[i]
          trunc(acc, reset)  # truncate to prefix
          traverse(s, down, side, i+1, acc)
      return  # all done by recursive calls
    if s[i] == '|':
      i = down[i]
      continue
    if s[i] == ']':
      i += 1
      continue
    append(acc, s[i])
    i += 1
  
  if len(acc) > 0:
    print(join(acc))  # emit non-empty variant


def expand(s):
  n = len(s)
  down, side = [0]*n, [0]*n
  pipe = parse(s, down, side)
  acc = [] # variant accumulator
  # Expand the first top-level variant:
  reset = len(acc)
  traverse(s, down, side, 0, acc)
  # Expand remaining top-level variants:
  while pipe > 0:
    trunc(acc, reset)
    traverse(s, down, side, pipe+1, acc)
    pipe = side[pipe]

  
me = sys.argv[0]
for arg in sys.argv[1:]:
  expand(arg)


