#include <unistd.h>
#include <string.h>

#include <nemesi/types.h>

#include "common.h"
#include "interface.h"

/*#include <nemesi/comm.h>*/

/* char buf[16384]; */
/* struct mpstr mp; */

#define BUFFER 8192

int get_plugin_pt(void);
int decode(char *, int, uint8 * (*)());

int get_plugin_pt(void)
{
	return 96;
}

int decode(char *data, int len, uint8 * (*ab_get) (uint32))
{
	static PMPSTR mp = NULL;

	int size;
	char out[BUFFER];
	int ret;
	uint8 *audio_data;

	if (!mp) {
		if ((mp = (PMPSTR) malloc(sizeof(MPSTR))) == NULL) {
			fprintf(stderr, "Cannot allocate memory!\n");
			return 1;
		}
		InitMP3(mp);
	}

	ret = decodeMP3(mp, data + 4, len - 4, out, BUFFER, &size);
	while (ret == MP3_OK) {
/*		write(audio_fd, out, size); */
		audio_data = (*ab_get) ((uint32) size);
		memcpy(audio_data, out, size);
		ret = decodeMP3(mp, NULL, 0, out, BUFFER, &size);
	}

	return 0;
}
