#ifndef _STRING_O_H
#define _STRING_O_H
#include <stdbool.h>

/* 
 * The maximum length of a string. Newly allocated strings are truncated to 
 * this length. The maximum memory allocation for a string will therefore be
 * STR_LEN_MAX + 1
 */
#define STR_LEN_MAX 1023

/*
 * Type definition:
 * String
 * 
 * Description:
 * Declares String to be an alias for the type: "pointer to a struct string".
 * That is, String can be used as a type name in place of "struct string*".
 * (See below for definition of struct string).
 */
typedef struct string* String;

/*
 * Function:
 * newString(char* value)
 * 
 * Description:
 * Dynamically allocate a new struct string with a new internal 
 * char* value that represents the same string as the value provide as a 
 * parameter and return a pointer (String) to the allocated struct.  
 * newString dynamically allocates new memory for the internal char*. The
 * internal value will have a maximum string length of STR_LEN_MAX. 
 * This means that the internal value may be a truncation of 
 * the parameter valuepassed to newString.
 * If the object store is enabled the new string will be saved to persistent
 * storage as part of successful allocation.
 * It is the user's responsibility to subsequently free the allocated
 * memory using deleteString.
 *
 * Usage: 
 *      String i = newString("hello");
 *
 * Parameters:
 * value - the char* value of the struct string to allocate. 
 *      Note: if the length of parameter value is greater than STR_LEN_MAX 
 *      the internal value of the struct string will be a truncation to
 *      length STR_LEN_MAX.
 *
 * Return:
 * On success: a new non-null, pointer to a dynamically allocated string  
 *      struct (or String) whose internal dynamically allocated value 
 *      is equivalent to the value given (or its truncation to STR_LEN_MAX)
 * On failure: NULL, and errno will be set to EINVAL or ENOMEM or to a value 
 *      set by storing objects if the object store is enabled and storage fails
 *      (see store_obj in obj_store.h)
 *
 * Errors:
 * If the call fails, the NULL pointer will be returned and errno will be 
 * set as follows.
 *      EINAL -  invalid argument: if value is NULL
 *      ENOMEM - not enough space: if dynamic allocation fails
 *      Other errno values set by store_obj defined in obj_store.h
 */
String newString(char* value);

/*
 * Function:
 * deleteString(String* as)
 * 
 * Description:
 * Delete a struct string previously allocated by the newString function.
 * deleteString also frees memory allocated for the internal char* value.
 * If the object store is enabled the string will be deleted from persistent
 * storage as part of successful deletion.
 *
 * Usage: 
 *      String s = newString("hello");
 *      ...
 *      ...
 *      deleteString(&s);
 *
 * Parameters:
 * as - the address of a String pointer
 *
 * Return:
 * Not applicable
 *
 * Errors:
 * Not applicable
 */
void deleteString(String* as);

/*
 * Type definition:
 * struct string - a string whose "member" functions ensure operations on its
 * internal char* are within string length bounds.
 * 
 * Description:
 * The definition of a string type with "member functions":
 *      add
 *      char_at
 *      equals
 *      get_value
 *      index_of
 *      length
 * 
 * The "member functions" are pointers to functions that are 
 * defined in string_o.c.
 * The pointer assignments are made by the newString function.
 * 
 * String (defined above) is another name for a pointer to a struct string.
 */
struct string {
    /*
     * Member field:
     * _len
     * 
     * Description:
     * The length of the internal string value. Users should not access
     * this member field directly. It is set during creation of the string 
     * struct. The length can be obtained using the length function.
     */
    int _len;
    
    /*
     * Member field:
     * _val
     * 
     * Description:
     * The internal char* value of the string struct. Users should not access
     * this member field directly. To guarantee correct usage within length
     * bounds, the value should only be manipulated by member functions and 
     * only obtained using the get_value function.
     * The internal char* value is dynamically allocated by newString and 
     * freed by deleteString.
     */
    char* _val;
    
    /*
     * Pointer to function member field:
     * add(String self, String s)
     * 
     * Description:
     * Add or concatenate two strings together and return a newly allocated 
     * string that is the result of the concatenation. If the function
     * succeeds, a new string struct is always allocated even if its internal
     * value will be logically equal to one of the arguments to the 
     * add function. As with newString, it is the responsibility of the user 
     * to subsequently free memory allocated for the returned String 
     * by using deleteString.
     * 
     * Usage: 
     *      String s1 = newString("hello");
     *      String s2 = newString(" again");
     *      String s3 = s1->add(s1, s2);    // s3 value will be "hello again"           
     *                                      // assumes s1 and s2 not null
     * Parameters:
     * self - the non-null String on which add is called (e.g. s1 in above
     *      example). 
     * s - the non-null String to add to/concatenate with self
     *
     * Return:
     * On success: a new non-null String whose value is the concatenation
     *      of the values of self and s 
     * On failure: NULL, and errno will be set to EINVAL
     *
     * Errors:
     * If the call fails, the NULL pointer will be returned and errno will be 
     * set to:
     *      EINVAL - invalid argument: if either self or s are NULL
     *      ENOMEM - not enough space: if dynamic allocation of the 
     *          concatenation fails (e.g. for the same reason as newString)
     *      Other errno values to do with object storage of new objects as
     *          indicated in comments to newString.
     */
    String (*add)(String self, String s);

    /*
     * Pointer to function member field:
     * char_at(String self, int posn)
     * 
     * Description:
     * Return the character in the given string at the given position or 0 for 
     * the empty string.
     * 
     * Usage: 
     *      String s = newString("hello");
     *      char c = s.char_at(s, 1);       // c will be 'e', assume s not null
     *
     * Parameters:
     * self - the non-null String on which char_at is called (e.g. s in above
     *      example). 
     * posn - the posn in the string to select a char. Valid string positions
     *      are in the range 0 to (length - 1) except for the case of the 
     *      empty string when 0 is a valid position.
     *
     * Return:
     * On success: the character at the given position in the string or 0 for 
     *      the empty string
     * On failure: 0, and errno will be set to EINVAL
     *
     * Errors:
     * If the call fails, 0 will be returned and errno will be set to:
     *      EINVAL - invalid argument: if self is NULL or posn is outside  
     *          the valid range for the string.
     */
    char (*char_at)(String self, int posn);

    /*
     * Pointer to function member field:
     * equals(String self, String s)
     * 
     * Description:
     * Compare two non-null strings for logical equality and return true if 
     * they are equal and false otherwise.
     * Two strings are equal if they have the same length and their internal 
     * arrays of characters are the same.
     * If either or both parameters is NULL, the function will return false.
     * Note: two pointers to strings are trivially equal if they are identical
     * (except when both are NULL).
     * 
     * Usage: 
     *      String s1 = newString("hello");     
     *      String s2 = newString("hello");
     *      String s3 = newString("goodbye");
     *      s1.equals(s1, s1);  // true: identical
     *      s1.equals(s1, s2);  // true: not identical but logically equal
     *      s1.equals(s1, s3);  // false: not identical and not logically equal
     *
     * Parameters:
     * self - the non-null String on which equals is called (e.g. s1 in above
     *      examples). 
     * s - the non-null string to compare with self
     *
     * Return:
     * True if self and s are both non null and logically equal, false
     * otherwise.
     *
     * Errors:
     * Not applicable
     */
    bool (*equals)(String self, String s);

    /*
     * Pointer to function member field:
     * get_value(String self, char* buf)
     * 
     * Description:
     * Copy the value of a string to the given buffer and return a pointer to 
     * the buffer. The buffer will be null-terminated (e.g. will form a valid
     * C string). It is the user's responsibility to ensure that the   
     * given buffer is large enough to hold the full length of the string plus
     * the null terminator. The function cannot check the size of the user
     * provided buffer.
     * 
     * Usage: 
     *      String s = newString("hello");      // assume s is not null
     *      char buf[s->length(s) + 1];     
     *      char *copy = s->get_value(s, buf);  // copy and buf are identical
     *
     * Parameters:
     * self - the non-null String on which get_value is called (e.g. s in above
     *      example). 
     * buf - a character buffer large enough to hold the value array of the
     *      string.
     *
     * Return:
     * On success: a pointer to the buffer containing the copy of the internal
     * char*
     * On failure: NULL, and errno will be set to EINVAL
     *
     * Errors:
     * If the call fails, the NULL pointer will be returned and errno will be 
     * set to:
     *      EINVAL - invalid argument: if either self or buf are NULL
     */
    char* (*get_value)(String self, char* buf);
    
    /*
     * Pointer to function member field:
     * index_of(String self, char c, int start)
     * 
     * Description:
     * Return the index position of the first occurrence of the given character
     * in the given string, starting from position start. If the character is
     * not in the string or occurs before the starting position, -1 is 
     * returned.
     * 
     * Usage: 
     *      String s = newString("hello");      // assume s is not null
     *      int l1 = s->index_of(s, 'l', 0);    // l1 will be 2
     *      int l2 = s->index_of(s, 'l', 3):    // l2 will be 3     
     *
     * Parameters:
     * self - the non-null String on which index_of is called (e.g. s in above
     *      example). 
     * c - the character to find in the string
     * start - the position in the string to start the search from
     *
     * Return:
     * On success: the index of the first occurrence of character c, or 
     *      -1 if the character is not in the string or is before the given 
     *      start position
     * On failure: -1 and errno will be set to EINVAL
     *
     * Errors:
     * If the call fails, -1 will be returned and errno will be set to:
     *      EINVAL - invalid argument: if self is NULL or start is outside  
     *          the valid range for the string.
     */
    int (*index_of)(String self, char c, int start);

    /*
     * Pointer to function member field:
     * length(String self)
     * 
     * Description:
     * Return the length of the string.
     * 
     * Usage: 
     *      String s = newString("hello");      // assume s is not null
     *      int len = s->length(s);             // len will be 5
     *
     * Parameters:
     * self - the non-null String on which length is called (e.g. s in above
     *      example). 
     *
     * Return:
     * On success: the length of the string
     * On failure: -1 and errno will be set to EINVAL
     *
     * Errors:
     * If the call fails, -1 will be returned and errno will be set to:
     *      EINVAL - invalid argument: if self is NULL
     */
    int (*length)(String self);
};

#endif