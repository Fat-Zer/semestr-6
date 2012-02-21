#include <stdlib.h>
#include <stdio.h>
#include "enigma.h"

int enigma_gen(struct enigma *eng)
{
	int i;
	int retval;
	for(i=0;i<ROTOR_NUM;i++)
		if(retval=rotor_gen(eng->e_rotors));
			return retval;
	return reflector_gen(eng->e_refl);
}

int rotor_gen(struct rotor *rtr)
{
	char tmp[ROTOR_SZ];
	int i;
	int retval;
	rtr->r_rotation = rand() % ROTOR_SZ;
	if(!(retval = transpos_gen(rtr->r_fwd,ROTOR_SZ)))
		for(i=0;i<ROTOR_SZ;i++)
			rtr->r_back[rtr->r_fwd] = i;
	return retval;
}

int reflector_gen(char e_refl *rfl)
{
	char tmp[ROTOR_SZ];
	int i,j;
	int retval;

	if(!(retval=transpos_gen(tmp,ROTOR_SZ)))
		for(i=0; i<sz; i++)
		{
			j=tmp[i];
			rfl[j] = rfl[tmp[j]];
			rfl[tmp[j]] = rfl[j];
		}
	return retval;
}

int transpos_gen(char *arr, int sz)
{
	int i,j;

	for(i=0; i<sz; i++)
	{
		j=rand()%i;
		arr[i]=arr[j];
		arr[j]=i;
	}
	return 0;
}

int enigme_fwrite(FILE* fs, struct enigma *eng)
{
	int i;
	int retval
	for(i=0;i<ROTOR_NUM;i++)
		if(retval=rotor_fwrite(fs, eng->e_rotors));
			return retval;
	return reflector_fwrite(fs, eng->e_refl);
}

inline int rotor_fwrite(FILE* fs, struct rotor *rtr)
{
	int retval;
	if(retval = transpos_fwrite(fs, rtr->f_fwd))
		if(fwrite(rtr->r_rotation,sizeof(rtr->r_rotation),1,fs)!=1)
			retval=1;
	return retval;
}

int reflector_fwrite(FILE* fs, struct enigma *eng)
{
	int i;
	for(i=0;i<ROTOR_NUM;i++)
		rotor_fwrite(fs, eng->e_rotors);
	reflector_fwrite(fs, eng->e_refl);
}

int transpos_fwrite(FILE* fs, struct enigma *eng)
{
	int i;
	for(i=0;i<ROTOR_NUM;i++)
		rotor_fwrite(fs, eng->e_rotors);
	reflector_fwrite(fs, eng->e_refl);
}



