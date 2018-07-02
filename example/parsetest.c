#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

static const char *JSMNTYPE_NAME[5] = {
	"JSMN_UNDEFINED",
	"JSMN_OBJECT",
	"JSMN_ARRAY",
	"JSMN_STRING",
	"JSMN_PRIMITIVE"
};

static int dump(const char *js, jsmntok_t *t, size_t count, int indent) {
	int i, j, k;
	if (count == 0) {
		return 0;
	}
	if (t->type == JSMN_PRIMITIVE) {
		printf("[VALUE] %.*s (size=%d)", t->end - t->start, js+t->start, t->size);
		return 1;
	} else if (t->type == JSMN_STRING) {
		printf("%s%.*s (size=%d)", t->size ? "[KEY] " : "[VALUE] ", t->end - t->start, js+t->start, t->size);
		return 1;
	} else if (t->type == JSMN_OBJECT) {
		printf("OBJECT (size=%d)\n", t->size);
		j = 0;
		for (i = 0; i < t->size; i++) {
			for (k = 0; k < indent; k++) printf("  ");
			j += dump(js, t+1+j, count-j, indent+1);
			printf(": ");
			j += dump(js, t+1+j, count-j, indent+1);
			printf("\n");
		}
		return j+1;
	} else if (t->type == JSMN_ARRAY) {
		j = 0;
		printf("ARRAY (size=%d)\n", t->size);
		for (i = 0; i < t->size; i++) {
			for (k = 0; k < indent-1; k++) printf("  ");
			printf("   - ");
			j += dump(js, t+1+j, count-j, indent+1);
			printf("\n");
		}
		return j+1;
	}
	return 0;
}


static int do_parse(const char *jsonstring) {
	int i;
	int r;
	jsmn_parser p;
	jsmntok_t t[128];	/* We expect no more than 128 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, jsonstring, strlen(jsonstring), t, sizeof(t)/sizeof(t[0]));

	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	dump(jsonstring, t, p.toknext, 1);
	
	return EXIT_SUCCESS;

}

int main(int argc, char **argv) {
	int i, n, ret;
	const char *JSON_STRINGS[] = {
		"{\"A\": [1,2,3,4], \"B\": {\"B1\": [{\"B11\":\"biunouno\"}, {\"B12\": \"biunodue\"}]}}",
		"{\"A\": 123, \"B\": \"test\", \"C\": {\"C1\": \"ciuno\", \"C2\": \"cidue\"}, \"D\":[{\"D1\":\"diuno\", \"D2\":\"didue\"},{\"D1\":\"diuno\", \"D2\":\"didue\"}], \"E\":{}, \"F\":[]}",
		"{\"L0\": {\"L1b\": 3, \"L1c\": {\"L1c1\": \"\", \"L1c4\": \"\", \"L1c7\": {\"L1c7a\": [{\"L1c7a1\": \"c7a1\", \"L1c7a3\": \"c7a3\", \"L1c7a2\": \"c7a2\"}, {\"L1c7a1\": \"c7a1\", \"L1c7a3\": \"c7a3\", \"L1c7a2\": \"c7a2\"}, {\"L1c7a1\": \"c7a1\", \"L1c7a3\": \"c7a3\", \"L1c7a2\": \"c7a2\"}], \"L1c7b\": {\"L1c7b2\": 11, \"L1c7b1\": 10}}, \"L1c6\": 4.0, \"L1c5\": [{\"L1c5a\": 6, \"L1c5c\": -1}, {\"L1c5a\": 6, \"L1c5c\": -1}]}, \"L1a\": {\"L1a3\": \"\", \"L1a1\": \"\", \"L1a2\": \"\"}, \"L1d\": \"\", \"L1e\": \"\"}}"
	};
	
	n = sizeof(JSON_STRINGS) / sizeof(char *);
	printf("Testing %d JSON string(s)\n", n);
	for (i=0; i < n; i++) {
		printf("################################################################################################################################################\n");
		printf("Parsing JSON string:\n%s\n", JSON_STRINGS[i]);
		printf("################################################################################################################################################\n");
        	ret = do_parse(JSON_STRINGS[i]);
	}

	return 0;
}
