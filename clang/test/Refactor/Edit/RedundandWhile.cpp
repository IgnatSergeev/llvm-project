// RUN: clang-refactor redundand-while -location=test:%s %s -- 2>&1 | grep -v CHECK | FileCheck %s

void bodyLocation() {
    while (false) {
        int thenVar;
        /*loc body_loc=*/
    }
}

// CHECK: 1 'body_loc' results:
// CHECK: void bodyLocation() {
// CHECK-NEXT: {{^     }}
// CHECK-NEXT: }

void conditionLoc() {
    while (/*loc false=*/false) {
        int thenVar;
    }
}

// CHECK: 1 'false' results:
// CHECK: void conditionLoc() {
// CHECK-NEXT: {{^     }}
// CHECK-NEXT: }

void noMatchingLocation() {
    /*loc no_matching_location=*/
    while (false) {
        int thenVar;
    }
}

// CHECK: 1 'no_matching_location' results:
// CHECK-NEXT: refactoring action can't be initiated without a matching location

