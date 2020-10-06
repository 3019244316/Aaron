#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, UEQ, logical_AND, logical_OR, logical_NOT, reg, variable, number, hex
	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus
	{"==", EQ},					// equal
	{"\\(", '('},				        // left parenthesis  
	{"\\)", ')'},					// right parenthesis  
	{"\\*", '*'},         				// multiplication  
	{"/", '/'},          				// division  
	{"-", '-'},          				// subtraction  
	{"!=", UEQ},       				// unequal  
	{"&&", logical_AND}, 				  
	{"\\|\\|", logical_OR},  			 
	{"!", logical_NOT},  				 
	{"0[xX][A-Fa-f0-9]{1,8}", hex}, 		 
	{"\\$[a-dA-D][hlHL]|\\$[eE]?(ax|dx|cx|bx|bp|si|di|sp)", reg}, //register  
	{"[a_zA_Z_][a-zA-Z0-9_]*", variable},		 
	{"[0-9]{1,10}", number}                		

};
    

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

bool check_parentheses(int p, int q);

int eval(int p, int q);

int find_dominant_operator(int p, int q);


static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {

		                    case 257:  
                		        tokens[nr_token].type = 257;  
		                        strcpy(tokens[nr_token].str,"==");  
                		        break;  
		                    case 40:  
                		        tokens[nr_token].type = 40;  
		                        break;  
                		    case 41:  
		                        tokens[nr_token].type = 41;  
                		        break;  
		                    case 42:  
                		        tokens[nr_token].type = 42;  
		                        break;  
                		    case 47:  
		                        tokens[nr_token].type = 47;  
		                        break;  
		                    case 43:  
		                        tokens[nr_token].type = 43;  
		                        break;  
		                    case 45:  
		                        tokens[nr_token].type = 45;  
		                        break;  
		                    case 258:  
		                        tokens[nr_token].type = 258;  
		                        strcpy(tokens[nr_token].str, "!=");  
		                        break;  
		                    case 259:  
		                        tokens[nr_token].type = 259;  
		                        strcpy(tokens[nr_token].str, "&&");  
		                        break;  
		                    case 260:  
		                        tokens[nr_token].type = 260;  
		                        strcpy(tokens[nr_token].str, "||");  
		                        break;  
		                    case 261:  
		                        tokens[nr_token].type = 261;  
		                        break;  
		                    case 262:  
		                        tokens[nr_token].type = 262;  
		                        strncpy(tokens[nr_token].str, &e[position-substr_len], substr_len);  
		                        break;  
		                    case 263:  
		                        tokens[nr_token].type = 263;  
		                        strncpy(tokens[nr_token].str, &e[position-substr_len], substr_len);  
		                        break;
				    case 264:
					tokens[nr_token].type = 264;  
		                        strncpy(tokens[nr_token].str, &e[position-substr_len], substr_len);  
		                        break;
		                    case 265:  
		                        tokens[nr_token].type = 265;  
		                        strncpy(tokens[nr_token].str, &e[position-substr_len], substr_len);  
		                        break;
		                    default:  
		                        nr_token--;  
		                        break;  
				}

				nr_token++; 
				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	nr_token--; 
	return true; 
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	
	int p = 0; 
	int q = nr_token;
	return eval(p, q);
}


int eval(int p, int q)
{  
        int i = 0;  
        if(p>q) 
                assert(0);  
        else if(p==q)  
        {  
  
                if(tokens[p].type==264) 
                {  
                        sscanf(tokens[p].str,"%d", &i);  
                        return i;  
                }  
                else if(tokens[p].type==265) 
                {  
                        sscanf(tokens[p].str,"%x", &i);  
                        return i;  
                } 
 
                else if(tokens[p].type==262)  
                
		/*
		{  
                        int j = 0,sl = 1,sw = 1;  
                        for( ; j<8 && sl!=0 && sw!=0; j++)  
                        {  
                                sl = strcmp(tokens[p].str+1, regsl[i]);  
                                sw = strcmp(tokens[p].str+1, regsw[i]);  
                        }  
                        if(sl==0)  
                        {       i = cpu.gpr[j]._32;  
                                return i;  
                        }  
                        else if(sw==0)  
                                return cpu.gpr[j]._16;  
                        else  
                        {  
                                if(strcmp(tokens[p].str,"$al")==0)  
                                        return reg_b(0);  
                                if(strcmp(tokens[p].str+1,"cl")==0)  
                                        return reg_b(1);  
                                if(strcmp(tokens[p].str+1,"dl")==0)  
                                        return reg_b(2);  
                                if(strcmp(tokens[p].str+1,"bl")==0)  
                                        return reg_b(3);  
                                if(strcmp(tokens[p].str+1,"ah")==0)  
                                        return reg_b(4);  
                                if(strcmp(tokens[p].str+1,"ch")==0)  
                                        return reg_b(5);  
                                if(strcmp(tokens[p].str+1,"dh")==0)  
                                        return reg_b(6);  
                                if(strcmp(tokens[p].str+1,"bh")==0)  
                                        return reg_b(7);  
                        }  
                        if(j==8)  
                                assert(0);  
                }  */

		{ 

                        int j = 0, sl = 1, sw = 1, sb = 1;

			/*
			char *regsl2[] = {"$eax", "$ecx", "$edx", "$ebx", "$esp", "$ebp", "$esi","$edi"};  
			char *regsw2[] = {"$ax"  , "$cx", "$dx", "$bx", "$sp", "$pi", "$si", "$di"};
			char *regsw3[] = {"$axx"  , "$cxx", "$dxx", "$bxx", "$spx", "$pix", "$six", "$dix"};
			char *regsw4[] = {"$axp"  , "$cxp", "$dxp", "$bxp", "$spp", "$pip", "$sip", "$dip"};
			char *regsw5[] = {"$axi"  , "$cxi", "$dxi", "$bxi", "$spi", "$pii", "$sii", "$dii"};
			char *regsb2[] = {"$al"  , "$cl", "$dl", "$bl", "$ah", "$ch", "$dh", "$bh"};
			char *regsb3[] = {"$alx"  , "$clx", "$dlx", "$blx", "$ahx", "$chx", "$dhx", "$bhx"};
			char *regsb4[] = {"$alp"  , "$clp", "$dlp", "$blp", "$ahp", "$chp", "$dhp", "$bhp"};
			char *regsb5[] = {"$ali"  , "$cli", "$dli", "$bli", "$ahi", "$chi", "$dhi", "$bhi"};
			*/

                        for(; j<8 && sl!=0; j++)  
                        {  
                                sl = strcmp(tokens[p].str+1, regsl[j]);  
				if(sl==0){                           
                                	return cpu.gpr[j]._32;
				}  
                        }  
                        
			j = 0;
			char str[3];
			for( ; j<8; j++)
			{
				strncpy(str, tokens[p].str+1, 2);

				sw = strcmp(str, regsw[j]);
				printf("%s %d\n", str, sw);				
 				if(sw==0){ 
					return cpu.gpr[j]._16;				
				}
                        }

			j = 0;
			for( ; j<4; j++)
			{
				strncpy(str, tokens[p].str+1, 2);
				sb = strcmp(str, regsb[j]);				
 				if(sb==0){ 
					return cpu.gpr[j]._8[0];				
				}
                        }

			j = 0;
			for( ; j<4; j++)
			{
				strncpy(str, tokens[p].str+1, 2);
				sb = strcmp(str, regsb[j]);				
 				if(sb==0){ 
					return cpu.gpr[j]._8[1];				
				}
                        }

			/*

                      	if( strcmp(tokens[p].str+1, regsb[0])==0 )  
                         	return cpu.gpr[0]._8[0];  
                       	if( strcmp(tokens[p].str+1, regsb[1])==0 )  
                              	return cpu.gpr[1]._8[0];  
                       	if( strcmp(tokens[p].str+1, regsb[2])==0 )    
                            	return cpu.gpr[2]._8[0];  
                     	if( strcmp(tokens[p].str+1, regsb[3])==0 )  
                              	return cpu.gpr[3]._8[0];  
                      	if( strcmp(tokens[p].str+1, regsb[4])==0 )  
                             	return cpu.gpr[0]._8[1];  
			if( strcmp(tokens[p].str+1, regsb[5])==0 )  	 
                             	return cpu.gpr[1]._8[1];  
                       	if( strcmp(tokens[p].str+1, regsb[6])==0 )    
                            	return cpu.gpr[2]._8[1];  
                     	if( strcmp(tokens[p].str+1, regsb[7])==0 )     
                              	return cpu.gpr[3]._8[1];
			*/
			
                }  

                else  assert(0);  

        }  
        else if(check_parentheses(p, q)==true)  
                return eval(p+1, q-1);  
        else  
        {  
                int op, val1, val2;  
                if( (q-p==1) && tokens[p].type=='-' ) 
                        return 0-eval(q, q);  

                if( ((q-p==1) || (tokens[p+1].type=='('&&tokens[q].type==')')) && tokens[p].type==261 )  
                { 
                        i = eval(p+1, q);  
                        return !i;  
                }  
                if( ((q-p==1) || (tokens[p+1].type=='(' && tokens[q].type==')')) && tokens[p].type=='*' )  
                { 
  
                        return swaddr_read(eval(p+1, q),4);
                }  
                op = find_dominant_operator(p, q);  
  
                val1 = eval(p, op-1);  
                val2 = eval(op+1, q);  
                switch(tokens[op].type)  
                {  
                        case '+': return val1 + val2;  
                        case '-': return val1 - val2;  
                        case '*': return val1 * val2;  
                        case '/': return val1 / val2;  
                        case 257:  
                              	if(val1 == val2)  
                                        return 1;  
                              	else  
                                      	return 0;  
                        case 258:  
                                if(val1 != val2)  
                                        return 1;  
                                else  
                                        return 0;  
                        case 259:  
                                if(val1 && val2)  
                                        return 1;  
                                else  
                                        return 0;  
                        case 260:  
                                if(val1 || val2)  
                                        return 1;  
                                else  
                                        return 0;  
                        default: assert(0);  
                }  
        }
  
        return 0;  
}  




bool check_parentheses(int p, int q)  
{  
    	int left = 0;  
    	int flag = 0;  
    	if( tokens[p].type==40 )  
    	{  
        	left++; 
		int i; 
        	for(i = p+1; i <= q; i++)  
        	{  
            		if( tokens[i].type==40 )  
            		{  
                		left++;  
            		}  
            		else if( tokens[i].type==41 )  
            		{  
                		left--;  
                		if( left==0 && i!=q )   
                    			flag = 1;  
                		if( left<0 )  
                    			assert(0);  
            		}  
        	}  
        	if( left==0 && tokens[q].type==41 && flag!=1 )  
            		return 1;  
        	else if(left==0)  
            		return 0;  
        	else  
            		assert(0);  
    	}
  
    	else  
        	return 0;  

}  


int find_dominant_operator(int p, int q)  
{  
    	int theop = p;  
    	int j = p;  
    	int r = 0;  
    	for( ; j<=q; j++)  
    	{  
        	if(tokens[j].type<262&&tokens[j].type!='!')  
        	{  
            		if(tokens[j].type==40)  
            		{  
                		r++;
                		for(j=j+1; tokens[j].type!=41 || r!=1; j++)  
                		{  
                    			if(tokens[j].type==40)  
                        		r++;  
                    			if(tokens[j].type==41)  
                        		r--;  
                		}  
                		r=0;  
            		}  
            	else if( tokens[j].type==260 )
                	theop = j;  
            	else if( tokens[j].type==259 && (tokens[theop].type<260 || tokens[theop].type>=262) )  
                	theop = j;
            	else if( tokens[j].type==258 && (tokens[theop].type<259 || tokens[theop].type>=262) )  
                	theop = j;
            	else if( tokens[j].type==257 && (tokens[theop].type<258 || tokens[theop].type>=262) )  
                	theop = j;
            	else if( tokens[j].type=='+' && (tokens[theop].type<256 || tokens[theop].type>=261) )  
                	theop = j;  
            	else if( tokens[j].type=='-' && (j==p || tokens[j-1].type>=256 || tokens[j-1].type==')') && (tokens[theop].type<256 || tokens[theop].type>=261) ) 
                	theop = j;  
            	else if( tokens[j].type=='/' && (tokens[theop].type>=261 || tokens[theop].type=='(' || tokens[theop].type=='*' || tokens[theop].type=='/') )  
                	theop = j;  
            	else if( tokens[j].type=='*' && ( j==p || (tokens[j-1].type>=256 || tokens[j-1].type==')') ) && (tokens[theop].type>=261 || tokens[theop].type=='(' || tokens[theop].type=='*' || tokens[theop].type=='/') )
                	theop = j;  
        	}  
    	}  
    	return theop;  

}  


