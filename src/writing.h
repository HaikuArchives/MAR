
#ifndef MARWRITING_H_
#define MARWRITING_H_

class BFile;
struct MarControl;

void write_message_file (BFile *, BMessage *, bool);
void write_resource_file (BFile *, BMessage *, bool);
void write_attribute_file (BFile *, BMessage *, bool);

#endif
