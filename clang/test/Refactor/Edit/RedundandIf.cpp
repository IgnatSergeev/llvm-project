// RUN: clang-refactor redundand-if -location=test:%s %s -- 2>&1 | grep -v CHECK | FileCheck %s

void bodyLocation() {
    if (false) {
        int thenVar;
        /*loc body_loc=*/
    }
}

// CHECK: 1 'body_loc' results:
// CHECK: void bodyLocation() {
// CHECK-NEXT: {{^     }}
// CHECK-NEXT: }

void ifFalse() {
    if (/*loc false=*/false) {
        int thenVar;
    } else {
        int elseVar;
    }
}

// CHECK: 1 'false' results:
// CHECK: void ifFalse() {
// CHECK-NEXT: {
// CHECK-NEXT: int elseVar;
// CHECK-NEXT: }
// CHECK-NEXT: }

void ifFalseNoElse() {
    if (/*loc false_no_else=*/false) {
        int thenVar;
    }
}

// CHECK: 1 'false_no_else' results:
// CHECK: void ifFalseNoElse() {
// CHECK-NEXT: {{^     }}
// CHECK-NEXT: }

void noMatchingLocation() {
    /*loc no_matching_location=*/
    if (false) {
        int thenVar;
    }
}

// CHECK: 1 'no_matching_location' results:
// CHECK-NEXT: refactoring action can't be initiated without a matching location

void ifTrue() {
    if (/*loc true=*/true) {
        int thenVar;
    } else {
        int elseVar;
    }
}

// CHECK: 1 'true' results:
// CHECK: void ifTrue() {
// CHECK-NEXT: {
// CHECK-NEXT: int thenVar;
// CHECK-NEXT: }
// CHECK-NEXT: }
