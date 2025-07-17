// FindAES version 1.1 by Jesse Kornblum
// http://jessekornblum.com/tools/findaes/
// This code is public domain.
//
// Revision History
//  3 Feb 2012 - Added entropy check. Limited to one file
// 18 Jan 2011 - Initial version

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "aes.h"

// Use a 10MB buffer
#define BUFFER_SIZE  10485760
#define WINDOW_SIZE  AES256_KEY_SCHEDULE_SIZE


void display_key(const unsigned char * key, uint64_t sz)
{
  uint64_t pos = 0;
  while (pos < sz)
  {
    printf("%02x ", key[pos]);
    ++pos;
  }
  printf("\n");
}

/// @brief Returns true if any byte in the block repeats more than eight times
int entropy(const unsigned char * buffer, size_t size)
{
  size_t i;
  static unsigned int count[256] = {0};
  static int first_entropy = 1;
  int result = 0;

  // We only need to compute the full frequency count the first time
  if (first_entropy)
  {
    first_entropy = 0;
    for (i = 0 ; i < size ; ++i)
      count[buffer[i]]++;
  }
  
  for (i = 0 ; i < 256 ; ++i)
  {
    if (count[i] > 8)
    {
      result = 1;
      break;
    }
  }

  // Shift the frequency counts
  count[buffer[0]]--;
  count[buffer[176]]++;

  return result;
}


void scan_buffer(unsigned char * buffer, uint64_t size, uint64_t offset)
{
  uint64_t pos;
  for (pos = 0 ; pos < size ; ++pos)
  {
    if (entropy(buffer + pos, AES128_KEY_SCHEDULE_SIZE))
      continue;

    if (valid_aes128_schedule(buffer + pos))
    {
      printf ("Found AES-128 key schedule at offset 0x%"PRIx64": \n", 
	      offset + pos);
      display_key(buffer + pos, AES128_KEY_SIZE);
    }
    if (valid_aes192_schedule(buffer + pos))
    {
      printf ("Found AES-192 key schedule at offset 0x%"PRIx64": \n", 
	      offset + pos);
      display_key(buffer + pos, AES192_KEY_SIZE);
    }
    if (valid_aes256_schedule(buffer + pos))
    {
      printf ("Found AES-256 key schedule at offset 0x%"PRIx64": \n", 
	      offset + pos);
      display_key(buffer + pos, AES256_KEY_SIZE);
    }
  }
}


// Use a sliding window scanner on the file to search for AES key schedules
int scan_file(char * fn)
{
  uint64_t offset = 0;
  unsigned char * buffer;
  FILE * handle;
  size_t bytes_read;

  if (NULL == fn)
    return TRUE;

  buffer = (unsigned char *)malloc(sizeof(unsigned char) * BUFFER_SIZE + WINDOW_SIZE);
  if (NULL == buffer)
    return TRUE;
  memset(buffer, 0, BUFFER_SIZE + WINDOW_SIZE);

  handle = fopen(fn,"rb");
  if (NULL == handle)
  {
    perror(fn);
    return TRUE;
  }

  printf ("Searching %s\n", fn);

  while (!feof(handle))
  {
    // Clear out the buffer except for whatever data we have copied
    // from the end of the last buffer
    memset(buffer + WINDOW_SIZE, 0, BUFFER_SIZE);

    //    printf ("Reading from 0x%"PRIx64"\n", ftello(handle));

    // Read into the buffer without overwriting the existing data
    bytes_read = (uint64_t)fread(buffer + WINDOW_SIZE,1,BUFFER_SIZE,handle);

    if (0 == offset)
    {      
      uint64_t size;
      if (bytes_read < BUFFER_SIZE)
	size = bytes_read;
      else
	size = bytes_read - WINDOW_SIZE;

      scan_buffer(buffer + WINDOW_SIZE, size , 0);
    }
    else
      scan_buffer(buffer, bytes_read, offset - WINDOW_SIZE);

    offset += bytes_read;

    // Copy the end of the buffer back to the beginning for the next window
    memcpy(buffer, buffer + BUFFER_SIZE, WINDOW_SIZE);
  }

  free(buffer);
  return FALSE;
}


int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf ("FindAES version 1.1 by Jesse Kornblum\n");
    printf ("Searches for AES-128, AES-192, and AES-256 keys\n\n");

    printf ("Usage: findaes [FILE]\n");
    return EXIT_FAILURE;
  }

  scan_file(argv[1]);

  return EXIT_SUCCESS;
}
