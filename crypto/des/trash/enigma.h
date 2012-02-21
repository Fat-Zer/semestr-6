#define ROTOR_SZ (0x100)

struct rotor
{
	unsigned char r_fwd[ROTOR_SZ];
	unsigned char r_back[ROTOR_SZ];
};

#define ROTOR_NUM (3)

struct enigma
{
	struct rotor e_rotors[ROTOR_NUM];
	unsigned char e_refl[ROTOR_SZ];
	int e_rotations[ROTOR_NUM];
};

int enigma_gen(struct enigma *eng);
int rotor_gen(struct rotor *rtr);
int reflector_gen(unsigned char *rfl);
int transpos_gen(unsigned char *arr, int sz);

int transpos_reverse(unsigned char *dst, unsigned char *src, size_t sz);

int enigma_fwrite(FILE* fs, struct enigma *eng);
int rotor_fwrite(FILE* fs, struct rotor *rtr);
int reflector_fwrite(FILE* fs, unsigned char* refl);
int transpos_fwrite(FILE* fs, unsigned char *transp, size_t transp_sz);

int enigma_fread(FILE* fs, struct enigma *eng);
int rotor_fread(FILE* fs, struct rotor *rtr);
int reflector_fread(FILE* fs, unsigned char* refl);
int transpos_fread(FILE* fs, unsigned char *transp, size_t transp_sz);

int enigma_verify(struct enigma *eng);
int rotor_verify(struct rotor *rtr);
int reflector_verify(unsigned char* refl);
int transpos_verify(unsigned char *transp, size_t transp_sz);

/** Verify that back transpos is reverse of fwd transpose.
 *
 *  @param back - back transpose
 *  @param fwd - forward transpose
 *  @param sz - size of the transpose
 */
int transpos_reverse_verify(unsigned char *back,
							unsigned char *fwd,
							size_t sz);

int enigma_fcrypt(struct enigma *eng, FILE *inf, FILE *outf);
int enigma_block_ncrypt(struct enigma *eng, unsigned char buf[], size_t sz);

unsigned char enigma_crypt(struct enigma *eng, unsigned char ch);

unsigned char apply_rotor_fwd(struct rotor *rtr, unsigned char ch);
unsigned char apply_rotor_bck(struct rotor *rtr, unsigned char ch);
unsigned char apply_reflector(unsigned char rfl[], unsigned char ch);

void enigma_rotate_rotors(struct enigma *eng, size_t n);

