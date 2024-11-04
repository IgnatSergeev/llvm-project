// RUN: clang-refactor simplify-expr -location=test:%s -v %s -- | FileCheck %s

/*loc =*/int test;

/*loc named=*/int test2;

/*loc = +1*/int test3;

/* loc = +100 */int test4;

/*loc named =+0*/int test5;

// CHECK: Test location group '':
// CHECK-NEXT:   89
// CHECK-NEXT:   139
// CHECK-NEXT:   176
// CHECK-NEXT: Test location group 'named':
// CHECK-NEXT:   114
// CHECK-NEXT:   195

// The following invocations are in the default group:

// CHECK: invoking action 'simplify-expr':
// CHECK-NEXT: -location={{.*}}tool-test-support-location.c:3:10

// CHECK: invoking action 'simplify-expr':
// CHECK-NEXT: -location={{.*}}tool-test-support-location.c:7:14

// CHECK: invoking action 'simplify-expr':
// CHECK-NEXT: -location={{.*}}tool-test-support-location.c:9:27

// The following invocations are in the 'named' group, and they follow
// the default invocation even if some of their locations occur prior to the
// locations from the default group because the groups are tested one-by-one:

// CHECK: invoking action 'simplify-expr':
// CHECK-NEXT: -location={{.*}}tool-test-support-location.c:5:15

// CHECK: invoking action 'simplify-expr':
// CHECK-NEXT: -location={{.*}}tool-test-support-location.c:11:18
