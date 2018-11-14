#ifndef _OBJ_STORE_H
#define _OBJ_STORE_H
#include <stdbool.h>

/*
 * Function:
 * disable_ostore()
 * 
 * Description:
 * Disables storage of objects to a file-based object store and, if it exists,
 * removes the ostore directory and its contents.
 *
 * Usage: 
 *      disable_ostore();
 *
 * Parameters:
 * none
 *
 * Return:
 * Not applicable, after execution of disable_ostore, ostore_is_on will 
 * return false.
 *
 * Errors:
 * Not applicable, ostore_is_on will always return false after calling 
 * disable_ostore. Errors may result in old object files or the object store 
 * directory not being deleted but there will be no further writes to the 
 * ostore (assuming other functions are implemented correctly).
 */
void disable_ostore();

/*
 * Function:
 * enable_ostore()
 * 
 * Description:
 * Enables storage of objects to a file-based object store and, if necessary,
 * creates the directory to store the objects.
 *
 * Usage: 
 *      bool r = enable_ostore();
 *
 * Parameters:
 * none, the object store directory will be a sub-directory of the current 
 * directory called ostore.
 *
 * Return:
 * true if the ostore directory already exists or if it was successfully 
 * created, false otherwise, in which case errno will be set. 
 * If the result of this function is true, ostore_is_on will return true.
 *
 *
 * Errors:
 * If the call fails, the function returns false and ostore will not exist
 * and ostore_is_on will return false and errno will be set to a value that
 * indicates why ostore could not be created.
 */
bool enable_ostore();

/*
 * Function:
 * ostore_is_on()
 * 
 * Description:
 * Indicates whether the object store is on/enabled and, therefore, whether 
 * objects can be written to the ostore directory.
 *
 * Usage: 
 *      bool r = ostore_is_on();
 *
 * Parameters:
 * none
 *
 * Return:
 * true if the object store is enabled (i.e. the ostore directory has been 
 * created by a successful call to enable_ostore), 
 * false otherwise (i.e. enable_ostore has not been called or disable_ostore 
 * has been called).
 *
 * Errors:
 * Not applicable
 */
bool ostore_is_on();

/*
 * Function:
 * store_obj(void* obj, char* (*new_str_rep)(void*))
 * 
 * Description:
 * Writes the string representation of the given object to a text file whose
 * name is the hexadecimal value of the given pointer to the object plus the 
 * ".txt" extension. This function uses the new_str_rep pointer to a 
 * function to create a new string representation of the given object.
 *
 * Usage: 
 *      bool r = store_obj(oid, new_str_rep);
 *                      // see newInteger in integer.c and tests in
 *                      // test_obj_store.c for examples of usage 
 *
 * Parameters:
 * obj - the pointer to the object 
 * new_str_rep - a pointer to a function that dynamically allocates a new 
 *      string representation of the given object. The implementation of 
 *      the function pointed to by new_str_rep must return a new char* 
 *      representation of the object passed to it if the object is not NULL, 
 *      and NULL otherwise. Note, implementations of string representation
 *      functions are provided in integer.c and string_o.c.
 *
 * Return:
 * true if the object store is enabled and the object is stored successfully, 
 * false otherwise. 
 *
 * Errors:
 * If the call fails, false will be returned and errno will be 
 * set as follows.
 *      EINVAL - invalid argument: if either obj or new_str_rep is NULL or
 *          invocation of new_str_rep returns NULL
 *      ENOENT - no such entity: if the object store is not enabled or the
 *          ostore directory does not exist
 *      Other errno values related to I/O errors writing to file.
 */
bool store_obj(void* obj, char* (*new_str_rep)(void*));

/*
 * Function:
 * unlink_obj(void *obj)
 * 
 * Description:
 * Unlinks/deletes the string representation file of the given object from the 
 * object store.
 *
 * Usage: 
 *      unlink_obj(obj);
 *
 * Parameters:
 * none
 *
 * Return:
 * Not applicable
 *
 * Errors:
 * Not applicable. If obj is non-NULL and there is a file in ostore that
 * corresponds to the given object, the file will be deleted.
 */
void unlink_obj(void *obj);

#endif