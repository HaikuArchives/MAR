
#include <File.h>
#include <Resources.h>
#include <Message.h>
#include <List.h>
#include <String.h>

#include <map>
#include <list>

#include "writing.h"

void
write_message_file (BFile *file, BMessage *msg, bool first)
{
	if (first)
		file->SetSize (0LL);
	msg->Flatten (file);
}

void
write_resource_file (BFile *file, BMessage *msg, bool first)
{
	char *name;
	uint32 type;
	int32 count;
	size_t len;
	int32 id;

	BResources *res;

	if (first)
		res = new BResources (file, true);
	else
		res = new BResources (file, false);

	map<uint32, int32> ids;

	for (int32 i = 0; msg->GetInfo (B_ANY_TYPE, i, &name, &type, &count) == B_NO_ERROR; ++i)
	{
		for (int32 j = 0; j < count; ++j)
		{
			if (type == 'ICON')
				id = 101;
			else if (type == 'MICN')
				id = 101;
			else
			{
				map<uint32, int32>::iterator it (ids.find (type));

				if (it == ids.end())
					ids[type] = id = 0;
				else
					id = it->second += 1;
			}

			switch (type)
			{
				case B_BOOL_TYPE:
				{
					bool value (msg->FindBool (name, j));

					res->AddResource (
						type,
						id,
						&value,
						sizeof (bool),
						name);
					break;
				}

				case B_DOUBLE_TYPE:
				{
					double value (msg->FindDouble (name, j));

					res->AddResource (
						type,
						id,
						&value,
						sizeof (double),
						name);
					break;
				}

				case B_FLOAT_TYPE:
				{
					float value (msg->FindFloat (name, j));

					res->AddResource (
						type,
						id,
						&value,
						sizeof (float),
						name);
					break;
				}

				case B_INT64_TYPE:
				{
					int64 value (msg->FindInt64 (name, j));

					res->AddResource (
						type,
						id,
						&value,
						sizeof (int64),
						name);
					break;
				}

				case B_INT32_TYPE:
				{
					int32 value (msg->FindInt32 (name, j));

					res->AddResource (
						type,
						id,
						&value,
						sizeof (int32),
						name);
					break;
				}

				case B_INT16_TYPE:
				{
					int16 value (msg->FindInt16 (name, j));
					
					res->AddResource (
						type,
						id,
						&value,
						sizeof (int16),
						name);
					break;
				}

				case B_INT8_TYPE:
				{
					int8 value (msg->FindInt8 (name, j));

					res->AddResource (
						type,
						id,
						&value,
						sizeof (int8),
						name);
					break;
				}

				case B_MESSAGE_TYPE:
				{
					BMessage sub;

					msg->FindMessage (name, j, &sub);

					char *buffer;
					ssize_t msize;

					msize = sub.FlattenedSize();
					buffer = new char [msize];
					sub.Flatten (buffer, msize);

					res->AddResource (
						type,
						id,
						buffer,
						msize,
						name);

					delete [] buffer;
					break;
				}

				/* Not supported in resources
				case B_MESSENGER_TYPE:
					break;
				*/

				/* not supported in resources
				case B_POINTER_TYPE:
					break;
				*/

				case B_POINT_TYPE:
				{
					BPoint value (msg->FindPoint (name, j));

					res->AddResource (
						type,
						id,
						&value,
						sizeof (BPoint),
						name);
					break;
				}

				case B_RECT_TYPE:
				{
					BRect value (msg->FindRect (name, j));

					res->AddResource (
						type,
						id,
						&value,
						sizeof (BRect),
						name);
					break;
				}

				case B_STRING_TYPE:
				{
					const char *buffer (msg->FindString (name, j));

					res->AddResource (
						type,
						id,
						buffer,
						strlen (buffer) + 1,
						name);
					break;
				}

				case B_MIME_TYPE:
				case B_OFF_T_TYPE:
				case B_REF_TYPE:
				case B_RGB_COLOR_TYPE:
				case B_SIZE_T_TYPE:
				case B_SSIZE_T_TYPE:
				case B_TIME_TYPE:
				case B_UINT64_TYPE:
				case B_UINT32_TYPE:
				case B_UINT16_TYPE:
				case B_UINT8_TYPE:
				case B_RAW_TYPE:
				default:
				{
					const void *value;
					ssize_t msize;

					msg->FindData (
						name,
						type,
						j,
						&value,
						&msize);

					res->AddResource (
						type,
						id,
						value,
						msize,
						name);
					break;
				}
			}
		}
	}

	delete res;
}

void
write_attribute_file (BFile *file, BMessage *msg, bool first)
{
	if (first)
	{
		char name[B_ATTR_NAME_LENGTH];
		list<BString> names;

		file->RewindAttrs();
		while (file->GetNextAttrName (name) == B_NO_ERROR)
			names.push_back (name);

		list<BString>::iterator it;

		for (it = names.begin(); it != names.end(); ++it)
			file->RemoveAttr (it->String());
	}

	char *name;
	uint32 type;
	int32 count;

	for (int32 i = 0; msg->GetInfo (B_ANY_TYPE, i, &name, &type, &count) == B_NO_ERROR; ++i)
	{
		for (int32 j = 0; j < count; ++j)
		{
			switch (type)
			{
				case B_BOOL_TYPE:
				{
					bool value (msg->FindBool (name, j));

					file->WriteAttr (
						name,
						type,
						0,
						&value,
						sizeof (bool));
					break;
				}

				case B_DOUBLE_TYPE:
				{
					double value (msg->FindDouble (name, j));

					file->WriteAttr (
						name,
						type,
						0,
						&value,
						sizeof (double));
					break;
				}

				case B_FLOAT_TYPE:
				{
					float value (msg->FindFloat (name, j));

					file->WriteAttr (
						name,
						type,
						0,
						&value,
						sizeof (float));
					break;
				}

				case B_INT64_TYPE:
				{
					int64 value (msg->FindInt64 (name, j));

					file->WriteAttr (
						name,
						type,
						0,
						&value,
						sizeof (int64));
					break;
				}

				case B_INT32_TYPE:
				{
					int32 value (msg->FindInt32 (name, j));

					file->WriteAttr (
						name,
						type,
						0,
						&value,
						sizeof (int32));
					break;
				}

				case B_INT16_TYPE:
				{
					int16 value (msg->FindInt16 (name, j));
					
					file->WriteAttr (
						name,
						type,
						0,
						&value,
						sizeof (int16));
					break;
				}

				case B_INT8_TYPE:
				{
					int8 value (msg->FindInt8 (name, j));

					file->WriteAttr (
						name,
						type,
						0,
						&value,
						sizeof (int8));
					break;
				}

				case B_MESSAGE_TYPE:
				{
					BMessage sub;

					msg->FindMessage (name, j, &sub);

					char *buffer;
					ssize_t msize;

					msize = sub.FlattenedSize();
					buffer = new char [msize];
					sub.Flatten (buffer, msize);

					file->WriteAttr (
						name,
						type,
						0,
						buffer,
						msize);

					delete [] buffer;
					break;
				}

				/* Not supported in resources
				case B_MESSENGER_TYPE:
					break;
				*/

				/* not supported in resources
				case B_POINTER_TYPE:
					break;
				*/

				case B_POINT_TYPE:
				{
					BPoint value (msg->FindPoint (name, j));

					file->WriteAttr (
						name,
						type,
						0,
						&value,
						sizeof (BPoint));
					break;
				}

				case B_RECT_TYPE:
				{
					BRect value (msg->FindRect (name, j));

					file->WriteAttr (
						name,
						type,
						0,
						&value,
						sizeof (BRect));
					break;
				}

				case B_STRING_TYPE:
				{
					const char *buffer (msg->FindString (name, j));

					file->WriteAttr (
						name,
						type,
						0,
						buffer,
						strlen (buffer) + 1);
					break;
				}

				case B_MIME_TYPE:
				case B_OFF_T_TYPE:
				case B_REF_TYPE:
				case B_RGB_COLOR_TYPE:
				case B_SIZE_T_TYPE:
				case B_SSIZE_T_TYPE:
				case B_TIME_TYPE:
				case B_UINT64_TYPE:
				case B_UINT32_TYPE:
				case B_UINT16_TYPE:
				case B_UINT8_TYPE:
				case B_RAW_TYPE:
				default:
				{
					const void *value;
					ssize_t msize;

					msg->FindData (
						name,
						type,
						j,
						&value,
						&msize);

					file->WriteAttr (
						name,
						type,
						0,
						value,
						msize);
					break;
				}
			}
		}
	}
}
