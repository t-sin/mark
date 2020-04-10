test_utf8 = executable('test_utf8', [
  'utf8.c',
  '../../src/util/utf8.c',
])
test('utf8 encoding/decoding', test_utf8)

test_byte_stream = executable('test_byte_stream', [
  'byte_stream.c',
  '../../src/util/byte_stream.c',
])
test('fundamental streams', test_byte_stream)

test_table = executable('test_table', [
  'table.c',
  '../../src/util/table.c',
])
test('hash tables', test_table)
