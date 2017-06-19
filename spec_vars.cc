#include <time.h>
#include "specs.h"

spec_unit		specs[256];

unsigned int	spec_unit_count;

bool			ws_tab[256];

char*			program_name;

unsigned int	last_word_to_scan = 0;
bool			there_is_a_read = false;

char			space = ' ';

unsigned int	max_src_len;

time_t			specs_start_time = time(0);

char*			converted_string;

bool			dodebugprt = false;

bool			dodebugprtx = false;
