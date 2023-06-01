#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/sched.h>

asmlinkage long sys_hello(void)
{
	printk("Hello, World!\n");
	return 0;
}

asmlinkage long sys_set_weight(long weight)
{
  if(weight < 0)
  {
    return EINVAL;
  }

  current->weight = weight;

  return 0;
}

asmlinkage long sys_get_weight(void)
{
	return current->weight;
}

asmlinkage long sys_get_ancestor_sum(void)
{
	struct task_struct *t;
	t = current;

	long sum = 0;

	do {
		sum += t->weight;
		t = t->parent;
	} while (t->pid != 1);

	return sum;
}

long sys_get_heaviest_aux(struct task_struct* task)
{
	struct list_head* node;
	long heaviest_pid = 0;
	long heaviest_weight = 0;
	struct task_struct* current_task;

	if (list_empty(&task->children))
	{
		return task->pid;
	}

	list_for_each(node, &task->children)
	{
        current_task = list_entry(node, struct task_struct, sibling);

        if(heaviest_weight < current_task->weight)
        {
            if(heaviest_pid == -1 || heaviest_pid > current_task->pid)
            {
                heaviest_pid = current_task->pid;
                heaviest_weight = current_task->weight;
            }
        }

        heaviest_pid = sys_get_heaviest_aux(current_task);
	}

	return heaviest_pid;
}

asmlinkage long sys_get_heaviest_descendant(void)
{

	return sys_get_heaviest_aux(current);
}

