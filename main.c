/**
 * \file
 *
 * \brief Application implement
 *
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <atmel_start.h>
#include <assert.h>
#include <string.h>
#include "hydrogen.h"


#define TEST_N_BYTES 32 

#define AESENCRYPT true
#define AESDECRYPT false


 #define CONTEXT "Example"
 #define ROTL(a,b) (((a) << (b)) | ((a) >> (32 - (b))))
 #define QR(a, b, c, d)( \
 b ^= ROTL(a + d, 7), \
 c ^= ROTL(b + a, 9), \
 d ^= ROTL(c + b,13), \
 a ^= ROTL(d + c,18))
 #define ROUNDS 20
 uint16_t counterTc;
 uint32_t C[4] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};
 uint32_t chacha_key[8]
 = {0x39443645,0x34383344,0x36393831, 0x36324437, 0x32334631, 0x34363145, 0x31363445, 0x43266325};
 uint32_t chacha_iv[2]
 = {0x00010203, 0x04050607};
 uint8_t chacha_plain_text[]
 = {0x52, 0x6f, 0x62, 0x65, 0x72, 0x74, 0x6f, 0x20, 0x49, 0x76, 0x61, 0x6e, 0x20, 0x42, 0x65, 0x72,
 0x6e, 0x61, 0x6c, 0x20, 0x44, 0x6f, 0x6d, 0x69, 0x6e, 0x67, 0x75, 0x65, 0x7a, 0x20, 0x4d, 0x45};
static uint8_t aes_iv[16]
= {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
0x0d, 0x0e, 0x0f}; 
 
 static uint8_t _iv[16] = {0x00};
 static uint8_t _output[TEST_N_BYTES] = {0x00};
 enum _test_mode_id { _ECB = 0, _CBC, _CFB8, _CFB16, _CFB32, _CFB64, _CFB128, _OFB,
 _CTR, _GCM, _CCM, _N_MODE };
 static struct _test_mode {
	 char * name;
	 uint8_t *text;
	 } test_mode[_N_MODE] = {{"ECB ", chacha_plain_text},
	 {"CBC ", chacha_plain_text},
	 {"CFB 8", chacha_plain_text},
	 {"CFB 16", chacha_plain_text},
	 {"CFB 32", chacha_plain_text},
	 {"CFB 64", chacha_plain_text},
	 {"CFB128", chacha_plain_text},
	 {"OFB ", chacha_plain_text},
	 {"CTR ", chacha_plain_text},
	 {"GCM ", chacha_plain_text},
	 {"CCM ", chacha_plain_text}};
	 static bool test_res[_N_MODE];
	 
 /*
	Test for hydrogen!!!!!--------------------------------------------
 -----------------------------------------------------------------------
 ---------------------------------
 */

 static const char *ctx = "libtests";
 static int
 streq(const char *expected, const char *found)
 {
	 if (strcmp(expected, found) != 0) {
		 fprintf(stderr, "Found: [%s]\n", found);
		 return 0;
	 }
	 return 1;
 }
 #define assert_streq(EXPECTED, FOUND) assert(streq((EXPECTED), (FOUND)))




	/*ENDS HERE hydrogen!!!!!!!!!!!!--------------------
------------------------------------------------------------------------------
----------------------------------------------------------------------------------*/

/*
	AES module from microcontroller

*/
void dump_buffer(uint8_t *buf, uint32_t size) {
uint32_t i; 
if (!size) { 
 return; 
} 
for (i = 0; i < size; i++) { 
 printf(" 0x%02x,", buf[i]); 
 if ((i % 8) == 7 && i != size - 1) { 
 printf("\r\n"); 
 } 
} 
printf("\r\n"); 
} 
bool test_crypt(uint8_t mode, bool enc, uint8_t *sign ) 
{ 
int32_t rc = 0; 
uint32_t i = 0; 
uint8_t *input; 
uint32_t off; 
printf("\n-----------------------\r\n"); 
delay_ms(50); 
printf("AES %s %s test:\r\n", test_mode[mode].name, enc ? "encrypt" :
"decrypt"); 
delay_ms(50); 
if (enc) { 
 aes_sync_set_encrypt_key(&CRYPTOGRAPHY_0, sign, AES_KEY_128); 
 input = test_mode[mode].text; 
 
} else { 
 aes_sync_set_decrypt_key(&CRYPTOGRAPHY_0, sign, AES_KEY_128); 
 input = _output; 
 
} 
printf("> start ... "); 
delay_ms(50); 
off = 0; 
memcpy(_iv, aes_iv, 16); 
rc = aes_sync_cfb128_crypt(&CRYPTOGRAPHY_0, enc, &input[i],
&_output[i], 32, _iv, &off); //AES cypher 128
 
 if (rc < 0) { 
 printf("Crypt error!\r\n"); 
 return false; 
 } 
printf("OK\r\n"); 
delay_ms(50); 
return true; 
} 

/*
	End of AES module
*/

 void chacha20_block(uint32_t out[16], uint32_t const in[16]) {
	 int i;
	 uint32_t x[16];

	 for (i = 0; i < 16; ++i) x[i] = in[i];
	 // 10 loops ×2 rounds/loop= 20 rounds
	 for (i = 0; i < ROUNDS; i += 2) {
		 // Odd round
		 QR(x[0], x[4], x[8], x[12]); // column 1
		 QR(x[5], x[9], x[13], x[1]); // column 2
		 QR(x[10], x[14], x[2], x[6]); // column 3
		 QR(x[15], x[3], x[7], x[11]);// column 4
		 // Even round
		 QR(x[0], x[1], x[2], x[3]); // row 1
		 QR(x[5], x[6], x[7], x[4]); // row 2
		 QR(x[10], x[11], x[8], x[9]); // row 3
		 QR(x[15], x[12], x[13], x[14]); // row 4

	 }
	 for (i = 0; i < 16; ++i) out[i] = x[i] + in[i];

 }


 void initBlockChacha (uint32_t key[8], uint32_t nonce[2], uint32_t stateBlock[16] )
 {
	 /* First 4 spaces are constants */
	 
	 stateBlock[0] =  C[0];
	 stateBlock[1] =  C[1];
	 stateBlock[2] =  C[2];
	 stateBlock[3] =  C[3];
	 
	 /* Next 8 spaces are the keys divided in 32 bit blocks*/
	 stateBlock[4] = key[0];
	 stateBlock[5] = key[1];
	 stateBlock[6] = key[2];
	 stateBlock[7] = key[3];
	 
	 stateBlock[8] = key[4];
	 stateBlock[9] = key[5];
	 stateBlock[10] = key[6];
	 stateBlock[11] = key[7];
	 /*Next two spaces are a counter*/
	 stateBlock[12] = 0;
	 stateBlock[13] = 0;
	 
	 /*Next two spaces are an iv*/
	 stateBlock[14] = chacha_iv[0];
	 stateBlock[15] = chacha_iv[1];
	 
	 for(int i = 0; i < 16; i++){
		 printf("State Block [%d] = 0x%08x \n", i, stateBlock[i]);
	 }

 }


 void cypher_Chacha20(uint32_t stateBlock[16], uint8_t const *bytes, uint8_t *output, size_t length)
 {
	 uint32_t auxBlock[16];
	 uint8_t *auxStateBlock;
	 uint8_t decrypt[40];
	 uint8_t counterPosition = 0;
	 //printf("arrived here");
	  printf("value encrypted chacha20: \n ");
	 for( int i = 0;i < length; i++)
	 {
		 if(counterPosition < 64)
		 {
			 counterPosition++;
		 }
		 else
		 {   counterPosition = 0;
			 if(stateBlock[12] == 0xffffffff)
			 {
				 stateBlock[12] = 0;
				 if(stateBlock[13] == 0xffffffff)
				 {
					 stateBlock[13] = 0;
				 }
				 else
				 {
					 stateBlock[13]++;
				 }
			 }
			 else
			 {
				 stateBlock[12]++;
			 }
			 
		 }
		 chacha20_block(auxBlock, stateBlock);
		 auxStateBlock =  (uint8_t *) stateBlock;
		 output[i] = bytes[i] ^ auxStateBlock[i];
		 printf(" 0x%02x ", output[i] );
	 }
	 
	 printf("\n value decrypted chacha20: \n ");
	 for( int i = 0;i < length; i++)
	 {
		 chacha20_block(auxBlock, stateBlock);
		 auxStateBlock =  (uint8_t *) stateBlock;
		 decrypt[i] = output[i] ^ auxStateBlock[i];
		 printf(" %c ", decrypt[i]);
	 }
 }





int main(void)
{
	uint32_t encryptBlock[16];
	uint8_t auxBlock[40];
	atmel_start_init();
    
	//initBlockChacha(chacha_key, chacha_iv, encryptBlock);
	//cypher_Chacha20(encryptBlock, chacha_plain_text, auxBlock, sizeof(chacha_plain_text));

	
	//delay_ms(50);
	/*
	
	Testing hydrogen
	
	
	*/
	#ifdef _MSC_VER
    /*
     * On Windows, disable the "Abort - Retry - Ignore" GUI dialog that otherwise pops up on
     * assertion failure.
     */
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
     #endif

    int ret;

    ret = hydro_init();
    assert(ret == 0);
	hydro_sign_keypair key_pair;
	hydro_sign_keygen(&key_pair);
    uint8_t signature[hydro_sign_BYTES];
	hydro_sign_create(signature, chacha_plain_text, sizeof(chacha_plain_text), CONTEXT, key_pair.sk);
	int verifyHydro = hydro_sign_verify(signature, chacha_plain_text, sizeof(chacha_plain_text), CONTEXT, key_pair.pk);
	
	printf("Signature: ");
	for(int i = 0; i < hydro_sign_BYTES; i ++)
	{
		printf("0x%.2X ", signature[i]);
		delay_ms(10);
	}
	
	printf("\n");
	delay_ms(10);
	
	
	if (verifyHydro != 0) {
		/* forged */
	}
	else
	{
		initBlockChacha(signature, chacha_iv, encryptBlock);
	    cypher_Chacha20(encryptBlock, chacha_plain_text, auxBlock, sizeof(chacha_plain_text));
		
		
		
		aes_sync_enable(&CRYPTOGRAPHY_0);
		delay_ms(50); 
		test_res[6] = test_crypt(6, AESENCRYPT, signature );
		delay_ms(10);
		printf("Text encrypted AES: \n"); 
		delay_ms(10);
		for(int i = 0; i < sizeof(_output); i++ ){
			printf("0x%.2X ", _output[i]);
			delay_ms(10);
		}
		
		test_res[6] = test_crypt(6, AESDECRYPT, signature );
		delay_ms(10);
		printf("\n Text decrypted in hex: \n");
		delay_ms(10);
		for(int i = 0; i < sizeof(_output); i++ ){
			printf("0x%.2X ", _output[i]);
			delay_ms(10);
		}
		printf("\n Text decrypted in string: \n");
		delay_ms(10);
		for(int i = 0; i < sizeof(_output); i++ ){
			printf("%c ", _output[i]);
			delay_ms(10);
		}
		
		
		
	}
	/*
	 End testing hydrogen
	
	*/

	while (1) {
	}
	return 0;
}
