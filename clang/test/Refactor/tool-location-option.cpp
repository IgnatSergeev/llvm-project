// RUN: rm -f %t.cp.cpp
// RUN: cp %s %t.cp.cpp
// RUN: clang-refactor simplify-expr -location=%t.cp.cpp:5:18 -v %t.cp.cpp -- | FileCheck --check-prefix=CHECK1 %s

bool test = true && true;

// CHECK1: invoking action 'simplify-expr':
// CHECK1-NEXT: -location={{.*}}.cp.cpp:5:18

// RUN: not clang-refactor simplify-expr -location=%s:5:18 -v %t.cp.cpp -- 2>&1 | FileCheck --check-prefix=CHECK-FILE-ERR %s
// CHECK-FILE-ERR: given file is not in the target TU
