
#define VERSION				"1.0.0"
#define READ_STREAM			0
#define READ_RESOURCES		1
#define READ_ATTRIBUTES		2
#define READ_MESSAGE			3

#define WRITE_STREAM			0
#define WRITE_MESSAGE		1
#define WRITE_RESOURCES		2
#define WRITE_ATTRIBUTES	3

#include <StorageDefs.h>
#include <Message.h>
#include <List.h>
#include <File.h>

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>


#include "marcontrol.h"
#include "reading.h"
#include "writing.h"

void usage (const char *);

int
main (int ac, char **av)
{
	char filename[B_PATH_NAME_LENGTH] = "";
	int reading (READ_STREAM);
	int writing (WRITE_STREAM);
	ReadControl rcontrol;
	int c;

	while ((c = getopt (ac, av, "hr:o:s:b:w:W:")) >= 0)
	{
		switch (c)
		{
			case 'h':
				usage (av[0]);
				return 0;

			case '?':
				usage (av[0]);
				return -1;

			case 'r':
				if (!strcmp (optarg, "r"))
				{
					reading = READ_RESOURCES;
					break;
				}

				if (!strcmp (optarg ,"a"))
				{
					reading = READ_ATTRIBUTES;
					break;
				}

				if (!strcmp (optarg, "m"))
				{
					reading = READ_MESSAGE;
					break;
				}

				fprintf (stderr, "Illegal use of -r option\n");
				usage (av[0]);
				return -1;

			case 'w':
				if (!strcmp (optarg, "r"))
				{
					writing = WRITE_RESOURCES;
					break;
				}

				if (!strcmp (optarg, "a"))
				{
					writing = WRITE_ATTRIBUTES;
					break;
				}

				if (!strcmp (optarg, "m"))
				{
					writing = WRITE_MESSAGE;
					break;
				}

				fprintf (stderr, "Illegal use of -w option\n");
				usage (av[0]);
				return -1;

			case 'o':
				strcpy (filename, optarg);
				break;

			case 's':
				if (!rcontrol.AddString (optarg))
				{
					fprintf (stderr, "Illegal -s argument\n");
					usage (av[0]);
					return -1;
				}
				break;

			case 'b':
				if (!rcontrol.AddRaw (optarg))
				{
					fprintf (stderr, "Illegal -b argument\n");
					usage (av[0]);
					return -1;
				}
				break;

			case 'W':
				rcontrol.width = atoi (optarg);
				break;
		}
	}

	if (optind == ac && reading)
	{
		fprintf (stderr, "Missing input file name\n");
		usage (av[0]);
		return -1;
	}

	if (reading && writing)
	{
		fprintf (stderr, "Input or Output has to be XML\n");
		usage (av[0]);
		return -1;
	}

	if (!reading && !writing)
	{
		fprintf (stderr, "No mode specified.  Try -r or -w\n");
		usage (av[0]);
		return -1;
	}

	if (writing != WRITE_STREAM
	&&  *filename == '\0')
	{
		fprintf (stderr, "Output file parameter required\n");
		usage (av[0]);
		return -1;
	}

	
	if (reading)
	{
		FILE *rout (stdout);

		if (*filename)
		{
			if ((rout = fopen (filename, "w")) == 0)
			{
				fprintf (stderr, "Cannot open output file %s\n", filename);
				return -1;
			}
		}
	
		for (int i = optind; i < ac; ++i)
		{
			if (reading == READ_MESSAGE)
				read_message_file (rout, av[i], &rcontrol);

			else if (reading == READ_ATTRIBUTES)
				read_attribute_file (rout, av[i], &rcontrol);

			else if (reading == READ_RESOURCES)
				read_resource_file (rout, av[i], &rcontrol);
		}

		if (rout != stdout)
			fclose (rout);
	}
	else
	{
		BFile file (filename, B_READ_WRITE | B_CREATE_FILE);

		if (file.InitCheck() != B_NO_ERROR)
		{
			fprintf (stderr, "Cannot open output file %s\n", filename);
			return -1;
		}

		if (optind == ac) // stdin input
		{
			MarControl marc;

			if (marc.Parse()   == MAR_OK
			&&  marc.Message() != 0)
			{
				if (writing == WRITE_ATTRIBUTES)
					write_attribute_file (&file, marc.Message(), true);
				else if (writing == WRITE_RESOURCES)
					write_attribute_file (&file, marc.Message(), true);
				else
					write_message_file (&file, marc.Message(), true);
			}
		}
		else
		{
			const char **array;

			array = new const char * [ac - optind];
			for (int i = optind; i < ac; ++i)
				array[i - optind] = av[i];

			MarControl marc (ac - optind, array);

			if (marc.Parse() == MAR_OK
			&&  marc.Message() != 0)
			{
				BMessage *msg (marc.Message());

				for (int32 i = 0; i < ac - optind; ++i)
				{
					BMessage sub;

					if (msg->HasMessage (av[i + optind]))
					{
						msg->FindMessage (av[i + optind], &sub);

						if (writing == WRITE_ATTRIBUTES)
							write_attribute_file (&file, &sub, i == 0);
						else if (writing == WRITE_RESOURCES)
							write_attribute_file (&file, &sub, i == 0);
						else
							write_message_file (&file, &sub, i == 0);
					}
				}
			}
		}
	}

	return 0;
}

void
usage (const char *program)
{
	fprintf (stderr, "MAR v%s usage: %s [options] [file ...]\n", VERSION, program);
	fprintf (stderr, "options:\n");
	fprintf (stderr, "   -h               Display this information.\n");
	fprintf (stderr, "   -r<m|a|r>        Reading mode assumes input files are in binary format.\n");
	fprintf (stderr, "                    Output will be tagged XML like text.  If reading mode\n");
	fprintf (stderr, "                    is specified, and the -o option is absent, output will be\n");
	fprintf (stderr, "                    written to stdout.\n");
	fprintf (stderr, "                    m - read files as flattened messages.\n");
	fprintf (stderr, "                    a - read files' attributes.\n"); 
	fprintf (stderr, "                    r - read files' resources.\n");
	fprintf (stderr, "   -s <type>        Treat type as string.  type is either its numeric\n");
	fprintf (stderr, "                    representation or four character representation.\n");
	fprintf (stderr, "                    Option is ignored if input is XML.\n");
	fprintf (stderr, "   -b <type>        Treat type as binary data (raw).  type is either its\n");
	fprintf (stderr, "                    numeric representation or four character representation.\n");
	fprintf (stderr, "                    Option is ignored if input is XML.\n");
	fprintf (stderr, "   -W <n>           Specify width of raws data in n characters.\n");
	
	fprintf (stderr, "\n");
	fprintf (stderr, "   -w<m|a|r>        Writing mode assumes input files are XML.  For writing mode,\n");
	fprintf (stderr, "                    -o option must be present.  If input file is absent from\n");
	fprintf (stderr, "                    command line, input is read from stdin.  Output specfied by\n");
	fprintf (stderr, "                    -w argument is as follows:\n");
	fprintf (stderr, "                    m - write output as flattened message.\n");
	fprintf (stderr, "                    a - write output as attributes.\n");
	fprintf (stderr, "                    r - write output as resources.\n");
	fprintf (stderr, "   -o <filename>    Write output to filename.\n\n");
}

