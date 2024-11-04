// RUN: clang-refactor unused-method -location=test:%s %s -- 2>&1 | grep -v CHECK | FileCheck %s

struct Bar {
    /*loc used=*/void used() {}
    /*loc unused=*/void unused() {}
};

void foo() {
    Bar a;
    a.used();
}

// CHECK: 1 'unused' results:
// CHECK: struct Bar {
// CHECK-NEXT: /*loc used=*/void used() {}
// CHECK-NEXT: /*loc unused=*/
// CHECK-NEXT: };

// CHECK: 1 'used' results:
// CHECK-NEXT: refactoring action can't be initiated without a matching location
