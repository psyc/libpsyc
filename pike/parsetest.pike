int main() {
	.Parser p = Parser();
	p->feed(":_source\tpsyc://test.psyc.eu/~foo\n"
		":_target\tpsyc://test2.psyc.eu/~bar\n"
		":_tag\tf00b4rb4z\n"
		"\n"
		":_foo\tbar\n"
		"_message_private\n"
		"OHAI\n"
		"|\n");
}
