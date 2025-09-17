//! -O1 -fcf-protection=none
extern "C" {

__attribute__((section (".text.addfunction")))
extern const unsigned char add[] =
	{ 0x8d, 0x04, 0x37, 0xc3 };

}
