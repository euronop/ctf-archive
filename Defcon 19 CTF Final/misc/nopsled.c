#include <stdlib.h>

#include <libexploit/libexploit.h>



int main(int argc, char *argv[])
{
	E_DATABUF_AUTO( db );
	int len = 128;

	if (argc == 2) {
		len = atoi(argv[1]);
	}

	ex_databuf_extend(db, len);
	db->length = len;
	ex_rand_nop_buf(db->data, db->length);
	ex_io_write(1, db);

	return EXIT_SUCCESS;
}
