// RUN: clang-refactor unused-field -location=test:%s %s -- 2>&1 | grep -v CHECK | FileCheck %s

struct Bar {
    /*loc used=*/int member;
    /*loc unused=*/int unusedMember;
};

void foo() {
    Bar a;
    a.member = 1;
}

// CHECK: 1 'unused' results:
// CHECK: struct Bar {
// CHECK-NEXT: /*loc used=*/int member;
// CHECK-NEXT: /*loc unused=*/
// CHECK-NEXT: };

// CHECK: 1 'used' results:
// CHECK-NEXT: refactoring action can't be initiated without a matching location
