
x86_arch_dep = declare_dependency(
	include_directories: include_directories('include')
)

if host_arch == 'x86'
	host_machine_dep = x86_arch_dep
endif

if build_arch == 'x86'
	build_machine_dep = x86_arch_dep
endif
