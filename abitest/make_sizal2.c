#include "defines.h"
#include <stdio.h>

/* These defines control the building of the list of types to check. There
   is a string identifying the type (with a comma after), a size of the type
   (also with a comma and an integer for adding to the total amount of types.  
*/
#ifdef CHECK_INT128
#define CI128_S "__int128",
#define CI128_I 16, 
#else
#define CI128_S
#define CI128_I
#endif
#ifdef CHECK_LONG_DOUBLE
#define CLD_S "long double",
#define CLD_I 16, 
#else
#define CLD_S
#define CLD_I
#endif
#ifdef CHECK_FLOAT128
#define CF128_S "__float128",
#define CF128_I 16, 
#else
#define CF128_S
#define CF128_I
#endif
#ifdef CHECK_M64_M128
#define CMM_S "__m64", "__m128",
#define CMM_I 8, 16, 
#else
#define CMM_S
#define CMM_I
#endif

/* Find the maximum of three integers.  */
int
max3 (int integers[])
{
  int max = (integers[0] > integers[1] ? integers[0] : integers[1]);
  return (max > integers[2] ? max : integers[2]);
}

/* This function calculates the size of a struct when elements have size
   and alignment of the list items.  Structs have a basic alignment like
   the largest alignment of it's single elements. Structs are layed out so
   they always fill in as much in the single alignment slots as possible.  */

int
calculate_struct_size (int element_count, int size[], int alignment[])
{
  int struct_size = 0, struct_alignment = 0, current_alignment = 0;
  int i;

  /* Find alignment of the struct elements.  */
  for (i=0; i<element_count; i++)
    if (struct_alignment < alignment[i])
      struct_alignment = alignment[i];

  for (i=0; i<element_count; i++) {
    /* Update size with minimum value.  */
    struct_size += size[i];
    current_alignment += size[i];

    if (current_alignment > struct_alignment)
      {
	/* This element doesn't fit within this alignment, make padding.  */
	struct_size += (struct_alignment - (current_alignment - size[i]));

	/* Set current_alignment to the base of this alignment.  */
	current_alignment = size[i];
      }
    else
      if ((current_alignment % alignment[i]) != 0)
	{
	  /* Pad for the alignment of this type. Can happen for structs
	     like { char, short, int }.  */
	  int padding = alignment[i] - (current_alignment % alignment[i]);

	  struct_size += padding;
	  current_alignment += padding;
	}

    if (current_alignment == struct_alignment)
      /* Current element fills alignment slot. Start over.  */
      current_alignment = 0;
  }

  /* Add final padding.  */
  if (current_alignment != 0)
    struct_size += (struct_alignment - current_alignment);

  return struct_size;
}


/* This constructs the test for size of structs and unions with three scalar
   types.  */
int
main (int argc, char **argv)
{
  int i, j, k;
  char *types[] = { "char", "short", "int", "long", "long long",
		    "float", "double", CI128_S CLD_S CF128_S CMM_S };
  int sizes[] = { 1, 2, 4, 8, 8, 4, 8, CI128_I CLD_I CF128_I CMM_I };
  int maxcheck = sizeof(types) / sizeof(types[0]);

  for (i=0; i<maxcheck; i++)
    for (j=0; j<maxcheck; j++)
      for (k=0; k<maxcheck; k++) {
	int element_sizes[3], struct_size, union_size;

	element_sizes[0] = sizes[i];
	element_sizes[1] = sizes[j];
	element_sizes[2] = sizes[k];

	/* There is no difference between size and alignment in the abi.  */
	struct_size = calculate_struct_size (3, element_sizes, element_sizes);
	union_size = max3(element_sizes);

	printf ("check_struct_and_union3(%s, %s, %s, %d, %d);\n", types[i],
		types[j], types[k], struct_size, union_size);
      }

  return 0;
}
