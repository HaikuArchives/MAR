
#include <File.h>
#include <Resources.h>
#include <Message.h>
#include <Point.h>
#include <Rect.h>
#include <Entry.h>
#include <Messenger.h>
#include <GraphicsDefs.h>
#include <List.h>
#include <String.h>
#include <fs_attr.h>

#include <stdio.h>
#include <ctype.h>

#include "reading.h"

void write_xml (FILE *, BMessage *, int, ReadControl *);
void write_xml_bool (FILE *, const char *, bool);
void write_xml_double (FILE *, const char *, double);
void write_xml_float (FILE *, const char *, float);
void write_xml_int64 (FILE *, const char *, int64);
void write_xml_int32 (FILE *, const char *, int32);
void write_xml_int16 (FILE *, const char *, int16);
void write_xml_int8 (FILE *, const char *, int8);
void write_xml_message (FILE *, const char *, BMessage *, int, ReadControl *);
void write_xml_messenger (FILE *, const char *, const BMessenger &);
void write_xml_mime (FILE *, const char *, const char *);
void write_xml_off (FILE *, const char *, off_t);
void write_xml_pointer (FILE *, const char *, void *);
void write_xml_point (FILE *, const char *, BPoint);
void write_xml_rect (FILE *, const char *, BRect);
void write_xml_ref (FILE *, const char *, entry_ref *);
void write_xml_color (FILE *, const char *, const rgb_color *);
void write_xml_size (FILE *, const char *, size_t);
void write_xml_ssize (FILE *, const char *, ssize_t);
void write_xml_string (FILE *, const char *, const char *, const char * = 0);
void write_xml_time (FILE *, const char *,  bigtime_t);
void write_xml_uint64 (FILE *, const char *, uint64);
void write_xml_uint32 (FILE *, const char *, uint32);
void write_xml_uint16 (FILE *, const char *, uint16);
void write_xml_uint8 (FILE *, const char *, uint8);
void write_xml_raw (FILE *, const char *, const char *, size_t, int, ReadControl *, const char * = 0);

bool write_xml_unhandled (FILE *, const char *, uint32, const char *, size_t, int, ReadControl *);
void unhandled_type (const char *, uint32);

void
read_message_file (FILE *fp, const char *input, ReadControl *rcontrol)
{
	BFile file (input, B_READ_ONLY);

	if (file.InitCheck() == B_NO_ERROR)
	{
		BMessage msg;

		fprintf (fp, "<!-- FILE: %s -->\n", input);

		while (msg.Unflatten (&file) == B_NO_ERROR)
		{
			write_xml (fp, &msg, 0, rcontrol);
			fprintf (fp, "\n");
			msg.MakeEmpty();
		}
	}
}

void
read_resource_file (FILE *fp, const char *input, ReadControl *rcontrol)
{
	BFile file (input, B_READ_ONLY);

	if (file.InitCheck() == B_NO_ERROR)
	{
		BResources res (&file);
		const char *name;
		type_code type;
		size_t size;
		int32 id;

		fprintf (fp, "<!-- FILE: %s -->\n", input);
		for (int32 i = 0; res.GetResourceInfo (i, &type, &id, &name, &size); ++i)
		{
			switch (type)
			{
				case B_BOOL_TYPE:
				{
					bool value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);
					write_xml_bool (fp, name, value);
					break;
				}

				case B_DOUBLE_TYPE:
				{
					double value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);
					write_xml_double (fp, name, value);
					break;
				}

				case B_FLOAT_TYPE:
				{
					float value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);
					write_xml_float (fp, name, value);
					break;
				}

				case B_INT64_TYPE:
				{
					int64 value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);
					write_xml_int64 (fp, name, value);
					break;
				}

				case B_INT32_TYPE:
				{
					int32 value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);
					write_xml_int32 (fp, name, value);
					break;
				}

				case B_INT16_TYPE:
				{
					int16 value;
					
					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);
					write_xml_int16 (fp, name, value);
					break;
				}

				case B_INT8_TYPE:
				{
					int8 value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);
					write_xml_int8 (fp, name, value);
					break;
				}

				case B_MESSAGE_TYPE:
				{
					char *buffer;
					BMessage sub;

					buffer = new char [size];
					res.ReadResource (
						type,
						id,
						buffer,
						0,
						size);

					sub.Unflatten (buffer);
					delete [] buffer;

					write_xml_message (fp, name, &sub, 0, rcontrol);
					break;
				}

				/* Not supported in resources
				case B_MESSENGER_TYPE:
					break;
				*/

				case B_MIME_TYPE:
				{
					char *buffer;

					buffer = new char [size + 1];
					res.ReadResource (
						type,
						id,
						buffer,
						0,
						size);
					buffer[size] = 0;

					write_xml_mime (fp, name, buffer);
					delete [] buffer;
					break;
				}

				case B_OFF_T_TYPE:
				{
					off_t value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);
					write_xml_off (fp, name, value);
					break;
				}

				/* not supported in resources
				case B_POINTER_TYPE:
					break;
				*/

				case B_POINT_TYPE:
				{
					BPoint value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);
					write_xml_point (fp, name, value);
					break;
				}

				case B_RECT_TYPE:
				{
					BRect value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);
					write_xml_rect (fp, name, value);
					break;
				}

				case B_REF_TYPE:
				{
					entry_ref value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);
					write_xml_ref (fp, name, &value);
					break;
				}

				case B_RGB_COLOR_TYPE:
				{
					rgb_color value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);
					write_xml_color (fp, name, &value);
					break;
				}

				case B_SIZE_T_TYPE:
				{
					size_t value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);

					write_xml_size (fp, name, value);
					break;
				}

				case B_SSIZE_T_TYPE:
				{
					ssize_t value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);
					write_xml_ssize (fp, name, value);
					break;
				}

				case B_STRING_TYPE:
				{
					char *buffer;

					buffer = new char [size + 1];
					res.ReadResource (
						type,
						id,
						buffer,
						0,
						size);
					buffer[size] = 0;
					write_xml_string (fp, name, buffer);
					delete [] buffer;
					break;
				}

				case B_TIME_TYPE:
				{
					bigtime_t value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);
					write_xml_time (fp, name, value);
					break;
				}

				case B_UINT64_TYPE:
				{
					uint32 value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);
					write_xml_uint64 (fp, name, value);
					break;
				}

				case B_UINT32_TYPE:
				{
					uint32 value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);

					write_xml_uint32 (fp, name, value);
					break;
				}

				case B_UINT16_TYPE:
				{
					
					uint16 value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);

					write_xml_uint16 (fp, name, value);
					break;
				}

				case B_UINT8_TYPE:
				{
					uint8 value;

					res.ReadResource (
						type,
						id,
						&value,
						0,
						size);
					write_xml_uint8 (fp, name, value);
					break;
				}


				case B_RAW_TYPE:
				{
					char *buffer;

					buffer = new char [size + 1];
					res.ReadResource (
						type,
						id,
						buffer,
						0,
						size);
					buffer[size] = 0;
					write_xml_raw (fp, name, buffer, size, 0, rcontrol);
					delete [] buffer;
					break;
				}

				default:
				{
					char *buffer;
					buffer = new char [size + 1];

					res.ReadResource (
						type,
						id,
						buffer,
						0,
						size);
					buffer[size] = 0;

					write_xml_unhandled (fp, name, type, buffer, size, 0, rcontrol);
					delete [] buffer;

					break;
				}
			}
		}
		fprintf (fp, "\n");
	}
}

void
read_attribute_file (FILE *fp, const char *input, ReadControl *rcontrol)
{
	BFile file (input, B_READ_ONLY);

	if (file.InitCheck() == B_NO_ERROR)
	{
		char name[B_ATTR_NAME_LENGTH];
		attr_info info;

		fprintf (fp, "<!-- FILE: %s -->\n", input);

		file.RewindAttrs();
		while (file.GetNextAttrName (name)    == B_NO_ERROR
		&&     file.GetAttrInfo (name, &info) == B_NO_ERROR)
		{
			switch (info.type)
			{
				case B_BOOL_TYPE:
				{
					bool value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);
					write_xml_bool (fp, name, value);
					break;
				}

				case B_DOUBLE_TYPE:
				{
					double value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);
					write_xml_double (fp, name, value);
					break;
				}

				case B_FLOAT_TYPE:
				{
					float value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);
					write_xml_float (fp, name, value);
					break;
				}

				case B_INT64_TYPE:
				{
					int64 value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);
					write_xml_int64 (fp, name, value);
					break;
				}

				case B_INT32_TYPE:
				{
					int32 value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);
					write_xml_int32 (fp, name, value);
					break;
				}

				case B_INT16_TYPE:
				{
					int16 value;
					
					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);
					write_xml_int16 (fp, name, value);
					break;
				}

				case B_INT8_TYPE:
				{
					int8 value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);
					write_xml_int8 (fp, name, value);
					break;
				}

				case B_MESSAGE_TYPE:
				{
					char *buffer;
					BMessage sub;

					buffer = new char [info.size];
					file.ReadAttr (
						name,
						info.type,
						0,
						buffer,
						info.size);

					sub.Unflatten (buffer);
					delete [] buffer;

					write_xml_message (fp, name, &sub, 0, rcontrol);
					break;
				}

				/* Not supported in resources
				case B_MESSENGER_TYPE:
					break;
				*/

				case B_MIME_TYPE:
				{
					char *buffer;

					buffer = new char [info.size + 1];
					file.ReadAttr (
						name,
						info.type,
						0,
						buffer,
						info.size);
					buffer[info.size] = 0;

					write_xml_mime (fp, name, buffer);
					delete [] buffer;
					break;
				}

				case B_OFF_T_TYPE:
				{
					off_t value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);
					write_xml_off (fp, name, value);
					break;
				}

				/* not supported in resources
				case B_POINTER_TYPE:
					break;
				*/

				case B_POINT_TYPE:
				{
					BPoint value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);
					write_xml_point (fp, name, value);
					break;
				}

				case B_RECT_TYPE:
				{
					BRect value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);
					write_xml_rect (fp, name, value);
					break;
				}

				case B_REF_TYPE:
				{
					entry_ref value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);
					write_xml_ref (fp, name, &value);
					break;
				}

				case B_RGB_COLOR_TYPE:
				{
					rgb_color value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);
					write_xml_color (fp, name, &value);
					break;
				}

				case B_SIZE_T_TYPE:
				{
					size_t value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);

					write_xml_size (fp, name, value);
					break;
				}

				case B_SSIZE_T_TYPE:
				{
					ssize_t value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);
					write_xml_ssize (fp, name, value);
					break;
				}

				case B_STRING_TYPE:
				{
					char *buffer;

					buffer = new char [info.size + 1];
					file.ReadAttr (
						name,
						info.type,
						0,
						buffer,
						info.size);
					buffer[info.size] = 0;
					write_xml_string (fp, name, buffer);
					delete [] buffer;
					break;
				}

				case B_TIME_TYPE:
				{
					bigtime_t value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);
					write_xml_time (fp, name, value);
					break;
				}

				case B_UINT64_TYPE:
				{
					uint32 value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);
					write_xml_uint64 (fp, name, value);
					break;
				}

				case B_UINT32_TYPE:
				{
					uint32 value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);

					write_xml_uint32 (fp, name, value);
					break;
				}

				case B_UINT16_TYPE:
				{
					uint16 value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);

					write_xml_uint16 (fp, name, value);
					break;
				}

				case B_UINT8_TYPE:
				{
					uint8 value;

					file.ReadAttr (
						name,
						info.type,
						0,
						&value,
						info.size);
					write_xml_uint8 (fp, name, value);
					break;
				}


				case B_RAW_TYPE:
				{
					char *buffer;

					buffer = new char [info.size + 1];
					file.ReadAttr (
						name,
						info.type,
						0,
						buffer,
						info.size);
					buffer[info.size] = 0;
					write_xml_raw (fp, name, buffer, info.size, 0, rcontrol);
					delete [] buffer;
					break;
				}

				default:
				{
					char *buffer;
					buffer = new char [info.size + 1];

					file.ReadAttr (
						name,
						info.type,
						0,
						buffer,
						info.size);
					buffer[info.size] = 0;

					write_xml_unhandled (fp, name, info.type, buffer, info.size, 0, rcontrol);
					delete [] buffer;
					break;
				}
			}
		}

		fprintf (fp, "\n");
	}

}

void
write_xml (FILE *fp, BMessage *msg, int tabs, ReadControl *rcontrol)
{
	char *name;
	uint32 type;
	int32 count;

	for (int32 i = 0; msg->GetInfo (B_ANY_TYPE, i, &name, &type, &count) == B_NO_ERROR; ++i)
	{
		for (int32 j = 0; j < count; ++j)
		{
			for (int t = 0; t < tabs; ++t)
				fprintf (fp, "\t");

			switch (type)
			{
				case B_BOOL_TYPE:
					write_xml_bool (fp, name, msg->FindBool (name, j));
					break;

				case B_DOUBLE_TYPE:
					write_xml_double (fp, name, msg->FindDouble (name, j));
					break;

				case B_FLOAT_TYPE:
					write_xml_float (fp, name, msg->FindFloat (name, j));
					break;

				case B_INT64_TYPE:
					write_xml_int64 (fp, name, msg->FindInt64 (name, j));
					break;

				case B_INT32_TYPE:
					write_xml_int32 (fp, name, msg->FindInt32 (name, j));
					break;

				case B_INT16_TYPE:
					write_xml_int16 (fp, name, msg->FindInt16 (name, j));
					break;

				case B_INT8_TYPE:
					write_xml_int8 (fp, name, msg->FindInt8 (name, j));
					break;

				case B_MESSAGE_TYPE:
				{
					BMessage sub;

					msg->FindMessage (name, j, &sub);
					write_xml_message (fp, name, &sub, tabs, rcontrol);
					break;
				}

				case B_MESSENGER_TYPE:
				{
					BMessenger msgr;

					msg->FindMessenger (name, j, &msgr);
					write_xml_messenger (fp, name, msgr);
					break;
				}

				case B_MIME_TYPE:
				{
					const char *buffer;
					ssize_t size;

					msg->FindData (
						name,
						B_MIME_TYPE,
						j,
						reinterpret_cast<const void **>(&buffer),
						&size);
						
					write_xml_mime (fp, name, buffer);
					break;
				}

				case B_OFF_T_TYPE:
				{
					const off_t *buffer;
					ssize_t size;

					msg->FindData (
						name,
						B_OFF_T_TYPE,
						j,
						reinterpret_cast<const void **>(&buffer),
						&size);

					write_xml_off (fp, name, *buffer);
					break;
				}

				case B_POINTER_TYPE:
				{
					void *pointer;

					msg->FindPointer (name, j, &pointer);
					write_xml_pointer (fp, name, pointer);
					break;
				}

				case B_POINT_TYPE:
					write_xml_point (fp, name, msg->FindPoint (name, j));
					break;

				case B_RECT_TYPE:
					write_xml_rect (fp, name, msg->FindRect (name, j));
					break;

				case B_REF_TYPE:
				{
					entry_ref ref;

					msg->FindRef (name, j, &ref);
					write_xml_ref (fp, name, &ref);
					break;
				}

				case B_RGB_COLOR_TYPE:
				{
					const rgb_color *buffer;
					ssize_t size;

					msg->FindData (
						name,
						B_RGB_COLOR_TYPE,
						j,
						reinterpret_cast<const void **>(&buffer),
						&size);

					write_xml_color (fp, name, buffer);
					break;
				}

				case B_SIZE_T_TYPE:
				{
					const size_t *buffer;
					ssize_t size;

					msg->FindData (
						name,
						B_SIZE_T_TYPE,
						j,
						reinterpret_cast<const void **>(&buffer),
						&size);

					write_xml_size (fp, name, *buffer);
					break;
				}

				case B_SSIZE_T_TYPE:
				{
					const ssize_t *buffer;
					ssize_t size;

					msg->FindData (
						name,
						B_SSIZE_T_TYPE,
						j,
						reinterpret_cast<const void **>(&buffer),
						&size);

					write_xml_ssize (fp, name, *buffer);
					break;
				}

				case B_STRING_TYPE:
					write_xml_string (fp, name, msg->FindString (name, j));
					break;

				case B_TIME_TYPE:
				{
					const bigtime_t *buffer;
					ssize_t size;

					msg->FindData (
						name,
						B_TIME_TYPE,
						j,
						reinterpret_cast<const void **>(&buffer),
						&size);

					write_xml_time (fp, name, *buffer);
					break;
				}

				case B_UINT64_TYPE:
				{
					const uint64 *buffer;
					ssize_t size;

					msg->FindData (
						name,
						B_UINT64_TYPE,
						j,
						reinterpret_cast<const void **>(&buffer),
						&size);

					write_xml_uint64 (fp, name, *buffer);
					break;
				}

				case B_UINT32_TYPE:
				{
					const uint32 *buffer;
					ssize_t size;

					msg->FindData (
						name,
						B_UINT32_TYPE,
						j,
						reinterpret_cast<const void **>(&buffer),
						&size);

					write_xml_uint32 (fp, name, *buffer);
					break;
				}

				case B_UINT16_TYPE:
				{
					const uint16 *buffer;
					ssize_t size;

					msg->FindData (
						name,
						B_UINT16_TYPE,
						j,
						reinterpret_cast<const void **>(&buffer),
						&size);

					write_xml_uint16 (fp, name, *buffer);
					break;
				}

				case B_UINT8_TYPE:
				{
					const uint8 *buffer;
					ssize_t size;

					msg->FindData (
						name,
						B_UINT8_TYPE,
						j,
						reinterpret_cast<const void **>(&buffer),
						&size);

					write_xml_uint8 (fp, name, *buffer);
					break;
				}


				case B_RAW_TYPE:
				{
					const char *buffer;
					ssize_t size;

					msg->FindData (
						name,
						B_RAW_TYPE,
						j,
						reinterpret_cast<const void **>(&buffer),
						&size);

					write_xml_raw (fp, name, buffer, size, tabs, rcontrol);
					break;
				}

				default:
				{
					const char *mbuf;
					char *buffer;
					ssize_t size;

					msg->FindData (
						name,
						type,
						j,
						reinterpret_cast<const void **>(&mbuf),
						&size);

					buffer = new char [size + 1];
					memcpy (buffer, mbuf, size);
					buffer[size] = 0;
						
					write_xml_unhandled (fp, name, type, buffer, size, tabs, rcontrol);
					delete [] buffer;
					break;
				
				}
			}
		}
	}
}

void
write_xml_bool (FILE *fp, const char *name, bool value)
{
	fprintf (fp, "<bool name=\"%s\">%s</bool>\n",
		name, value ? "true" : "false");
}


void
write_xml_double (FILE *fp, const char *name, double value)
{
	fprintf (fp, "<double name=\"%s\">%g</double>\n",
		name, value);
}

void
write_xml_float (FILE *fp, const char *name, float value)
{
	fprintf (fp, "<float name=\"%s\">%f</float>\n",
		name, value);
}

void
write_xml_int64 (FILE *fp, const char *name, int64 value)
{
	fprintf (fp, "<int64 name=\"%s\">%Ld</int64>\n",
		name, value);
}

void
write_xml_int32 (FILE *fp, const char *name, int32 value)
{
	fprintf (fp, "<int32 name=\"%s\">%ld</int32>\n",
		name, value);
}

void
write_xml_int16 (FILE *fp, const char *name, int16 value)
{
	fprintf (fp, "<int16 name=\"%s\">%d</int16>\n",
		name, value);
}

void
write_xml_int8 (FILE *fp, const char *name, int8 value)
{
	int k (value);

	fprintf (fp, "<int8 name=\"%s\">%d</int8>\n",
		name, k);
}

void
write_xml_message (FILE *fp, const char *name, BMessage *value, int tabs, ReadControl *rcontrol)
{
	fprintf (fp, "<message name=\"%s\" what=\"%lu\">\n",
		name, value->what);
	write_xml (fp, value, tabs + 1, rcontrol);
	fprintf (fp, "</message>\n");
}

void
write_xml_messenger (FILE *fp, const char *name, const BMessenger &value)
{
	BLooper *looper;
	BHandler *handler;

	handler = value.Target (&looper);
	fprintf (fp, "<messenger name=\"%s\">%p,%p</messenger>\n",
		name, handler, looper);
}

void
write_xml_mime (FILE *fp, const char *name, const char *value)
{
	fprintf (fp, "<mime name=\"%s\">%s</mime>\n",
		name, value);
}

void
write_xml_off (FILE *fp, const char *name, off_t value)
{
	fprintf (fp, "<off name=\"%s\">%Ld</off>\n",
		name, value);
}

void
write_xml_pointer (FILE *fp, const char *name, void *value)
{
	fprintf (fp, "<pointer name=\"%s\">%p</pointer>\n",
		name, value);
}

void
write_xml_point (FILE *fp, const char *name, BPoint value)
{
	fprintf (fp, "<point name=\"%s\">%.2f,%.2f</point>\n",
		name, value.x, value.y);
}

void
write_xml_rect (FILE *fp, const char *name, BRect value)
{
	fprintf (fp, "<rect name=\"%s\">%.2f,%.2f,%.2f,%.2f</rect>\n",
		name, value.left, value.top, value.right, value.bottom);
}

void
write_xml_ref (FILE *fp, const char *name, entry_ref *value)
{
	fprintf (fp, "<ref name=\"%s\">%ld,%Ld,%s</ref>\n",
		name, value->device, value->directory, value->name);
}

void
write_xml_color (FILE *fp, const char *name, const rgb_color *value)
{
	int color[4];

	color[0] = value->red;
	color[1] = value->green;
	color[2] = value->blue;
	color[3] = value->alpha;

	fprintf (fp, "<color name=\"%s\">%d,%d,%d,%d</color>\n",
		name, color[0], color[1], color[2], color[3]);
}

void
write_xml_size (FILE *fp, const char *name, size_t value)
{
	fprintf (fp, "<size name=\"%s\">%lu</size>\n",
		name, value);
}

void
write_xml_ssize (FILE *fp, const char *name, ssize_t value)
{
	fprintf (fp, "<ssize name=\"%s\">%ld</ssize>\n",
		name, value);
}

void
write_xml_string (FILE *fp, const char *name, const char *value, const char *code)
{
	fprintf (fp, "<string name=\"%s\"", name);

	if (code)
	{
		if (isprint (code[0])
		&&  isprint (code[1])
		&&  isprint (code[2])
		&&  isprint (code[3]))
			fprintf (fp, " type=\"%c%c%c%c\"", code[0], code[1], code[2], code[3]);
		else
		{
			uint32 type;

			#ifdef __INTEL__
			type = ((0UL + (code[0])) << 24)
				+   ((0UL + (code[1])) << 16)
				+   ((0UL + (code[2])) << 8)
				+   ((0UL + (code[3])));
			#else
			type = ((0UL + (code[3])) << 24)
				+   ((0UL + (code[2])) << 16)
				+   ((0UL + (code[1])) << 8)
				+   ((0UL + (code[0)));
			#endif
	
			fprintf (fp, " type=\"lu\"", type);
		}
	}

	BString buffer (value);
	buffer.ReplaceAll ("&", "&amp;");
	buffer.ReplaceAll ("<", "&lt;");
	buffer.ReplaceAll (">", "&gt;");
	buffer.ReplaceAll ("\"", "&quot;");
	fprintf (fp, ">%s</string>\n", buffer.String());
}

void
write_xml_time (FILE *fp, const char *name,  bigtime_t value)
{
	fprintf (fp, "<time name=\"%s\">%Ld</time>\n",
		name, value);
}

void
write_xml_uint64 (FILE *fp, const char *name, uint64 value)
{
	fprintf (fp, "<uint64 name=\"%s\">%Lu</uint64>\n",
		name, value);
}

void
write_xml_uint32 (FILE *fp, const char *name, uint32 value)
{
	fprintf (fp, "<uint32 name=\"%s\">%lu</uint32>\n",
		name, value);
}

void
write_xml_uint16 (FILE *fp, const char *name, uint16 value)
{
	fprintf (fp, "<uint16 name=\"%s\">%u</uint16>\n",
		name, value);
}

void
write_xml_uint8 (FILE *fp, const char *name, uint8 value)
{
	unsigned int v (value);

	fprintf (fp, "<uint8 name=\"%s\">%u</uint8>\n",
		name, v);
}

void
write_xml_raw (
	FILE *fp,
	const char *name,
	const char *value,
	size_t size,
	int tabs,
	ReadControl *rcontrol,
	const char *code)
{
	fprintf (fp, "<raw name=\"%s\"", name);

	if (code)
		fprintf (fp, " type=\"%c%c%c%c\"", code[0], code[1], code[2], code[3]);
	
	fprintf (fp, ">\n");

	for (int t = 0; t < tabs + 1; ++t)
		fprintf (fp, "\t");

	for (size_t k = 0; k < size; ++k)
	{
		if (k != 0 && (k % rcontrol->width == 0))
		{
			fprintf (fp, "\n");
			for (int t = 0; t < tabs + 1; ++t)
				fprintf (fp, "\t");
		}

		int x (value[k] & 0xFF);
		fprintf (fp, "%02X", x);
	}
	fprintf (fp, "\n");
	for (int t = 0; t < tabs; ++t)
		fprintf (fp, "\t");
	fprintf (fp, "</raw>\n");
}

bool
write_xml_unhandled (
	FILE *fp,
	const char *name,
	uint32 type,
	const char *buffer,
	size_t size,
	int tabs,
	ReadControl *rcontrol)
{
	// first look for raw
	for (int32 i = 0; i < rcontrol->raws.CountItems(); ++i)
	{
		uint32 *rtype ((uint32 *)rcontrol->raws.ItemAt (i));

		if (*rtype == type)
		{
			char code[4];

			string_code (code, type);
			write_xml_raw (fp, name, buffer, size, tabs, rcontrol, code);
			return true;
		}
	}

	// now strings
	for (int32 i = 0; i < rcontrol->strings.CountItems(); ++i)
	{
		uint32 *stype ((uint32 *)rcontrol->strings.ItemAt (i));

		if (*stype == type)
		{
			char code[4];
			
			string_code (code, type);
			write_xml_string (fp, name, buffer, code);
			return true;
		}
	}

	unhandled_type (name, type);
	return false;
}

void
unhandled_type (const char *name, uint32 type)
{
	char code[4];

	string_code (code, type);
	fprintf (stderr, "Unhandled name:%s type:%lu ('%c%c%c%c')\n",
		name,
		type,
		code[0],
		code[1],
		code[2],
		code[3]);
}

void
string_code (char code[4], uint32 type)
{
	#ifdef __INTEL__
	code[0] = (type & 0xFF000000UL) >> 24;
	code[1] = (type & 0x00FF0000UL) >> 16;
	code[2] = (type & 0x0000FF00UL) >> 8;
	code[3] = type & 0x000000FFUL;
	#else
	code[0] = type & 0x000000FFUL;
	code[1] = (type & 0x0000FF00UL) >> 8;
	code[2] = (type & 0x00FF0000UL) >> 16;
	code[3] = (type & 0xFF000000UL) >> 24;
	#endif
}

ReadControl::ReadControl (void)
	: width (16L)
{
}

ReadControl::~ReadControl (void)
{
	for (int32 i = 0; i < raws.CountItems(); ++i)
		delete ((uint32 *)raws.ItemAt (i));

	for (int32 i = 0; i < strings.CountItems(); ++i)
		delete ((uint32 *)strings.ItemAt (i));
}

bool
ReadControl::AddType (BList *list, const char *arg)
{
	uint32 type (0UL);

	if (isdigit (*arg))
	{
		while (*arg && isdigit (*arg))
			type = type * 10 + *arg++ - '0';

		if (*arg) // non digit mixed with digit
			return false;
	}
	else
	{
		if (strlen (arg) != 4)
			return false;

		#ifdef __INTEL__
		type = ((0UL + (arg[0])) << 24)
			+   ((0UL + (arg[1])) << 16)
			+   ((0UL + (arg[2])) << 8)
			+   ((0UL + (arg[3])));
		#else
		type = ((0UL + (arg[3])) << 24)
			+   ((0UL + (arg[2])) << 16)
			+   ((0UL + (arg[1])) << 8)
			+   ((0UL + (arg[0)));
		#endif
	}

	list->AddItem (new uint32 (type));
	return true;
}

bool
ReadControl::AddRaw (const char *arg)
{
	return AddType (&raws, arg);
}

bool
ReadControl::AddString (const char *arg)
{
	return AddType (&strings, arg);
}

