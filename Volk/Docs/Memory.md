# Idea 1 - Default Const Reference
All instantiations are treated as if they were const-valued unique_ptr.
To allow a function to modify the an in parameter, add the `mut` keyword before the parameter name.
Memory management is handled at the end of every scope, where all owned instantiations are cleaned up.