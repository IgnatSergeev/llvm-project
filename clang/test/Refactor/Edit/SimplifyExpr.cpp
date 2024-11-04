// RUN: clang-refactor simplify-expr -location=test:%s %s -- 2>&1 | grep -v CHECK | FileCheck %s

bool foo = true;
bool bar = false /*loc and_false_lhs=*/ && foo;

// CHECK: 1 'and_false_lhs' results:
// CHECK: bool bar = false;

bool foo2 = true;
bool bar2 = foo2 /*loc and_false_rhs=*/ && false;

// CHECK: 1 'and_false_rhs' results:
// CHECK: bool bar2 = false;

bool foo3 = true;
bool bar3 = true /*loc and_true_lhs=*/ && foo3;

// CHECK: 1 'and_true_lhs' results:
// CHECK: bool bar3 = foo3;

bool foo4 = true;
bool bar4 = foo4 /*loc and_true_rhs=*/ && true;

// CHECK: 1 'and_true_rhs' results:
// CHECK: bool bar4 = foo4;

bool foo5 = true;
bool bar5 = /*loc no_matching_location=*/ true || foo5;

// CHECK: 1 'no_matching_location' results:
// CHECK-NEXT: refactoring action can't be initiated without a matching location

bool foo6 = true;
bool bar6 = false /*loc or_false_lhs=*/ || foo6;

// CHECK: 1 'or_false_lhs' results:
// CHECK: bool bar6 = foo6;

bool foo7 = true;
bool bar7 = foo7 /*loc or_false_rhs=*/ || false;

// CHECK: 1 'or_false_rhs' results:
// CHECK: bool bar7 = foo7;

bool foo8 = true;
bool bar8 = true /*loc or_true_lhs=*/ || foo8;

// CHECK: 1 'or_true_lhs' results:
// CHECK: bool bar8 = true;
//
bool foo9 = true;
bool bar9 = foo9 /*loc or_true_rhs=*/ || true;

// CHECK: 1 'or_true_rhs' results:
// CHECK: bool bar9 = true;

