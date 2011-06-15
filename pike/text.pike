int main() {
	write("%O\n", 
	      psyc_text("some template with a single variable '[_foo_ba]' done.", ([ "_foo" : "abc" ])));
	      //psyc_text("some template [_foo] with a single variable '[_foo_ba]' done.", ([ "_foo" : "abc" ])));
}
