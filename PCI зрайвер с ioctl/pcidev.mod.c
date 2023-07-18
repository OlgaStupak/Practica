#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif


static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x92997ed8, "_printk" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xde80cd09, "ioremap" },
	{ 0xb19b445, "ioread8" },
	{ 0x425364b1, "__register_chrdev" },
	{ 0xaee657ee, "__class_create" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xf2d7865d, "device_create" },
	{ 0x645620c0, "class_destroy" },
	{ 0x9bdbb0da, "__pci_register_driver" },
	{ 0x48931e20, "device_destroy" },
	{ 0x7377b2e, "class_unregister" },
	{ 0xdf1ac593, "pci_unregister_driver" },
	{ 0x541a6db8, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("pci:v00008086d0000100Esv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "4A5293EDF17C122C3D93047");
