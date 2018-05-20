#include "data.h"
#include <stdio.h>

lvalue* result_assert(lvalue_result v){
	if(!v.success){
		puts("F: result_assert\n");
		exit(1);
	}
	return v.data;
}

lvalue* make_scalar(int val){
	lvalue* data = malloc(sizeof(lvalue));
	data->scalar = true;
	data->v.scalar = val;
	return data;
}

lvalue* make_cons(lvalue* lhs, lvalue* rhs){
	lvalue* data = malloc(sizeof(lvalue));
	data->scalar = false;
	data->v.cons.lhs = lhs;
	data->v.cons.rhs = rhs;
	return data;
}

lvalue* make_list(int nr, ...){
    va_list args;
    va_start(args, nr);

    lvalue* list = nil;
    lvalue* tail;
 
    for(int i=0;i<nr;i++){
    	lvalue* item = va_arg(args, lvalue*);
    	if(list==nil){
    		list=tail=CONS(item, nil);
    	}else{
    		tail->v.cons.rhs=CONS(item, nil);
    		tail=tail->v.cons.rhs;
    	}
    }
 
    va_end(args);

    return list;
}

lvalue* make_string(char* str){
	lvalue* list = nil;
    lvalue* tail;
 
    for(int i=0;str[i]!='\0';i++){
    	if(list==nil){
    		list=tail=CONS(SCALAR(str[i]), nil);
    	}else{
    		tail->v.cons.rhs=CONS(SCALAR(str[i]), nil);
    		tail=tail->v.cons.rhs;
    	}
    }

    return list;
}

bool lvalue_eq(lvalue* a, lvalue* b){
	if(a == b){
		return true;
	}else if(a->scalar && b->scalar){
		return a->v.scalar == b->v.scalar;
	}else if (!a->scalar && !b->scalar){
		return lvalue_eq(a->v.cons.lhs, b->v.cons.lhs) &&
				lvalue_eq(a->v.cons.rhs, b->v.cons.rhs);
	}
}

bool lvalue_eq_ascii(lvalue* l, char* str){
	if(l==nil && *str=='\0') return true;
	if(!l->scalar && l->v.cons.lhs->scalar){
		return l->v.cons.lhs->v.scalar == str[0] &&
				lvalue_eq_ascii(l->v.cons.rhs, str+1);
	}
}

lvalue_result assoclist_get(lvalue* list, lvalue* key){
	if(list==nil){
		return FAILURE;
	}else if(list->scalar){
		return FAILURE;
	}else{
		lvalue* head = list->v.cons.lhs;
		if(head->scalar){
			return FAILURE;
		}else{
			if(lvalue_eq(head->v.cons.lhs, key)){
				return SUCCEED(head->v.cons.rhs);
			}else{
				return assoclist_get(list->v.cons.rhs, key);
			}
		}
	}
}

lvalue_result assoclist_rev_get(lvalue* list, lvalue* key){
	if(list==nil) return FAILURE;
	if(list->scalar){
		return FAILURE;
	}else{
		lvalue* head = list->v.cons.lhs;
		if(head->scalar){
			return FAILURE;
		}else{
			if(lvalue_eq(head->v.cons.rhs, key)){
				return SUCCEED(head->v.cons.lhs);
			}else{
				return assoclist_get(list->v.cons.rhs, key);
			}
		}
	}
}

lvalue_result assoclist_get_str(lvalue* list, char* key){
	if(list==nil) return FAILURE;
	if(list->scalar){
		return FAILURE;
	}else{
		lvalue* head = list->v.cons.lhs;
		if(head->scalar){
			return FAILURE;
		}else{
			if(lvalue_eq_ascii(head->v.cons.lhs, key)){
				return SUCCEED(head->v.cons.rhs);
			}else{
				return assoclist_get_str(list->v.cons.rhs, key);
			}
		}
	}
}

int list_len(lvalue* list){
	if(list==nil) return 0;
	if(list->scalar) return -1;
	int i=1; //If not scalar, at least one item
	while(!list->v.cons.rhs->scalar){
		i+=1;
		list=list->v.cons.rhs;
	}
	return i;
}

lvalue* list_get(lvalue* list, int pos){
	if(list->scalar) return nil;
	int i=0;
	while(!list->scalar){
		if(i==pos) return list->v.cons.lhs;
		i+=1;
		list=list->v.cons.rhs;
	}
	return nil;
}

void lvalue_free(lvalue* l){
	if(gc_get_used(l)) return;
	if(!l->scalar){
		lvalue* lhs = l->v.cons.lhs;
		lvalue* rhs = l->v.cons.rhs;
		l->v.cons.lhs = l->v.cons.rhs = nil;
		if(!gc_get_used(lhs)) lvalue_free(lhs);
		if(!gc_get_used(rhs)) lvalue_free(rhs);
	}
	free(l);
}

bool gc_get_used_in(lvalue* structure, lvalue* l){
	if(structure==l) return true;
	if(!structure->scalar) return gc_get_used_in(structure->v.cons.lhs, l) ||
									gc_get_used_in(structure->v.cons.rhs, l);
}

bool gc_get_used(lvalue* l){
	gc_cell* curr = gc_root;
	while(curr!=NULL){
		if(gc_get_used_in(curr->data, l)) return true;
	}
	return false;
}