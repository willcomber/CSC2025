CONTENTS

1. COMPILING TESTS AND APPLICATIONS
2. RUNNING TESTS AND INTERPRETING OUTPUT
3. GENERAL APPROACH TO IMPLEMENTATION
4. INTEGER IMPLEMENTATION HINTS
5. STRING IMPLEMENTATION HINTS AND ADDITIONAL RULE
6. OBJ_STORE IMPLEMENTATION HINTS AND ADDITIONAL RULE

------------------------------------------------------------------------------
1. COMPILING TESTS AND APPLICATIONS

In the csc2025-assignment1-dist directory there is a Makefile that you can use
to compile test programs and example applications. The relevant compilation
command will create a bin subdirectory, if necessary, and output application
files to that directory.

The Makefile has a number of targets. The ones you will use most often are:

all         - to make the the test suites (test_integer, test_obj_store 
              and test_integer) and example applications (integer_app and
              string_app)
clean       - to clean up (typically before compilation) by removing the bin
              subdirectory
clean_all   - to clean everything by removing the bin directory, any core 
              dumps and the ostore directory (if it exists)
              
To compile tests, enter the following at the command line prompt in the  
csc2025-assignment1-dist directory:

    # make

To clean up before compiling do:

    # make clean
    or
    # make clean_all
    
You can clean then compile, in a single line as follows:

    # make clean; make

Assuming compilation succeeds, you will have the following binary applications
in the bin subdirectory.

    bin/
        integer_app
        string_app
        test_integer
        test_obj_store
        test_string
        
The directory will also contain some ".o" files. 

Other make targets will compile individual applications and do partial clean 
ups.

If you are developing on Linux, set CFLAGS to "-std=c99 -D_GNU_SOURCE" when
you invoke make, as follows:

    # make CFLAGS="-std=c99 -D_GNU_SOURCE"

------------------------------------------------------------------------------
2. RUNNING TESTS AND INTERPRETING OUTPUT

There are three test programs: 
    test_integer    - to run unit tests of the integer struct
    test_string     - to run unit tests of the string struct
    test_obj_store  - to test unit tests of the object store

The programs run normal case and error case tests of each of the "public" 
functions declared in integer.h, string_o.h and obj_store.h. The normal and 
error cases are often executed as separate runs.

Expected test output for correctly implemented integer.c, string_o.c and 
obj_store.c functionality is given in README-test-output.txt.

You can run a test program directly. For example, to run the integer tests, do
the following at the command line (assuming your current directory is
csc2025-assignment1-dist):

    # cd bin
    # ./test_integer
    
    or 
    
    # bin/test_integer
    
This will produce a usage message that tells you how to run individual tests, 
or all tests, and how to turn on the object store during tests. For example,

    # bin/test_integer -a

will run all tests. 

    # bin/test_integer 0

will run test 0 (the test of newInteger).

If you run all tests then you will find the program aborts because of the
parts of integer.c (or string_o.c or obj_store.c) that you have not
implemented.

Therefore, YOU MAY FIND IT MORE CONVENIENT to run the relevant test script in
csc2025-assignment1-dist. There is a test script for each suite of tests that
will run each individual test in a separate process. This means that the
failure of individual tests will not prevent the execution of other tests. To
do this for tests of integer.c, at the command line in directory
csc2025-assignment1-dist type:

    # ./integer_tests.sh
    
There is a comment in integer_tests.sh that explains what it does.

For a unit test that completes successfully you will see the relevant output
shown in README-test-output.txt. For a test that fails you will see lines
of output similar to:

---------------------------------------------------------------
Running test 6: test_subtract_norm ...
[case: 001, line: 265] 0 - 0, actual: NULL, expected: 0
assertion "act && act->get_value(act) == exp" failed: file "test_src/test_integer.c", line 588, function "assert_success"
abort (core dumped)
---------------------------------------------------------------

This tells you that the normal test of the subtract function failed
(test_subtract_norm) at test case 1 and that the error was detected at line
265 in the test_src/test_integer.c. It also tells you the parameters to the 
subtract function that caused the failure (0, 0), the actual result of the 
call to subtract (NULL) and the expected result (0).

You can work out why the test failed by looking at line 265 of
test_src/test_integer.c and the lines preceding it. This will give you some
idea of what the error is. When you first run the tests this will be
because you have not implemented subtract yet.

The way to work is:
    - try to implement a function,
    - compile test programs
    - run a test program using the relevant test script
    - work out the cause of any test failures
    - repeat until there are no failures for either normal or error cases

In addition to the test programs, the applications integer_app and string_app
provide example usage of the functionality in integer.c and string_o.c. To run
the programs, do the following at the command line (assuming your current
directory is csc2025-assignment1-dist):

    # cd bin
    # ./integer_app
    
    - to run the integer application, or:
    
    # ./string_app
    
    - to run the string usage application.

You will have to look at the source code for each application in test_src to
work out what the applications are doing and to see usage of the functionality
in integer.c and string_o.c, respectively.

------------------------------------------------------------------------------
3. GENERAL APPROACH TO IMPLEMENTATION

The suggested general approach to implementing your solution is:

1 Implement one function at a time.
2 Read the specification of the function in the relevant header file
  (integer.h, string_o.h or obj_store.h). Read comments too the function in 
  the relevant .c file (integer.c, string_o.c or obj_store.c). Read hints and  
  rules in this file and in the coursework specification.
3 Try to implement normal functionality first and test it using the relevant
  test program. If it does not pass normal case tests, look at the test code 
  for indications of the expected behaviour of the function. Refine your
  normal behaviour implementation until the function passes normal case tests.
4 Work out the error conditions (e.g. integer overflow in the case of
  Integer), how to deal with to them, and what validity checks to do on
  parameters to the function.
5 Implement error checking and test it with the relevant test program. Make
  sure that any changes you have made do NOT cause failure of normal case 
  tests.
6 Repeat steps 2 to 5 until your function passes both normal and error
  case tests.
7 Move on to another function.

You have to recompile/make the project before running tests.

------------------------------------------------------------------------------
4. INTEGER IMPLEMENTATION HINTS

Your task is to implement _subtract, _divide and _modulo functions in
integer.c. These functions are annotated with a TODO comment. Do NOT change
any other code in integer.c. Do NOT change any code in the integer test
program test_src/test_integer.c. You can and should look at test_integer.c 
for help with implementation of the functions in integer.c.

Implement the functions so that they check for invalid parameters and detect
positive or negative integer overflow as specified in integer.h. That is, the
functions must comply with the specification of their normal and error case
behaviour defined in integer.h.
  
Hints: 
- Use implemented functions such as _add and _multiply as a model.
- _divide and _modulo are simpler than _subtract
- It is not possible to check whether an operation overflows directly. For
  example, you cannot check for overflow of addition by doing the addition 
  and checking the result against INT_MAX (e.g. x + y <= INT_MAX) but you can
  use that invariant to derive appropriate conditions.
- Make sure you understand the specification of functions in integer.h, 
  including error conditions.
- For each function you are implementing, look at the relevant unit tests in 
  test_src/test_integer.c source code to help work out the expected behaviour
  of a function.

------------------------------------------------------------------------------
5. STRING IMPLEMENTATION HINTS AND ADDITIONAL RULE

Your task is to implement newString, deleteString, _add, _char_at, _equals,
_get_value and _index_of functions in string_o.c. These functions are
annotated with a TODO comment. Do NOT change any other code in string_o.c. Do
NOT change any code in the string test program test_src/test_string.c. You can
and should look at test_string.c for help with implementation of the functions
in string_o.c.

Implement the functions so that they check for invalid parameters, including
operations that would be out of specified length bonds. You must, for example,
guarantee that the length of the internal C-string of the string struct is
never greater than STR_LEN_MAX. That is, the functions must comply with the
specification of their normal and error case behaviour defined in string_o.h.
 
Hints:

- Implement newString first. None of the other functions can work until you
  have implemented newString. newInteger is a good model for newString. 
  There is some additional work you have to do but the implementation is
  similar. There are guidance notes in the body of the function to help you.
- Be careful to pay attention to where the specification or comments state
  that something *new* should be assigned, returned or allocated
- Free any temporary dynamic memory allocations you make and free allocations
  as appropriate when there are failures.
- Make sure deleteString frees the memory allocated in newString.
- For each function you are implementing, look at the relevant unit tests in
  test_src/test_string.c source code to help work out the expected behaviour 
  of a function. There may also be code that you can copy or adapt from a unit
  test to form your solution. This is allowed and does not count as plagiarism.
- You cannot know the length of any C-string (char*) parameter to a function.
  The parameter could be NULL or it could have a length from 0 up to an 
  arbitrarily large number. It may not even be terminated correctly. For this  
  reason, the standard library usually provides at least two versions of each
  function in string.h (e.g strlen and strnlen both determine the length 
  of a C-string). You must decide which is safest to use.
- READ THE MAN PAGE for any system library function you use.

ADDITIONAL RULE

For C-string processing (determining length, copying, concatenating etc) you
must only use functions in the str* family declared in string.h. That is, you
can only use functions that begin "str" (taking account of the hint about
string length).

Where necessary, you can use malloc (or some other appropriate function) to
allocate memory for a C-string. You have to determine when it is necessary to
dynamically allocate the memory for a C-string and, if it is necessary, you
have to decide whether to use malloc or some other library function that does
the dynamic allocation.
    
------------------------------------------------------------------------------
6. OBJECT STORE IMPLEMENTATION HINTS AND ADDITIONAL RULE

Your task is to implement store_obj in obj_store.c. The function is annotated
with a TODO comment. Do NOT change any other code in obj_store.c. Do NOT
change any code in the obj_store test program test_src/obj_store.c. You can
and should look at obj_store.c for help with implementation of store_obj.
There may also be code that you can copy or adapt from a unit test (or from
test helper functions) to form your solution. This is allowed and does not
count as plagiarism.

There are comments in the body of the function in obj_store.c to help you.

READ THE MAN PAGE for any system library function you use.
 
ADDITIONAL RULE

For filesystem operations in store_obj you must only use the following functions: 

    open
    close
    write
    unlink
    
You must NOT use any other filesystem or file stream library functions. 

------------------------------------------------------------------------------
