#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xfb8e7d2d, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x26e6bf9e, __VMLINUX_SYMBOL_STR(elv_unregister) },
	{ 0x763cc79d, __VMLINUX_SYMBOL_STR(elv_register) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xfc05dcb5, __VMLINUX_SYMBOL_STR(elv_dispatch_sort) },
	{ 0x50eedeb8, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x2a75e5ae, __VMLINUX_SYMBOL_STR(kobject_put) },
	{ 0xf1faac3a, __VMLINUX_SYMBOL_STR(_raw_spin_lock_irq) },
	{ 0x8871e67c, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xd9a79435, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x13062a39, __VMLINUX_SYMBOL_STR(elevator_alloc) },
	{ 0xb4390f9a, __VMLINUX_SYMBOL_STR(mcount) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "2FDA8BCF5C5E1162C9EABFB");
