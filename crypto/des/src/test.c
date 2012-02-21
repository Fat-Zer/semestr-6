#include "des.h"
#include <stdint.h>
#include "des_work.h"
#include "bit_reverse.h"

uint32_t des_f(uint32_t val, uint64_t k_i);

void print_block32(char* head, unsigned char block[])
{
	int i;
	printf("%10s : %02X", head ? head : "", block[0]);
	for(i=1;i<4;i++)
		printf("-%02X", block[i]);
	putchar('\n');
}

void print_block(char* head, unsigned char block[])
{
	int i;
	printf("%16s : %02X", head ? head : "", block[0]);
	for(i=1;i<8;i++)
		printf("-%02X", block[i]);
	putchar('\n');
}

int main()
{
	unsigned char key[8]   = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
	unsigned char block[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xe7};
	unsigned char tmp_block[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	unsigned char zero_block[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	uint64_t k_arr[16]={
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0};
	des_ctx ctx={
		{	0, 0, 0, 0, 0, 0, 0, 0, 	0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 	0, 0, 0, 0, 0, 0, 0, 0},
		{	0, 0, 0, 0, 0, 0, 0, 0, 	0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 	0, 0, 0, 0, 0, 0, 0, 0}
	};
	uint64_t tmp;
	int i;
	
	des_key(&ctx, key);
	des_gen_ks(des_byte2key(key), k_arr);
	
//	print_block("k[0] my", k_arr);
//	print_block("k[0] work", ctx.ek);
/*	tmp = des_f(0,k_arr[0]);
	print_block32("des_f(0,k[1])", &tmp);
	tmp = des_ssl_f(0,ctx.ek);
	print_block32("ssl_f(0,k[1])", &tmp);
	
	tmp = des_work_f(0,ctx.ek);
	print_block32("work_f(0,k[1])", &tmp);
*/	
	des_enc(&ctx, block, 1);
	print_block("Encrypted", block);
	des_dec(&ctx, block, 1);
	print_block("Decrypted", block);

	puts("===============================================");
	print_block("Plain", block);

	//*(uint64_t*)block = des_byte2key(block);
	des_block_crypt((uint64_t*)block, k_arr, DES_ENCRYPT_ACT);
	print_block("Encrypted", block);

	reverse_bit_buff(tmp_block, block, 8);
	print_block("Mirrored", tmp_block);

	for(i=0; i<8; i++) tmp_block[i]=reverse_bit_byte(block[i]);
	print_block("ByBitMirrored", tmp_block);

	des_block_crypt((uint64_t*)block, k_arr, DES_DECRYPT_ACT);
	print_block("Decrypted", block);

	return 0;
}

