/**
 * Calculates the length of a '\0' terminated string.
 * (the null character is not part of the length)
 * @param str Pointer to a '\0' terminated string.
 * @return Returns the length of the string. 
 */
int strlen(const char *str);

/**
 * Determines if two strings are exactly equal to each other.
 * @param str1 Pointer to a '\0' terminated string.
 * @param str2 Pointer to a '\0' terminated string.
 * @return Returns 1 if equal, else 0.
 */
int strcmp(const char *str1, const char *str2);

/**
 * Copies the string from src into dst.
 * @param dest Buffer to copy into.
 * @param src String to copy.
 * @param n Non-negative maximum about of bytes that can be copied from src.
 * @return Returns the dst pointer.
 */
//char* strncpy(char *dst, const char *src, int n);
void my_strcpy(char* str1, char* str2);

/**
 *Memset, replaces a requested number of characters in char with ASCII value input
 *@param n Number of characters to be replaced
 *@param src address
 *@param c int ASCII value for char replacement (i.e 10 for \0)
 **/
 void str_memset(char *str, int c, int n);

 /**
 *ATOF TYPE OF FUNCTION
 *Only converting string to int though
 **/
double my_atof(char *str);
