#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;


void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

int init=0;

WP* new_wp()
{
	if(init==0) 
	{
		init_wp_pool();
	}
	WP *p = free_;
	if(p)
	{
		free_ = free_->next;
		p->next = head;
		head = p;
		init++;
		return p;
	}
	else{
		assert(0);
	}
}

WP* scan_watchpoint()
{
	WP *p = head;
	bool success = true;
	if(p==NULL)
	{
		printf("No watchpoint\n");
		return false;
	}
	else{

		while(p)
		{
			p->new_val = expr(p->expr, &success);
			if(!success)
				printf("Fail to eval neq_val in watchpoint %d\n", p->NO);
			else{
				if(p->new_val != p->old_val)
				{
					printf("hit watchpoint %d", p->NO);
					printf("expr =%s\n", p->expr);
					printf("Old value = %#x\n New value=%#x\n", p->old_val, p->new_val);
					p->old_val = p->new_val;
					printf("Program paused\n");
					return p;
				}
			}
		}
	}

	return NULL;

}

int free_wp(int NO)
{
	WP *p = head;
	WP *pre = p;
	if(head==NULL)
	{
		printf("No watchpoint\n");
		return 0;
	}
	else if( head->NO==NO )
	{
		head = head->next;
		p->next = free_;
		free_ = p;
		printf("Watchpoint %d deleted\n", p->NO);
		return 1;
	}
	else{

		while( p->NO && p->NO!=NO )
		{
			pre = p;
			p = p->next;
		}
		if( p->NO==NO )
		{
			pre->next = p->next;
			p->next = free_;
			free_ = p;
			printf("Watchpoint %d deleted\n", p->NO);
			init--;
			return 1;
		}

	}
	return 0;
}

int set_watchpoint(char *e)
{
	WP *p;
	p = new_wp();
	printf("Set watchpoint  #%d\n", p->NO);
	strcpy(head->expr, e);
	printf("expr=%s\n", p->expr);

	bool success = true;
	p->old_val = expr(p->expr, &success);
	if(!success)
	{
		printf("Fail to eval\n");
		return 0;
	}
	else{
		printf("Old value=%#x\n", p->old_val);
	}

	return 1;
}

bool delete_watchpoint(int NO)
{
	if( free_wp(NO) )
		return true;
	else 
	{
		printf("Delete failed\n");
		return false;
	}

}

void list_watchpoint()
{
	WP *p = head;
	if(p==NULL)
	{
		printf("None\n");
	}
	else
	{
		while(p)
		{
			printf("%2d %-25s%#x\n", p->NO, p->expr, p->old_val);
			p = p->next;
		}
	}
}




