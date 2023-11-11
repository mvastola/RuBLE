#set environment RUBY_GC_DISABLE 1
#set environment RUBY_GC_STRESS 1

# (Failed) attempt to disable some CPU math extensions so gdb reverse debugging would work
#set environment LC_ALL C
#set environment GLIBC_TUNABLES glibc.cpu.hwcaps=-AVX,-AVX2,-AVX512F,-AVX512DQ,-AVX512_IFMA,-AVX512PF,-AVX512ER,-AVX512CD,-AVX512BW,-AVX512VL,-AVX512_VBMI,-AVX512_VBMI2,-AVX512_VNNI,-AVX512_BITALG,-AVX512_VPOPCNTDQ,-AVX512_4VNNIW,-AVX512_4FMAPS,-AVX512_VP2INTERSECT,-AVX512_FP16,-AVX_VNNI,-AVX512_BF16,-BMI1,-BMI2,-LZCNT,-MOVBE,-RTM:glibc.cpu.hwcap_mask=0
# custom libc without AVX extensions
#set environment LD_PRELOAD /usr/local/src/glibc-2.37/build/libc.so

# To debug issue with code hanging
set record full insn-number-max 20
#set disable-randomization on
#set record full insn-number-max 200000
source contrib/gdb-cpp-frame-filters.py
add-symbol-file lib/rubble/rubble.so
catch throw
#b c_debug
#b rb_check_typeddata


#b rb_exc_raise
