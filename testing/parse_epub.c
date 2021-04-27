#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "colors.h"

#define ERROR   B_RED "ERROR" RESET ": "	// Default error message starter.
#define BUFLENGTH 256

int getMimetype( char * tld_path, char * mimetype, int length );
int getNavPoints( char * tld_path, char ** result );
char * skipUntil( char * str, char * delims, int num_delims, int remove_delim );
char * skipChars( char * str, char * delims, int num_delims );
char captureUntil( char ** str, char ** result, char * delims, int num_delims, int include );

int main( int argc, char ** argv )
{
	/* Define the paths for the epub book directory. */
	// List of books used to test.
	char * books[] = { "eagle_and_empire", "the_blade_itself" };
	
	// Select the book we want to use.
	int book_selection = 0;

	// Create a string to hold the path to the top level directory for the book.
	// Ensure a trailing forward slash is added for safety.
	// Format is "./books/" + book_name. Example: "./books/eagle_and_empire/"
	char tld_path[ 128 ];
	strcpy( tld_path, "./books/" );
	strcat( tld_path, books[ book_selection ] );
	strcat( tld_path, "/" );
	printf( "Book: %s\nPath: %s\n\n", books[ book_selection ], tld_path );

	int mimetype_length_max = 128;
	char * mimetype = (char*) malloc( mimetype_length_max );
	if( getMimetype( tld_path, mimetype, mimetype_length_max ) )
	{
		printf( ERROR "getMimetype failed.\n" );
		return 0;
	}

	printf( "Mimetype: %s\n\n", mimetype );


	char * nav_list;
	if( getNavPoints( tld_path, &nav_list ) )
	{
		printf( ERROR "getNavPoints failed.\n" );
		return 0;
	}

	printf( "List of Chapters: %s\n", nav_list );

	free( mimetype );
	free( nav_list );
	return 0;
}


/****
	*	Take the top level directory path and parse the epub directory
	*	to get the mimetype as located in the 'mimetype' file. The file is
	*	defined such that the only content in it should be the mimetype, so
	*	we only need to open the file and return the first line in it.
	*
	*	@param tld_path			Path to top level directory of the epub book.
	*	@param mimetype 		Pointer to location to store mimetype value. 
	*							Memory must already be allocated.
	*	@param length 			Length of the mimetype char array.
	*
	*	@return					Result of mimetype access. 0: success, 1: failure.
****/
int getMimetype( char * tld_path, char * mimetype, int length )
{
	// Create char array to hold path to the mimetype file.
	// Length of 'mimetype' is 8.
	char * path_to_mimetype = (char*) malloc( strlen( tld_path ) + 8 + 1 );
	strcpy( path_to_mimetype, tld_path );
	strcat( path_to_mimetype, "mimetype" );

	FILE * mimetype_file;
	if( ( mimetype_file = fopen( path_to_mimetype, "r" ) ) == NULL )
	{
		printf( ERROR "Unable to open mimetype file.\n" );
		free( path_to_mimetype );
		return 1;
	}

	if( !fgets( mimetype, length, mimetype_file ) )
	{
		printf( ERROR "Unable to read from mimetype_file.\n" );
		free( path_to_mimetype );
		fclose( mimetype_file );
		return 1;
	}

	free( path_to_mimetype );
	fclose( mimetype_file );

	return 0;
}


/****
	*	Parse the epub directory to gather the list of chapters/sections in the
	*	book. The 'META-INF/container.xml' file will contain a 'rootfile' tag
	*	that contains the path to the .opf file. This .opf file (Open Packaging
	*	Format) is defined by the standards at: 
	*				http://idpf.org/epub/20/spec/OPF_2.0.1_draft.htm
	*	The standards defined in section 2.0 are used to parse the .opf file to
	*	gather the navigation points in the book.
	*
	*	@param tld_path 		Path to top level directory of the epub book.
	*	@param result 			Pointer to a char array to hold the resulting
	*							user-readable navigation point string.
	*
	*	@return 				Success status. 0: success, 1: failure.
****/
int getNavPoints( char * tld_path, char ** result )
{
	*result = (char*) malloc( 10 );
	strcpy( *result, "nav_list" );

	// Get the path to the contents.xml file and open it.
	// strlen( "META-INF/container.xml" ) = 22
	char * container_path = (char*) malloc( strlen( tld_path ) + 23 );
	strcpy( container_path, tld_path );
	strcat( container_path, "META-INF/container.xml" );

	FILE * container_file;
	if( ( container_file = fopen( container_path, "r" ) ) == NULL )
	{
		printf( ERROR "Unable to open container file.\n" );
		free( container_path );
		return 1;
	}


	// Define flags to help with parsing.
	/*int in_tag = 0;
	int in_string = 0;
	char quote = '"';
	char l_bracket = '<';
	char r_bracket = '>';*/


	// Iterate through the file and print out each character.
	// Process contents.
	char buf[ BUFLENGTH ];
	fgets( buf, BUFLENGTH, container_file );
	printf( "Original: '%s'\n", buf );

	// Remove characters until open bracket indicating tag.
	char * token = strtok( buf, "<" );
	// printf( "Remove up to first bracket:\n'%s'\n\n", token );

	// Remove all spaces/newlines until next character.
	token = skipChars( token, " \n", 2 );
	// printf( "Remove spaces:\n'%s'\n\n", token );
	if( token[ 0 ] == 0 )
	{
		printf( ERROR "String thrown out.\n" );
		return 0;
	}


	// Get first tag name.
	char * tag_name = NULL;
	char found_delim = captureUntil( &token, &tag_name, " >", 2, 0 );
	printf( "Tag '%s':\n", tag_name );

	while( 1 )
	{
		/*
		// Capture tag name by capturing string until next space.
		// Also check if we run into a close bracket to see if 
		// it's only a tag name.
		found_delim = captureUntil( &token, &tag_name, " >", 2, 0 );
		if( found_delim == '>' )
			printf( "No attributes.\n" );
		printf( "Tag '%s':\n", tag_name );
		// printf( "Token:\n'%s'\n\n", token );
		*/

		// Remove all spaces/newlines until next character.
		token = skipChars( token, " \n", 2 );
		// printf( "Remove spaces:\n'%s'\n\n", token );

		// Get attribute name.
		char * attribute_name = NULL;
		char * attribute_value = NULL;
		found_delim = captureUntil( &token, &attribute_name, " =?>", 4, 0 );

		// Capture attributes and values until tag header closes with '>'.
		while( found_delim != '>' && found_delim != '?' )
		{
			printf( "    '%s':", attribute_name );
			// printf( "Token:\n'%s'\n\n", token );

			// Remove all spaces/newlines until next character.
			token = skipChars( token, " \n", 2 );
			// printf( "Remove spaces:\n'%s'\n\n", token );


			// Remove all characters until a '=' is found if the found_delimiter
			// was a space.
			if( found_delim == ' ' )
				token = skipUntil( token, "=", 1, 0 );
			// printf( "Skip until '=':\n'%s'\n\n", token );

			if( token[ 0 ] == 0 )
			{
				printf( "\n" ERROR "Missing '='.\n" );
				break;
			}

			// Remove all characters until the '"' is found denoting the start
			// of an attribute value.
			token = skipUntil( token, "\"", 1, 1 );
			// printf( "Skip until '\"':\n'%s'\n\n", token );

			// Get attribute value.
			found_delim = captureUntil( &token, &attribute_value, "\"", 1, 0 );
			printf( "'%s'\n", attribute_value );
			// printf( "Token:\n'%s'\n\n", token );

			// Remove all spaces/newlines until next character.
			token = skipChars( token, " \n", 2 );
			// printf( "Remove spaces:\n'%s'\n\n", token );

			// Get attribute name.
			found_delim = captureUntil( &token, &attribute_name, " =?>", 4, 0 );

			if( token[ 0 ] == 0 )
			{
				printf( ERROR "Malformed tag.\n" );
				break;
			}

			/* TEMP */
			// break;
		}

		printf( "Remaining string: '%s'\n", token );
		if( found_delim == '?' && token[ 0 ] != '>' )
		{
			printf( ERROR "Invalid format.\n" );
		}

		printf( "Tag closed.\n" );
		free( attribute_name );
		free( attribute_value );


		found_delim = captureUntil( &token, &tag_name, ">", 1, 0 );

		if( found_delim != '>' )
			printf( "Tag '%s':\n", tag_name );

		// Remove all spaces/newlines until next character.
		token = skipChars( token, " \n", 2 );
		// printf( "Remove spaces:\n'%s'\n\n", token );
		if( token[ 0 ] == 0 )
		{
			// printf( ERROR "String thrown out.\n" );

			if( fgets( buf, BUFLENGTH, container_file ) == NULL )
			{
				printf( "\n\nEnd of File.\n\n" );
				break;
			}
			printf( "\n\nNew String: '%s'\n", buf );
			found_delim = captureUntil( &token, &tag_name, " >", 2, 0 );
		}

		printf( "Tag '%s':\n", tag_name );

		token = strtok( buf, "<" );
	}


	free( tag_name );

	/*
		while( fgets( buf, 2, container_file ) != NULL )
		{
			if( buf[ 0 ] == quote )
			{
				in_string = !in_string;
				// printf( "%s STRING\n", in_string ? "START" : "END" );
				printf( "%s", buf );
			}
			else if( buf[ 0 ] == l_bracket && !in_string )
			{
				in_tag = 1;
				// printf( "START TAG\n" );
				printf( "\n%s", buf );
			}
			else if( buf[ 0 ] == r_bracket && !in_string )
			{
				in_tag = 0;
				// printf( "END TAG\n" );
				printf( "%s\n", buf );
			}
			else
			{
				printf( "%s", buf );
			}

			// printf( "%s%s", buf, !in_tag ? "\n" : "" );
		}
	*/

	printf( "\n\n" );
	fclose( container_file );
	free( container_path );

	return 0;
}


/****
	*	Take a char array and skip through until one of the delimiters is found.
	*	Iterate through each character until the null character. At each step,
	*	check if any of the delimiters matches the current character. If so,
	*	break out. If not, increment the pointer.
	*
	*	@param str 				Char array to skip through.
	*	@param delims 			Char array of delimiter characters to check.
	*	@param num_delims		Number of delimiters in delims array.
	*	@param remove_delim 	Flag to remove delim from string (1) or not (0).
	*
	*	@return 				Pointer to next non-space character.
****/
char * skipUntil( char * str, char * delims, int num_delims, int remove_delim )
{
	int i;
	while( str[ 0 ] != 0 )
	{
		// printf( "Check char: '%c'\n", str[ 0 ] );
		for( i = 0; i < num_delims; i++ )
		{
			// printf( "	Match '%c': %s\n", delims[ i ], str[ 0 ] == delims[ i ] ? "TRUE" : "FALSE" );
			if( str[ 0 ] == delims[ i ] )
			{
				if( remove_delim )
					str++;

				return str;
			}
		}

		str++;
	}

	return str;
}


/****
	*	Take a char array and skip through until none of the delimiters are
	*	found. Iterate through each character until the null character. 
	*	At each step, check if any of the delimiters matches the current 
	*	character. If so, increment the pointer and continue. If not, 
	*	return the trimmed string.
	*
	*	@param str 				Char array to skip through.
	*	@param delims 			Char array of delimiter characters to check.
	*	@param num_delims		Number of delimiters in delims array.
	*
	*	@return 				Pointer to next non-space character.
****/
char * skipChars( char * str, char * delims, int num_delims )
{
	int i;
	while( str[ 0 ] != 0 )
	{
		for( i = 0; i < num_delims; i++ )
		{
			if( str[ 0 ] == delims[ i ] )
			{
				str++;
				i = num_delims + 1;
				break;
			}
		}

		// If no delimiters are found, i = num_delims.
		// If one is found, i = num_delims + 1.
		if( i == num_delims )
		{
			return str;
		}
	}

	return str;
}


/****
	*	Capture part of a string until one of the delimiter characters.
	*	Source string is trimmed until after the first found elimiter character.
	*
	*	@param str 				Pointer to source to capturefrom and trim.
	*	@param result 			Pointer to location to store captured string.
	*							Value expected to either be malloc'd previously
	*							or NULL.
	*	@param delims 			Char array of delimiter characters to check.
	*	@param num_delims		Number of delimiters in delims array.
	*	@param include			Flag to include the matched delimiter. 1 = TRUE.
	*
	*	@return 				The matched delimiter character. 0 = None found.
****/
char captureUntil( char ** str, char ** result, char * delims, int num_delims, int include )
{
	int length = 0;
	int i;
	while( (*str)[ length ] != '\0' )
	{
		for( i = 0; i < num_delims; i++ )
		{
			if( (*str)[ length ] == delims[ i ] )
			{
				length++;

				if( include )
					length++;

				*result = (char*) realloc( *result, length );
				memcpy( *result, *str, length - 1 );
				(*result)[ length - 1 ] = 0;

				if( include )
					*str += length - 1;
				else
					*str += length;
				return delims[ i ];
			}
		}

		length++;
	}

	return 0;
}


/****
	*
	*
	*
****/
int readAndAppend( FILE * f, char * dest, char * src, int buf_length )
{

}
