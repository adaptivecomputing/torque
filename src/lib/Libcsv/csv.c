/* Copyright 2008 Cluster Resources */

/* A small library of functions dealing with comma separated variable strings.
 * This library is designed to work with small lists of 10 or so items.
 * It does not scale to larger lists which would require some kind of pointer
 * array to the list items with the associated parsing and construction of
 * this structure, etc.
 */

#include <string.h>

/**
 * Gets the number of items in a string list.
 * @param str  The string list.
 * @return The number of items in the list.
 */
int csv_length( char *str )
{
	int		length = 0;
	char	*cp;

	if (!str || *str == 0)
		return(0);

	length++;
	cp = str;
	while ((cp = strchr(cp, ',')))
	{
		cp++;
		length++;
	}
	return(length);
}

/**
 * Gets the nth item from a comma seperated list of names.
 * @param str  The string list.
 * @param n The item number requested (0 is the first item).
 * @return Null if str is null or empty,
 *     otherwise, a pointer to a local buffer containing the nth item.
 */
char *csv_nth( char *str, int n )
{
	int		i;
	char	*cp;
	char	*tp;
static	char	buffer[128];

	if (!str || *str == 0)
		return(0);

	cp = str;
	for (i = 0; i < n; i++)
	{
		if (!(cp = strchr(cp, ',')))
		{
			return(0);
		}
		cp++;
	}
	memset(buffer, 0, sizeof(buffer));
	if ((tp = strchr(cp, ',')))
	{
		strncpy(buffer, cp, tp-cp);
	}
	else
	{
		strcpy(buffer, cp);
	}
	return(buffer);
}



