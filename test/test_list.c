#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <lib.h>
#include <psyc/parse.h>
#include <psyc/render.h>
#include <psyc/syntax.h>

#define NELEMS 100

int main (int argc, char **argv) {
	uint8_t verbose = argc > 1;
	int i, k, n, ret;

	PsycParseListState listState;
	PsycList list_text, list_bin;
	PsycString elems_text[NELEMS], elems_bin[NELEMS], elem;
	char buf_text[NELEMS * 200], buf_bin[NELEMS * 200], *elems[NELEMS], **elems2 = NULL;

	struct timeval start, end;

	for (i=0; i<NELEMS; i++)
		elems_text[i] = (PsycString) PSYC_C2STR("1234567890abcdefghijklmnopqrstuvwxyz-._ 1234567890abcdefghijklmnopqrstuvwxyz-._ 1234567890abcdefghijklmnopqrstuvwxyz-._ 1234567890");

	for (i=0; i<NELEMS; i++)
		elems_bin[i]  = (PsycString) PSYC_C2STR("1234567890|abcdefghijklmnopqrstuvwxyz|_\n1234567890|abcdefghijklmnopqrstuvwxyz|_\n1234567890|abcdefghijklmnopqrstuvwxyz|_\n1234567890");

	psyc_list_init(&list_text, elems_text, PSYC_NUM_ELEM(elems_text), PSYC_LIST_NO_LENGTH);
	psyc_list_init(&list_bin, elems_bin, PSYC_NUM_ELEM(elems_bin), PSYC_LIST_CHECK_LENGTH);

	psyc_render_list(&list_text, buf_text, sizeof(buf_text));
	psyc_render_list(&list_bin, buf_bin, sizeof(buf_bin));

	printf("list_text: %ld\n%.*s\n", list_text.length, (int)list_text.length, buf_text);
	printf("list_bin: %ld\n%.*s\n", list_bin.length, (int)list_bin.length, buf_bin);

#if 1
	printf("parsing text list to elems[]\n");
	gettimeofday(&start, NULL);
	for (n = 0; n < NELEMS; n++) {
		psyc_parse_list_state_init(&listState);
		psyc_parse_list_buffer_set(&listState, buf_text, list_text.length);
		i = 0;
		do {
			ret = psyc_parse_list(&listState, &elem);
			switch (ret) {
				case PSYC_PARSE_LIST_END:
					ret = 0;
				case PSYC_PARSE_LIST_ELEM:
					if (verbose) printf("|%d: %.*s... (%ld)\n", i, 10, elem.data, elem.length);
					//elems[i] = malloc(elem.length);
					//memcpy(&elems[i++], elem.data, elem.length);
					break;
				default:
					printf("# Error while parsing list: %i\n", ret);
					ret = -1;
			}
		} while (ret > 0);
	}
	gettimeofday(&end, NULL);
	//printf("%ld ms\n", (end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec) / 1000);
	printf("%ld us\n", (end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec));
#endif

#if 1
	printf("parsing binary list to elems[]\n");
	gettimeofday(&start, NULL);
	for (n = 0; n < NELEMS; n++) {
		psyc_parse_list_state_init(&listState);
		psyc_parse_list_buffer_set(&listState, buf_bin, list_bin.length);
		i = 0;
		do {
			ret = psyc_parse_list(&listState, &elem);
			switch (ret) {
				case PSYC_PARSE_LIST_END:
					ret = 0;
				case PSYC_PARSE_LIST_ELEM:
					//if (verbose) printf("|%.*s\n", (int)elem.length, elem.data);
					if (verbose) printf("|%d: %.*s... (%ld)\n", i, 10, elem.data, elem.length);
					elems[i] = malloc(elem.length);
					memcpy(elems[i++], elem.data, elem.length);
					break;
				default:
					printf("# Error while parsing list: %i\n", ret);
					ret = -1;
			}
		} while (ret > 0);
	}
	gettimeofday(&end, NULL);
	//printf("%ld ms\n", (end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec));
	printf("%ld us\n", (end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec));
#endif

#if 1
	printf("parsing binary list to elems2[] with realloc\n");
	gettimeofday(&start, NULL);
	for (n = 0; n < NELEMS; n++) {
		psyc_parse_list_state_init(&listState);
		psyc_parse_list_buffer_set(&listState, buf_bin, list_bin.length);
		i = 0;
		do {
			ret = psyc_parse_list(&listState, &elem);
			switch (ret) {
				case PSYC_PARSE_LIST_END:
					ret = 0;
				case PSYC_PARSE_LIST_ELEM:
					if (elems2)
						elems2 = realloc(elems2, (i+1) * sizeof(char*));
					else
						elems2 = malloc((i+1) * sizeof(char*));

					elems2[i] = malloc(elem.length);
					memcpy(elems2[i++], elem.data, elem.length);
					break;
				default:
					printf("# Error while parsing list: %i\n", ret);
					ret = -1;
			}
		} while (ret > 0);
	}
	gettimeofday(&end, NULL);
	//printf("%ld ms\n", (end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec));
	printf("%ld us\n", (end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec));
#endif

#if 1
	printf("parsing binary list to elems2[] with malloc\n");
	gettimeofday(&start, NULL);
	for (n = 0; n < NELEMS; n++) {
		psyc_parse_list_state_init(&listState);
		psyc_parse_list_buffer_set(&listState, buf_bin, list_bin.length);
		i = 0;
		do {
			ret = psyc_parse_list(&listState, &elem);
			switch (ret) {
				case PSYC_PARSE_LIST_END:
					ret = 0;
				case PSYC_PARSE_LIST_ELEM:
					/*
					if (elems2)
						elems2 = realloc(elems2, (i+1) * sizeof(char*));
					else
						elems2 = malloc((i+1) * sizeof(char*));
					*/
					elems2 = malloc(sizeof(char*));
					elems2[i] = malloc(elem.length);
					memcpy(elems2[i], elem.data, elem.length);
					break;
				default:
					printf("# Error while parsing list: %i\n", ret);
					ret = -1;
			}
		} while (ret > 0);
	}
	gettimeofday(&end, NULL);
	//printf("%ld ms\n", (end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec));
	printf("%ld us\n", (end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec));
#endif

#if 1
	printf("parsing binary list to elems2[] with double-parsing\n");
	gettimeofday(&start, NULL);
	for (n = 0; n < NELEMS; n++) {
		psyc_parse_list_state_init(&listState);
		psyc_parse_list_buffer_set(&listState, buf_bin, list_bin.length);
		i = 0;
		k = 0;

		do {
			ret = psyc_parse_list(&listState, &elem);
			switch (ret) {
				case PSYC_PARSE_LIST_END:
					ret = 0;
				case PSYC_PARSE_LIST_ELEM:
					k++;
					break;
				default:
					printf("# Error while parsing list: %i\n", ret);
					ret = -1;
			}
		} while (ret > 0);

		elems2 = malloc(k * sizeof(char*));
		psyc_parse_list_buffer_set(&listState, buf_bin, list_bin.length);

		do {
			ret = psyc_parse_list(&listState, &elem);
			switch (ret) {
				case PSYC_PARSE_LIST_END:
					ret = 0;
				case PSYC_PARSE_LIST_ELEM:	
					elems2[i] = malloc(elem.length);
					memcpy(elems2[i++], elem.data, elem.length);
					break;
				default:
					printf("# Error while parsing list: %i\n", ret);
					ret = -1;
			}
		} while (ret > 0);
	}
	gettimeofday(&end, NULL);
	//printf("%ld ms\n", (end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec));
	printf("%ld us\n", (end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec));
#endif

	return 0;
}
