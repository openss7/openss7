...

struct type_IMAGE_ReadImage__INV {
    struct element_IMAGE_1 {
	int nelem;
	struct choice_IMAGE_0 {
	    int     offset;
#define	choice_IMAGE_0_file__name	1
#define	choice_IMAGE_0_mail__address	2

	    union {
		struct type_IMAGE_Filename *file__name;

		struct type_IMAGE_Mail__Address *mail__address;
	    }       un;
	} *element_IMAGE_2[1];
    } *element_IMAGE_0;

    struct element_IMAGE_4 {
	int nelem;
	struct type_IMAGE_Format *Format[1];
    } *element_IMAGE_3;
};
int	free_IMAGE_ReadImage__INV ();

...
