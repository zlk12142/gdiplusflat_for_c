#include <regex.h>
#include <stdio.h>
#include "converter.h"

int main()
{
	converter_convert("GdiPlusEnums.h");
	converter_convert("GdiPlusColor.h");
	converter_convert("GdiplusTypes.h");
	return 0;
}
