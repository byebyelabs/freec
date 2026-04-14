# freec

### Malloc Violations

#### Dangling Pointers
- [] No store directly after `malloc`
- [] Return from `root` without `free`
- [] Pass into 
- [] Conditional `free`s that does not `free` in each case.
- 

#### Use After Free
- [] Pass into a function that `free`s pointer, and `free` again in parent.


### Appendix
1. `root` of the program: entry point, usually `main` function.
2. 
