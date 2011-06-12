int main() {
	.Parser p = Parser();
	p->feed(":_source\tpsyc://nb.tgbit.net/~foo\n"
		":_target\tpsyc://nb2.tgbit.net/~bar\n"
		":_tag\tf00b4rb4z\n"
		"\n"
		":_foo\tbar\n"
		"_message_private\n"
		"OHAI\n"
		"|\n");
}
