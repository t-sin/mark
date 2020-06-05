test_stream = executable('test_stream', [
  'stream.c',
  '../../src/core/basic_stream.c',
  '../../src/core/obj.c',
  '../../src/util/byte_stream.c',
  '../../src/util/utf8.c',
  '../../src/util/table.c',
])
test('high-level streams', test_stream)

test_read = executable('test_read', [
  'read.c',
  '../../src/core/obj.c',
  '../../src/core/basic_stream.c',
  '../../src/core/lerror.c',
  '../../src/core/lstring.c',
  '../../src/core/list.c',
  '../../src/core/arithmetic.c',
  '../../src/core/print.c',
  '../../src/core/package.c',
  '../../src/core/environment.c',
  '../../src/core/symbol.c',
  '../../src/core/read.c',
  '../../src/core/eval.c',
  '../../src/core/data.c',
  '../../src/util/byte_stream.c',
  '../../src/util/utf8.c',
  '../../src/util/table.c',
])
test('simple Lisp reader', test_read)
