#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

// FindAES version 1.0 by Jesse Kornblum
// http://jessekornblum.com/tools/findaes/
// This code is public domain.

#include "aes.h"

#define BUFFER_SIZE  10485760
#define WINDOW_SIZE  AES256_KEY_SCHEDULE_SIZE


void display_key(unsigned char * key, uint64_t sz)
{
  uint64_t pos = 0;
  while (pos < sz)
  {
    printf("%02x ", key[pos]);
    ++pos;
  }
  printf("\n");
}


void scan_buffer(unsigned char * buffer, uint64_t size, uint64_t offset)
{
  uint64_t pos;
  for (pos = 0 ; pos < size ; ++pos)
  {
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
  int count;

  if (1 == argc)
  {
    printf ("FindAES version 1.0 by Jesse Kornblum\n");
    printf ("Searches for AES-128, AES-192, and AES-256 keys\n\n");

    printf ("Usage: findaes [FILES]\n");
    return EXIT_FAILURE;
  }

  for (count = 1 ; count < argc ; ++count)
    scan_file(argv[count]);

  return EXIT_SUCCESS;
}
