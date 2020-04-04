test_utf8 = executable('test_utf8', [
  'utf8.c',
  '../src/util/utf8.c',
])
test('utf8 encoding/decoding', test_utf8)

test_byte_stream = executable('test_byte_stream', [
  'byte_stream.c',
  '../src/util/byte_stream.c',
])
test('fundamental streams', test_byte_stream)

test_stream = executable('test_stream', [
  'stream.c',
  '../src/core/stream.c',
  '../src/core/obj.c',
  '../src/util/byte_stream.c',
  '../src/util/utf8.c',
])
test('high-level streams', test_stream)
