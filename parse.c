#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char** tokenize(char* text){
	char** tokens = malloc(strlen(text)*sizeof(char*)); //TODO: Change
	int tok_pos = 0;
	int text_len = strlen(text);
	for(int i=0;i<text_len;i++){
		// printf("Iter start, i=%i, tok_pos=%i\n", i, tok_pos);
		if(text[i]=='(' || text[i]==')'){
			// puts("Paren... ");
			tokens[tok_pos] = malloc(2);
			tokens[tok_pos][0] = text[i];
			tokens[tok_pos][1] = 0;
			// printf("Emitted token '%c'\n", text[i]);
		}else{
			// puts("Word...");
			int start=i;
			while(text[i]!='(' && text[i]!=')' && text[i]!=' ' && i<text_len){
				// printf("Word iter, i=%i, char=%c\n", i, text[i]);
				i++;
			}
			// printf("Broken, start=%i, i=%i, len=%i\n", start, i, i-start);
			if(i-start==0) continue;
			tokens[tok_pos] = malloc(i-start+1);
			memcpy(tokens[tok_pos], text+start, i-start);
			// puts("memcpy done");
			tokens[tok_pos][i-start] = 0;
			// printf("Emitted token '%s'\n", tokens[tok_pos]);
			if(text[i]=='(' || text[i]==')') i--;
		}
		tok_pos++;
	}

	tokens[tok_pos] = NULL;

	return tokens;
}