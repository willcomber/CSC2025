#ifndef _INTEGER_H
#define _INTEGER_H
#include <stdbool.h>

/*
 * Type definition:
 * Integer
 * 
 * Description:
 * Declares Integer to be an alias for the type: "pointer to a struct integer".
 * That is, Integer can be used as a type name in place of "struct integer*".
 * (See below for definition of struct integer).
 */
typedef struct integer* Integer;

/*
 * Function:
 * newInteger(int value)
 * 
 * Description:
 * Dynamically allocate a new struct integer with the given value and return a
 * pointer (Integer) to the allocated struct.  
 * If the object store is enabled, the new integer will be saved to persistent
 * storage as part of successful allocation.
 * It is the user's responsibility to use deleteInteger to subsequently free
 * the allocated memory.
 *
 * Usage: 
 *      Integer i = newInteger(10);
 *
 * Parameters:
 * value - the int value of the new struct integer
 *
 * Return:
 * On success: a new non-null, pointer to a dynamically allocated struct 
 *      integer (or Integer) whose internal value is the given value 
 * On failure: NULL, and errno will be set to ENOMEM or to a value indicating
 *      failure to store the new struct integer if the object store is enabled  
 *      and storage fails (see store_obj in obj_store.h)
 *
 * Errors:
 * If the call fails, the NULL pointer will be returned and errno will be 
 * set as follows.
 *      ENOMEM - not enough space: if dynamic allocation fails
 *      Other errno values set by store_obj (see obj_store.h)
 */
Integer newInteger(int value);

/*
 * Function:
 * deleteInteger(int* ai)
 * 
 * Description:
 * Delete a struct integer previously allocated by the newInteger function.
 * If the object store is enabled the integer will be deleted from persistent
 * storage as part of successful deletion.
 * After deletion the Integer pointer to the struct will be NULL.
 *
 * Usage: 
 *      Integer i = newInteger(10);
 *      ...
 *      ...
 *      deleteInteger(&i);
 *
 * Parameters:
 * ai - the address of an Integer pointer
 *
 * Return:
 * Not applicable.
 * After deletion the Integer pointer, that ai is the address  of, will be 
 * NULL.
 *
 * Errors:
 * Not applicable
 */
void deleteInteger(Integer* ai);

/*
 * Function:
 * IntMax()
 * 
 * Description:
 * Return the Integer representing the largest positive value INT_MAX. 
 * If IntMax does not already exist, this function uses newInteger to
 * instantiate it. newInteger guarantees there is only ever one instance of 
 * IntMax.
 *
 * Usage: 
 *      Integer imax = IntMax();
 *
 * Return:
 * Not applicable
 *
 * Errors:
 * Not applicable
 */
Integer IntMax();

/*
 * Function:
 * IntMin()
 * 
 * Description:
 * Return the Integer representing the smallest negative value INT_MIN. 
 * If IntMin does not already exist, this function uses newInteger to
 * instantiate it. newInteger guarantees there is only ever one instance of 
 * IntMin.
 *
 * Usage: 
 *      Integer imin = IntMin();
 *
 * Return:
 * Not applicable
 *
 * Errors:
 * Not applicable
 */
Integer IntMin();

/*
 * Type definition:
 * struct integer - an integer whose functions detect and signal errors for 
 * positive or negative overflow of arithmetic operations.
 * 
 * Description:
 * The definition of an integer type with "member functions":
 *      add
 *      subtract
 *      multiply
 *      divide
 *      modulo
 *      get_value
 * 
 * The "member functions" are pointers to functions that are defined in 
 * integer.c
 * The pointer assignments are made by the newString function.
 * 
 * Integer (defined above) is another name for a pointer to a struct integer.
 */
struct integer {
    /*
     * Member field:
     * _val
     * 
     * Description:
     * The internal int value of the integer struct. Users should not access
     * this member field directly. To guarantee overflow detection etc., 
     * the value should only be manipulated by member functions and only 
     * obtained using the get_value member function.
     */
    int _val;
    
    /*
     * Pointer to function member field:
     * add(Integer self, Integer i)
     * 
     * Description:
     * Add the values of two Integers and return a new Integer that is the 
     * result.
     * 
     * Usage: 
     *      Integer r = lhs->add(lhs, rhs);
     *                                  // assume lhs and rhs have been created
     *                                  // with newInteger and are not null
     *
     * Parameters:
     * self - the non-null Integer on which add is called (e.g. lhs in above
     *      example). 
     * i - the non-null Integer to add to self
     *
     * Return:
     * On success: a new non-null Integer whose value is the sum of the values
     *      of self and i
     * On failure: NULL, and errno will be set to EINVAL or ERANGE
     *
     * Errors:
     * If the call fails, the NULL pointer will be returned and errno will be 
     * set as follows.
     *      EINVAL - invalid argument: if either self or i is NULL
     *      ERANGE - result too large: if the result of the operation 
     *          self->get_value(self) + i->get_value(i) would cause positive 
     *          or negative integer overflow
     */
    Integer (*add)(Integer self, Integer i);

    /*
     * Pointer to function member field:
     * subtract(Integer self, Integer i)
     * 
     * Description:
     * Subtract the value of one Integer from another and return a new Integer
     * that is the result.
     * 
     * Usage: 
     *      Integer r = lhs->subtract(lhs, rhs);
     *                                  // assume lhs and rhs have been created
     *                                  // with newInteger and are not null
     *
     * Parameters:
     * self - the non-null Integer on which subtract is called (e.g. lhs in
     *      above example). 
     * i - the non-null Integer to subtract from self
     *
     * Return:
     * On success: a new non-null Integer whose value is the subtraction of 
     *      the value of i from the value of self
     * On failure: NULL, and errno will be set to EINVAL or ERANGE
     *
     * Errors:
     * If the call fails, the NULL pointer will be returned and errno will be 
     * set as follows.
     *      EINVAL - invalid argument: if either self or i is NULL
     *      ERANGE - result too large: if the result of the operation 
     *          self->get_value(self) - i->get_value(i) would cause positive 
     *          or negative integer overflow
     */
    Integer (*subtract)(Integer self, Integer i);
    
    /*
     * Pointer to function member field:
     * multiply(Integer self, Integer i)
     * 
     * Description:
     * Multiply the values of two Integers and return a new Integer that is the 
     * result.
     * 
     * Usage: 
     *      Integer r = lhs->multiply(lhs, rhs);
     *                                  // assume lhs and rhs have been created
     *                                  // with newInteger and are not null
     *
     * Parameters:
     * self - the non-null Integer on which multiply is called (e.g. lhs in
     *      above example). 
     * i - the non-null Integer to multiply self with
     *
     * Return:
     * On success: a new non-null Integer whose value is the multiplication of 
     *      the value of self with the value of i
     * On failure: NULL, and errno will be set to EINVAL or ERANGE
     *
     * Errors:
     * If the call fails, the NULL pointer will be returned and errno will be 
     * set as follows.
     *      EINVAL - invalid argument: if either self or i is NULL
     *      ERANGE - result too large: if the result of the operation 
     *          self->get_value(self) * i->get_value(i) would cause positive 
     *          or negative integer overflow
     */
    Integer (*multiply)(Integer self, Integer i);

    /*
     * Pointer to function member field:
     * divide(Integer self, Integer i)
     * 
     * Description:
     * Divide the value of one Integer by another and return a new Integer that
     * is the result.
     * 
     * Usage: 
     *      Integer r = lhs->divide(lhs, rhs);
     *                                  // assume lhs and rhs have been created
     *                                  // with newInteger and are not null
     *
     * Parameters:
     * self - the non-null Integer on which divide is called (e.g. lhs in
     *      above example). 
     * i - the non-null Integer to divide self by
     *
     * Return:
     * On success: a new non-null Integer whose value is the integer division 
     *      of the value of self by the value of i
     * On failure: NULL, and errno will be set to EINVAL or ERANGE
     *
     * Errors:
     * If the call fails, the NULL pointer will be returned and errno will be 
     * set as follows.
     *      EINVAL - invalid argument: if either self or i is NULL
     *      ERANGE - result too large: if the result of the operation 
     *          self->get_value(self) / i->get_value(i) would cause positive 
     *          or negative integer overflow or division by zero. 
     */
    Integer (*divide)(Integer self, Integer i);
    
    /*
     * Pointer to function member field:
     * modulo(Integer self, Integer i)
     * 
     * Description:
     * Divide the value of one Integer by another and return a new Integer that
     * is the remainder after integer division (the modulus or result of the 
     * modulo operation)
     * 
     * Usage: 
     *      Integer r = lhs->modulo(lhs, rhs);
     *                                  // assume lhs and rhs have been created
     *                                  // with newInteger and are not null
     *
     * Parameters:
     * self - the non-null Integer on which modulo is called (e.g. lhs in
     *      above example). 
     * i - the non-null Integer to divide self by to compute the modulus
     *
     * Return:
     * On success: a new non-null Integer whose value is the modulus of 
     *      the value of self divided by the value of i
     * On failure: NULL, and errno will be set to EINVAL or ERANGE
     *
     * Errors:
     * If the call fails, the NULL pointer will be returned and errno will be 
     * set as follows.
     *      EINVAL - invalid argument: if either self or i is NULL
     *      ERANGE - result too large: if the result of the operation 
     *          self->get_value(self) % i->get_value(i) would cause positive 
     *          or negative integer overflow or division by zero. 
     */
    Integer (*modulo)(Integer self, Integer i);
    
    /*
     * Pointer to function member field:
     * get_value(Integer self)
     *
     * Description:
     * Get the primitive int value of the struct integer.
     * 
     * Usage: 
     *      int i = oi->get_value(oi);
     *                                  // assume oi has been created
     *                                  // with newInteger and is not null
     *
     * Parameters:
     * self - the non-null Integer on which get_value is called (e.g. oi in
     *      above example). 
     *
     * Return:
     * On success: the value of int member field of the integer struct or
     *      0 if self is NULL, and errno will be set to EINVAL to distinguish
     *      this with a valid value of 0
     *
     * Errors:
     * If self is NULL, 0 will be returned and errno will be set to EINVAL
     */
    int (*get_value)(Integer self);
};

#endif