#!/bin/bash
python run-clang-tidy.py \
$(pwd)/src/ \
-header-filter="$(pwd)/src" \
-checks=-*,\
clang-analyzer-core*,\
clang-analyzer-security*,\
performance-faster-string-find,\
performance-inefficient-vector-operation,\
performance-type-promotion-in-math-fn,\
performance-unnecessary-copy-initialization,\
performance-unnecessary-value-param,\
misc-misplaced-const,\
misc-unused-using-decls,\
misc-throw-by-value-catch-by-reference,\
readability-const-return-type,\
readability-container-size-empty,\
readability-non-const-parameter,\
readability-redundant-declaration,\
readability-redundant-function-ptr-dereference,\
readability-simplify-subscript-expr,\
readability-static-accessed-through-instance,\
readability-redundant-string-init,\
readability-implicit-bool-conversion,\
readability-simplify-boolean-expr,\
bugprone-incorrect-roundings,\
modernize-use-nullptr,\
modernize-loop-convert,\
modernize-use-equals-default \
-p $(pwd)/build/ -fix -format -j 12
