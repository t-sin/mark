
lis_obj make_t();
lis_obj make_nil();
lis_obj make_int(lis_int n);
lis_obj make_char(lis_int ch);
lis_obj make_array();

lis_obj * eval(lis_obj * obj);
void print(lis_obj * obj);