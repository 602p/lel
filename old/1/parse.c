#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "parse.h"

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
			while(text[i]!='(' && text[i]!=')' && text[i]!=' '  && text[i]!='\n' && text[i]!='\t' && i<text_len){
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

void assert(bool s, char* msg){
	if(!s){
		printf("F: (parse) assert: ");
		printf("%s\n", msg);
		exit(2);
	}
}

lvalue* do_parse(char*** tokens);

lvalue* do_parse_list(char*** tokens){
	assert(strcmp(**tokens, "(")==0, "List not starting with (");
	(*tokens)++;
	lvalue* list = nil;
	lvalue* tail = nil;
	while(strcmp(**tokens, ")")!=0){
		if(tail==nil){
			tail=list=CONS(do_parse(tokens), nil);
		}else{
			tail->v.cons.rhs=CONS(do_parse(tokens), nil);
			tail=tail->v.cons.rhs;
		}
	}
	assert(strcmp(**tokens, ")")==0, "List not ending with ) ??");
	(*tokens)++;
	return list;
}

lvalue* do_parse_scalar(char*** tokens){
	char* token = **tokens;
	(*tokens)++;

	bool is_num = true;
	for(int i=0; i<strlen(token); i++){
		if(!isdigit(token[i])){
			is_num = false;
			break;
		}
	}

	if(is_num){
		return SCALAR(atoi(token));
	}else{
		return make_string(token);
	}
}

lvalue* do_parse(char*** tokens){
	if(strcmp(**tokens, "(")==0) return do_parse_list(tokens);
	else return do_parse_scalar(tokens);
}

lvalue* parse(char** tokens){
	return do_parse(&tokens);
}