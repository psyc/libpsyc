int main() {
	werror("render:\n%s\---\n",
	       render( ([ "_context" : "psyc://denmark.lit/~hamlet#_follow" ]), 
		       ([ "_type_content" : "text/plain", "_subject" : "to be or not to be ?",
			  "_bla" : 123 ]), 
		       "_message", 
		       "to be or not to be ?"));
}
