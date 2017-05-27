#!/usr/bin/env python

# docker run --rm -it -v `pwd`:/mnt angr/angr

import angr
import simuvex
import binaryninja

#print "analyzing"

bv = binaryninja.BinaryViewType['ELF'].open('magic')
bv.update_analysis_and_wait()

#print "extracting"

sum_string = "sum is %ld\n"
strings = bv.get_strings()
sum_ref = [r for r in strings if
           sum_string == bv.read(r.start, r.length)][0]
code_ref = bv.get_code_refs(sum_ref.start)[0]
main_start = bv.get_previous_function_start_before(code_ref.address)
main_func = bv.get_function_at(main_start)
main_return = [o for o in main_func.low_level_il.basic_blocks[0] if
               binaryninja.enums.LowLevelILOperation.LLIL_RET == o.operation][0]

#print "return from main is {0:x}".format(main_return.address)

proj = angr.Project('magic',
                    load_options={'auto_load_libs': False})
add_options={simuvex.o.BYPASS_UNSUPPORTED_SYSCALL}

path_group = proj.factory.path_group(threads=1)

path_group.explore(find=main_return.address)

#print path_group.found[0].state.posix.dumps(0)
#print path_group.found[0].state.posix.dumps(1)
