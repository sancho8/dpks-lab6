#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/rculist.h>
#include <linux/slab.h>


MODULE_AUTHOR("Yaroshenko Oleksandr");
MODULE_DESCRIPTION("Memory allocation");
MODULE_LICENSE("Dual BSD/GPL");
static uint hellop = 1;
module_param(hellop, uint, S_IRUGO); 
MODULE_PARM_DESC(hellop, "Determines how many time to print \"Hello, world\"");

static LIST_HEAD(list_node_head);
struct linked_hello{
	struct list_head node_head;
	int event_num;
	ktime_t event_time;
};



static int __init hello_init(void)
{
	if(hellop == 0 || (hellop  >= 5 && hellop <= 10))
	{
		printk(KERN_EMERG "WARNING! Invalid argument. Continuing job...");
	}
	else if(hellop > 10) 
	{
		BUG_ON(hellop > 10);
	}
	else 
	{

		int i = 0; 
		ktime_t start, end;
		struct linked_hello* alloc_list; 

		for(i = 0; i < hellop; i++)
		{
			start = ktime_get();

			printk(KERN_EMERG "Hello, world!\n");
			
			end = ktime_get();
			
			if(i != 3)
			{
				alloc_list = (struct linked_hello*) kmalloc(sizeof(struct linked_hello), GFP_KERNEL);
			}
			else
			{
				alloc_list = (struct linked_hello*) 0;
			}
			
			*alloc_list = (struct linked_hello){
				.event_num = i,
				.event_time = start
			};

			list_add_tail(&alloc_list->node_head, &list_node_head);
		}
	}
	return 0;
}

static void __exit hello_exit(void)
{
	struct linked_hello *md, *tmp;

	printk(KERN_EMERG "Clearing memory");

	list_for_each_entry_safe(md, tmp, &list_node_head, node_head) 
	{
		printk(KERN_EMERG "%d: ", md->event_num);
		pr_info("%lldns kernel time.\n", (long long int) (md->event_time));
		list_del(&md->node_head);
		kfree(md);
	}

	BUG_ON(!list_empty(&list_node_head));
}

module_init(hello_init);
module_exit(hello_exit);
