class ParserWithCallback {
	inherit .Parser;
	void handle_packet(mapping rvars, mapping evars, string|void method, string|void body) {
		write("rvars -> %O\n", rvars);
		write("evars -> %O\n", evars);
		write("method-> %O\n", method);
		write("body  -> %O\n", body);
		if (!method) {
			method = "";
		}
		if (!body) {
			body = "";
		}
		/*
		string re = render(rvars, evars, method, body);
		write("\n------test:\n%s\n-----rendered:\n%s\n----\n", test, re);
		*/
	}
	void handle_error(int code) {
		write("error: %d\n", code);
	}
}

array(string) testcases = ({ 
	"00-length-no-content",
	"00-length-no-data",
	"00-length-no-value",
	"00-method-only",
	"00-no-content",
	"00-no-data",
	"00-no-entity",
	"00-no-routing",
	"00-no-value",
	"01",
	"01-length",
	"01-utf8",
	"02-list",
	"03-list",
	"04-circuit",
	"05-message-private",
	"06-message-private-remote",
	"07-dns-fake",
	"07-dns-invalid",
	"08-context-enter",
	"08-context-leave",
	"err-01-length",
	"err-02-list"
});
string basedir = "../test/packets/";

string test;

int main() {
	.Parser p = ParserWithCallback();
	for (int i = 0; i < sizeof(testcases); i++) {
		Stdio.File f = Stdio.File(basedir + testcases[i], "r");
		test = f->read();
		f->close();
		write("TEST #%d: %s\n", i, testcases[i]); 
		p->feed(test);
		write("-----\n");

	}
}
