
#define ROTOR_SZ 256

struct rotor
{
	char r_fwd[ROTOR_SZ];
	char r_back[ROTOR_SZ];
	int r_rotation;
};

struct reflector
{

};
#define ROTOR_NUM 3

struct enigma
{
	struct rotor e_rotors[ROTOR_NUM];
	char e_refl[ROTOR_SZ];
};

int enigma_gen(struct enigma *eng);
int rotor_gen(struct rotor *rtr);
int reflector_gen(char e_refl *rfl);
int transpos_gen(char *arr, int sz);


