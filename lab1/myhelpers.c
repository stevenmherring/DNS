#define NULL ((char *)0)

int strlen(const char *str);
int strcmp(const char *str1, const char *str2);
void my_strcpy(char* str1, char* str2);
void str_memset(char* str, int c, int n);
double my_atof(char *str);


/*
-----------------------------------------------------------------------------
Find string length using a char array passed to the function
-----------------------------------------------------------------------------
*/
int strlen(const char *str) 
{
	if(str != NULL) {
		const char *temp;
		for(temp = str; *temp; ++temp);
			return(temp - str);
	} else {
		return 0;
	}
}

/*
-----------------------------------------------------------------------------
Compare two strings, return 1 if they are equivalent and 0 if not.
-----------------------------------------------------------------------------
*/
int strcmp(const char *str1, const char *str2)
{
	if(str1 != NULL && str2 != NULL) {
		const char *s1, *s2;
		s1 = str1;
		s2 = str2;
	/*
	Check the strings character by character if their length is the same
	use a while loop, while the current character is equal
	if either hit NULL '\0' then break out of the loop
	*/
		while(*s1 == *s2)
		{
			if(*s1 == '\0' || *s2 == '\0')
				break;
			s1++;
			s2++;
		}
	/*
	After a string breaks the while loop, confirm if the other string is also at a null char.
	If they are both a '\0' return 1, or else return 0
	*/
	if(*s1 == '\0' && *s2 == '\0')
		return 1;
	else
		return 0;
} else {
	return 0;
}
}

/*
-----------------------------------------------------------------------------
Copy a string to a new location
-----------------------------------------------------------------------------
*/
// char* strncpy(char *dst, const char *src, int n)
// {
// 	int i = 0;
// 	int j = hw_strlen(src);
// 	dst = malloc(n * sizeof(char));
// 	if(src == NULL) {
// 		return dst;
// 	}
// 	if(j < n) {
// 		while(i < j){
// 			*dst++ = *src++;
// 			i++;
// 		}
// 		while(i < n){
// 			*dst++ = '\0';
// 			i++;
// 		}
// 		return dst - i;
// 	} else {
// 		while(i < n) {
// 			*dst++ = *src++;
// 			i++;
// 		}
// 		return dst - i;
// 	}

// 	return dst;
// }

//Cant use malloc so I have to do something different.
void my_strcpy(char* str1, char* str2) {
	int n = 0; //counter to traverse char*
	while(str1[n] != '\0') {
		str2[n] = str1[n]; //set current char to new string location
		n++; //inc
	} //while
	str2[n] = '\0'; //null terminate
}

/*
-----------------------------------------------------------------------------
Memset, replace N number of characters in a memory location
-----------------------------------------------------------------------------
*/
void str_memset(char* str, int c, int n) {
	int i;
	//first make sure n is less than length of mem location
	//to prevent seg fault
	if(n > strlen(str)) { }
	else { 
		for(i = strlen(str); i > -1; i--) { str[i] = c; }
	}
}

/*
-----------------------------------------------------------------------------
ATOF - convert char* to int
-----------------------------------------------------------------------------
*/
double my_atof(char *str) {
	double ret = 0;
	int e = 0;
	int c;
	int i = 0;
	int sign = 1;
	while ((c = *str++) != '\0' && c < 57 && c >= 48 ) {
	        ret = ret * 10.0 + (c - '0');
	}
	if (c == '.') {
	        while ((c = *str++) != '\0' && c < 57 && c >= 48) {
	                ret = ret * 10.0 + (c - '0');
	                e = e-1;
	        }
	}
	if (c == 'e' || c == 'E') {
	        c = *str++;
	        if (c == '+')
	                c = *str++;
	        else if (c == '-') {
	                c = *str++;
	                sign = -1;
	        }
	        while (c < 57 && c >= 48) {
	                i = i * 10.0 + (c - '0');
	                c = *str++;
	        }
	        e += i*sign;
	}
	while (e > 0) {
	        ret *= 10.0;
	        e--;
	}
	while (e < 0) {
	        ret *= 0.1;
	        e++;
	}
	return ret;
}
